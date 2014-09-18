#include "odTransport.hpp"
#include "dataLink.hpp"
#include "boostly.hpp"
#include <stdio.h>

// Get OD entry sizes

#define ENUM_VAL0(name, size) size
#define ENUM_VAL(name, size) size
#define ENUM_CONSTANT(name, value) 0

uint8_t odSize[] = {
#include "od_vals.h"
};

#undef ENUM_VAL0
#undef ENUM_VAL
#undef ENUM_CONSTANT

// Hold this lock for the duration of a send - response.
// E.g. we don't want to send a second request in the middle of retrying.
// If we add more transports, this mutex should be shared by them all.
// Only one transaction at a time regardless of the transport.
// The MPIC isn't implemented to do multiple things at once.
// So, come to think of it, this mutex may belong in the Data Link layer.
boostly::Mutex gTransportMutex;

// We have to provide one buffer so we can hold on to the responses
// after the responseCallback returns.
static dl::Buffer ourResponseBuffer;

// Use this mutex for accessing gotResponse and pResponseBuffer
boostly::Mutex gResponseMutex;
bool gGotResponse;
static dl::Buffer *gpResponseBuffer;
// Signal from receive thread to whoever sent a packet,
// the response is come.
boostly::Condition gResponseSignal;

// Mutex for accessing telemetry related data
boostly::Mutex gTelemetryMutex;
// Telemetry names, values, number of items
odt::OdName *gTelemetryNames;
uint32_t *gTelemetryValues;
unsigned gTelemetryNItems;
odt::TelemetryCallback *gpTelemetryCallback;

// Error statistics
odt::Statistics gStat;

// forward declarations
static dl::Buffer *responseCallback(dl::Buffer *message);
static dl::Buffer *telemetryCallback(dl::Buffer *message);
static bool sendReceive(dl::Buffer &buf, odt::OdName *names,
        uint32_t *outValues, unsigned n);
static bool parseValues(uint8_t seq, dl::Buffer *pBuf, odt::OdName *names,
        uint32_t *outValues, unsigned n);

void odt::init() {
    gpResponseBuffer = &ourResponseBuffer;
    gGotResponse = false;
    dl::registerCallback(responseCallback, dl::COMMAND_CHANNEL);
    dl::registerCallback(telemetryCallback, dl::TELEMETRY_CHANNEL);
    gStat.badSequence = 0;
    gStat.badLength = 0;
    gStat.timeout = 0;
}

//
// Accessing the Object Dictionary
//

bool odt::putMultiple(OdName *names, uint32_t *values, unsigned n) {
    dl::Buffer request;
    request.contentLength(0);
    for (unsigned i = 0; i < n; i++) {
        uint16_t putCmd = names[i];
        putCmd |= (odSize[names[i]] == 2 ? 0x4000 : 0x0000);
        request.add16(putCmd);
        if (odSize[names[i]] == 2) {
            request.add16(values[i]);
        } else {
            request.add32(values[i]);
        }
    }
    // We expect to receive no values, hence the NULLs and zero.
    return sendReceive(request, NULL, NULL, 0);
}

bool odt::getMultiple(OdName *names, uint32_t *outValues, unsigned n) {
    dl::Buffer request;
    request.contentLength(0);
    for (unsigned i = 0; i < n; i++) {
        request.add16(odGetCommand(names[i]));
    }
    return sendReceive(request, names, outValues, n);
}

// Note that names and outValues are what we expect to get in the response.
static bool sendReceive(dl::Buffer &buf, odt::OdName *names, uint32_t
        *outValues, unsigned n) {
    boostly::Guard guard(gTransportMutex);
    // discard any pending response TODO is this the right place?
    // TODO Shouldn't we grab gResponseMutex?
    gGotResponse = false;
    // If we're not getting any values back then we want an ACK.
    uint8_t seq = dl::send(buf, dl::COMMAND_CHANNEL, n == 0);
    // wait for response and parse it
    bool timedOut = false;
    while (!timedOut) {
        boostly::Time deadline;
        deadline.setToNow();
        deadline.addSeconds(1);
        boostly::Guard responseGuard(gResponseMutex);
        while (!gGotResponse && !timedOut) {
            timedOut = gResponseSignal.timed_wait(responseGuard, deadline);
        }
        if (gGotResponse) {
            return parseValues(seq, gpResponseBuffer, names, outValues, n);
        }
    }
    printf("Base response timeout.\n");
    gStat.timeout++;
    return true;
}

static bool parseValues(uint8_t seq, dl::Buffer *pBuf, odt::OdName *names,
        uint32_t *outValues, unsigned n) {
    if (seq != pBuf->sequence()) {
        printf("Bad sequence number %u (expected %u)\n", pBuf->sequence(), seq);
        gStat.badSequence++;
        return true;
    }
    unsigned expectedLength = 0;
    for (unsigned i = 0; i < n; i++) {
        expectedLength += odSize[names[i]];
    }
    if (expectedLength != pBuf->contentLength()) {
        printf("Bad response length %u (expected %u)\n",
                pBuf->contentLength(), expectedLength);
        gStat.badLength++;
        return true;
    }
    printf("parseValues(): raw content is");
    for (unsigned i = 0; i < pBuf->contentLength(); i++) {
        printf(" %02x", pBuf->content()[i]);
    }
    printf("\n");
    // We could also check if n == 0 then pBuf->flagIsAck() should be true.
    uint8_t *p = pBuf->content();
    for (unsigned i = 0; i < n; i++) {
        outValues[i] = *p++;
        outValues[i] |= *p++ << 8;
        if (odSize[names[i]] != 2) {
            outValues[i] |= *p++ << 16;
            outValues[i] |= *p++ << 24;
        }
    }
    return false;
}

static dl::Buffer *responseCallback(dl::Buffer *message) {
    boostly::Guard responseGuard(gResponseMutex);
    gGotResponse = true;
    dl::Buffer *tmp = gpResponseBuffer;
    gpResponseBuffer = message;
    printf("Got a response packet.\n");
    gResponseSignal.notify();
    return tmp;
}

uint16_t odt::odGetCommand(OdName name) {
    uint16_t cmd = name;
    if (odSize[name] == 2)
        cmd |= 0xc000;
    else
        cmd |= 0x8000;
    return cmd;
}

//
// Access EEPROM
//

// Write the contents of EEPROM staging to the actual EEPROM.
// Returns error codes from OD_EEPROM_ERROR, 0 == OK.
// Caller waits several seconds.
unsigned odt::eeWrite() {
    bool berr = put(OD_EEPROM_ERROR, 0);
    if (berr) return 100;
    struct timespec tenthSecond = {
        0,   // seconds
        10 * 1000 * 1000  // nanoseconds
    };
    // Stage 1
    struct timespec ts1;
    int ierr = clock_gettime(CLOCK_REALTIME, &ts1);
    if (ierr) return 101;
    berr = put(OD_EEPROM_CTRL, 0xaa05);
    if (berr) return 102;
    struct timespec ts2;
    ierr = clock_gettime(CLOCK_REALTIME, &ts2);
    if (ierr) return 103;
    unsigned count = 0;
    while (1) {
        if (0 == get(OD_EEPROM_CTRL)) break;
        if (count > 20) return 104;
        nanosleep(&tenthSecond, NULL);
        count++;
    }

    // Stage 2
    struct timespec ts3;
    ierr = clock_gettime(CLOCK_REALTIME, &ts3);
    if (ierr) return 105;
    berr = put(OD_EEPROM_CTRL, 0x505a);
    if (berr) return 106;
    struct timespec ts4;
    ierr = clock_gettime(CLOCK_REALTIME, &ts4);
    if (ierr) return 107;
    count = 0;
    while (1) {
        if (0 == get(OD_EEPROM_CTRL)) break;
        if (count > 20) return 108;
        nanosleep(&tenthSecond, NULL);
        count++;
    }

    // Stage 3
    struct timespec ts5;
    ierr = clock_gettime(CLOCK_REALTIME, &ts5);
    if (ierr) return 109;
    berr = put(OD_EEPROM_CTRL, 0xc5a0);
    if (berr) return 110;
    struct timespec ts6;
    ierr = clock_gettime(CLOCK_REALTIME, &ts6);
    if (ierr) return 111;
    struct timespec oneSecond = { 1, 0 };
    nanosleep(&oneSecond, NULL);
    count = 0;
    while (1) {
        if (0 == get(OD_EEPROM_CTRL)) break;
        if (count > 20) return 112;
        nanosleep(&tenthSecond, NULL);
        count++;
    }
    struct timespec ts7;
    ierr = clock_gettime(CLOCK_REALTIME, &ts7);
    if (ierr) return 113;

    // Wrap it up
    unsigned eeprom_err = get(OD_EEPROM_ERROR);
    if (0 == eeprom_err) {
        nanosleep(&oneSecond, NULL);
        put(OD_EEPROM_CTRL, 0x1234);   // force dictionary to repopulate
        nanosleep(&oneSecond, NULL);
        nanosleep(&oneSecond, NULL);
    }
    float delta;
    delta = (ts2.tv_nsec - ts1.tv_nsec) * 0.000000001;
    delta += ts2.tv_sec - ts1.tv_sec;
    printf("Write first command: %f\n", delta);
    delta = (ts3.tv_nsec - ts2.tv_nsec) * 0.000000001;
    delta += ts3.tv_sec - ts2.tv_sec;
    printf("Pause: %f\n", delta);
    delta = (ts4.tv_nsec - ts3.tv_nsec) * 0.000000001;
    delta += ts4.tv_sec - ts3.tv_sec;
    printf("Write second command: %f\n", delta);
    delta = (ts5.tv_nsec - ts4.tv_nsec) * 0.000000001;
    delta += ts5.tv_sec - ts4.tv_sec;
    printf("Pause: %f\n", delta);
    delta = (ts6.tv_nsec - ts5.tv_nsec) * 0.000000001;
    delta += ts6.tv_sec - ts5.tv_sec;
    printf("Write third command: %f\n", delta);
    delta = (ts7.tv_nsec - ts6.tv_nsec) * 0.000000001;
    delta += ts7.tv_sec - ts6.tv_sec;
    printf("Final pause: %f\n", delta);
    return eeprom_err;
}

//
// Telemetry
// 

static dl::Buffer *telemetryCallback(dl::Buffer *message) {
    // TODO implement
    printf("Got a telemetry packet.\n");
    return message;
}

bool odt::setUpTelemetry(OdName *names, uint32_t *outValues, unsigned n,
        unsigned interval, TelemetryCallback *callback) {
    bool err = stopTelemetry();
    if (err) {
        printf("setUpTelemetry(): Failed to stop telemetry.\n");
        return true;
    }
    uint32_t max = get(OD_TELEMETRY_MAX);
    if (max == 0xffff) {
        printf("setUpTelemetry(): Failed to get TELEMETRY_MAX.\n");
        return true;
    }
    if (n > max) {
        printf("setUpTelemetry(): Too many telemetry entries.\n");
        return true;
    }
    boostly::Guard guard(gTelemetryMutex);
    // TODO implement
    return true;  // as long as it's incomplete
}

bool odt::stopTelemetry() {
    return put(OD_TELEMETRY_INTERVAL, 0);
}

void odt::getStatistics(Statistics &stat) {
    stat = gStat;
}

