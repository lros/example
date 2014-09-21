#include "odTransport.hpp"
#include "dataLink.hpp"
#include <boost/chrono.hpp>
#include <boost/thread.hpp>

#if defined RDA_CONFIG_debug
    #include <stdio.h>
#else
    #undef DEBUG_DATA
    #undef DEBUG_EEPROM
#endif

static uint16_t od_v6_map[odt::NUM_OD_ENTRIES];
static uint8_t od_v6_size[odt::NUM_OD_ENTRIES];

static uint16_t od_v7_map[odt::NUM_OD_ENTRIES];
static uint8_t od_v7_size[odt::NUM_OD_ENTRIES];

static void initTables() {
    unsigned i;
    for (i = 0; i < odt::NUM_OD_ENTRIES; i++) {
        od_v6_map[i] = 0xffff;
        od_v6_size[i] = 0xff;
        od_v7_map[i] = 0xffff;
        od_v7_size[i] = 0xff;
    }

    // Slurp in the Object Dictionary version 6:
    #define ENUM_VAL0(name, size) \
            i = 0; od_v6_map[odt::name] = i; od_v6_size[odt::name] = size
    #define ENUM_VAL(name, size) \
            i++; od_v6_map[odt::name] = i; od_v6_size[odt::name] = size
    #define ENUM_CONSTANT(name, value) 0
    #include "od_vals_v6.h"
    #undef ENUM_VAL0
    #undef ENUM_VAL
    #undef ENUM_CONSTANT
    ;  // The stuff above ends without a semicolon.

    // Slurp in the Object Dictionary version 7:
    #define ENUM_VAL0(name, size) \
            i = 0; od_v7_map[odt::name] = i; od_v7_size[odt::name] = size
    #define ENUM_VAL(name, size) \
            i++; od_v7_map[odt::name] = i; od_v7_size[odt::name] = size
    #define ENUM_CONSTANT(name, value) 0
    #include "od_vals_v7.h"
    #undef ENUM_VAL0
    #undef ENUM_VAL
    #undef ENUM_CONSTANT
    ;  // The stuff above ends without a semicolon.
}

#if defined DEBUG_DATA
    #define errPrint(...) fprintf(stderr, __VA_ARGS__)
#else
    #define errPrint(...) do { } while (0)
#endif

// These point to tables for the MPIC's current version.
// Set by setOdVersion().
static uint16_t *odIndex;
static uint8_t *odSize;

// Sorry, I have trouble with these verbose Boost type names.
// Also I find the term "Condition Variable" to be meaningless.
typedef boost::mutex boostMutex;
typedef boost::condition_variable boostEvent;
typedef boost::lock_guard<boost::mutex> boostGuard;
// You need to hold a WaitGuard in order to wait on an Event.
typedef boost::unique_lock<boost::mutex> boostWaitGuard;
typedef boost::chrono::steady_clock::time_point boostAbsTime;

// Hold this lock for the duration of a send - response.
// E.g. we don't want to send a second request in the middle of retrying.
// If we add more transports, this mutex should be shared by them all.
// Only one transaction at a time regardless of the transport.
// The MPIC isn't implemented to do multiple things at once.
// So, come to think of it, this mutex may belong in the Data Link layer.
boostMutex gTransportMutex;

// We have to provide one buffer so we can hold on to the responses
// after the responseCallback returns.
static dl::Buffer ourResponseBuffer;

// Use this mutex for accessing gotResponse and pResponseBuffer
boostMutex gResponseMutex;
bool gGotResponse;
static dl::Buffer *gpResponseBuffer;
// Signal from receive thread to whoever sent a packet,
// that the response is come.
boostEvent gResponseEvent;

// Mutex for accessing telemetry related data
boostMutex gTelemetryMutex;
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
static uint16_t odGetCommand(odt::OdName name);

void odt::init() {
    initTables();
    gpResponseBuffer = &ourResponseBuffer;
    gGotResponse = false;
    dl::registerCallback(responseCallback, dl::COMMAND_CHANNEL);
    dl::registerCallback(telemetryCallback, dl::TELEMETRY_CHANNEL);
    gStat.badSequence = 0;
    gStat.badLength = 0;
    gStat.timeout = 0;
}

bool odt::setOdVersion() {
    // In all OD versions, OD_VERSION is the same so pick a version.
    odIndex = od_v6_map;
    odSize = od_v6_size;
    unsigned version = get(OD_VERSION);
    if (version == 0xffff) return true;
    switch (version) {
        case 6:
            odIndex = od_v6_map;
            odSize = od_v6_size;
            printf("Using version 6 object dictionary.\n");
            break;
        case 7:
            odIndex = od_v7_map;
            odSize = od_v7_size;
            printf("Using version 7 object dictionary.\n");
            break;
        default:
            return true;
    }
    return false;
}

//
// Accessing the Object Dictionary
//

bool odt::putMultiple(OdName *names, uint32_t *values, unsigned n) {
    dl::Buffer request;
    request.contentLength(0);
    for (unsigned i = 0; i < n; i++) {
        uint16_t putCmd = odIndex[names[i]];
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
    boostGuard transportGuard(gTransportMutex);
    {
        // discard any pending response (shouldn't happen)
        boostGuard responseGuard(gResponseMutex);
        gGotResponse = false;
    }
    // If we're not getting any values back then we want an ACK.
    uint8_t seq = dl::send(buf, dl::COMMAND_CHANNEL, n == 0);
    // wait for response and parse it
    bool timedOut = false;
    while (!timedOut) {
        boostAbsTime deadline = boost::chrono::steady_clock::now()
            + boost::chrono::milliseconds(100);
        boostWaitGuard responseGuard(gResponseMutex);
        while (!gGotResponse && !timedOut) {
            timedOut = gResponseEvent.wait_until(responseGuard, deadline);
        }
        if (gGotResponse) {
            gGotResponse = false;
            return parseValues(seq, gpResponseBuffer, names, outValues, n);
        }
    }
    errPrint("Base response timeout.\n");
    gStat.timeout++;
    return true;
}

static bool parseValues(uint8_t seq, dl::Buffer *pBuf, odt::OdName *names,
        uint32_t *outValues, unsigned n) {
    if (seq != pBuf->sequence()) {
        errPrint("Bad sequence number %u (expected %u)\n",
                pBuf->sequence(), seq);
        gStat.badSequence++;
        return true;
    }
    unsigned expectedLength = 0;
    for (unsigned i = 0; i < n; i++) {
        expectedLength += odSize[names[i]];
    }
    if (expectedLength != pBuf->contentLength()) {
        errPrint("Bad response length %u (expected %u)\n",
                pBuf->contentLength(), expectedLength);
        gStat.badLength++;
        return true;
    }
    #if defined DEBUG_DATA
        printf("parseValues(): raw content is");
        for (unsigned i = 0; i < pBuf->contentLength(); i++) {
            printf(" %02x", pBuf->content()[i]);
        }
        printf("\n");
    #endif
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
    boostGuard responseGuard(gResponseMutex);
    gGotResponse = true;
    dl::Buffer *tmp = gpResponseBuffer;
    gpResponseBuffer = message;
    gResponseEvent.notify_one();
    return tmp;
}

static uint16_t odGetCommand(odt::OdName name) {
    uint16_t cmd = odIndex[name];
    if (odSize[name] == 2)
        cmd |= 0xc000;
    else
        cmd |= 0x8000;
    return cmd;
}

//
// Access EEPROM
//

#if defined DEBUG_EEPROM
    #define GETTIME(ts, err) \
        if (clock_gettime(CLOCK_REALTIME, &ts)) return err

    static float delta(timespec &t2, timespec &t1) {
        float result = (t2.tv_nsec - t1.tv_nsec) * 0.000000001;
        result += t2.tv_sec - t1.tv_sec;
        return result;
    }
#else
    #define GETTIME(ts, err) do { } while (0)
#endif

// Write the contents of EEPROM staging to the actual EEPROM.
// Returns error codes from OD_EEPROM_ERROR, 0 == OK.
// Caller waits several seconds.
unsigned odt::eeWrite() {
    #if defined DEBUG_EEPROM
        struct timespec ts1, ts2, ts3, ts4, ts5, ts6, ts7;
    #endif
    if (put(OD_EEPROM_ERROR, 0)) return 100;
    struct timespec tenthSecond = {
        0,   // seconds
        100 * 1000 * 1000  // nanoseconds
    };
    // Stage 1
    GETTIME(ts1, 101);
    if (put(OD_EEPROM_CTRL, 0xaa05)) return 102;
    GETTIME(ts2, 103);
    unsigned count = 0;
    while (1) {
        if (0 == get(OD_EEPROM_CTRL)) break;
        if (count > 20) return 104;
        nanosleep(&tenthSecond, NULL);
        count++;
    }

    // Stage 2
    GETTIME(ts3, 105);
    if (put(OD_EEPROM_CTRL, 0x505a)) return 106;
    GETTIME(ts4, 107);
    count = 0;
    while (1) {
        if (0 == get(OD_EEPROM_CTRL)) break;
        if (count > 20) return 108;
        nanosleep(&tenthSecond, NULL);
        count++;
    }

    // Stage 3
    GETTIME(ts5, 109);
    if (put(OD_EEPROM_CTRL, 0xc5a0)) return 110;
    GETTIME(ts6, 111);
    struct timespec oneSecond = { 1, 0 };
    nanosleep(&oneSecond, NULL);
    count = 0;
    while (1) {
        if (0 == get(OD_EEPROM_CTRL)) break;
        if (count > 20) return 112;
        nanosleep(&tenthSecond, NULL);
        count++;
    }
    GETTIME(ts7, 113);

    // Wrap it up
    unsigned eeprom_err = get(OD_EEPROM_ERROR);
    if (0 == eeprom_err) {
        // get dictionary to repopulate from eeprom
        nanosleep(&tenthSecond, NULL);
        put(OD_EEPROM_CTRL, 0x1234);
        nanosleep(&oneSecond, NULL);
        nanosleep(&oneSecond, NULL);
    }
    #if defined DEBUG_EEPROM
        printf("Write first command: %f\n", delta(ts2, ts1));
        printf("Response: %f\n", delta(ts3, ts2));
        printf("Write second command: %f\n", delta(ts4, ts3));
        printf("Response: %f\n", delta(ts5, ts4));
        printf("Write third command: %f\n", delta(ts6, ts5));
        printf("Response: %f\n", delta(ts7, ts6));
    #endif
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
        errPrint("setUpTelemetry(): Failed to stop telemetry.\n");
        return true;
    }
    uint32_t max = get(OD_TELEMETRY_MAX);
    if (max == 0xffff) {
        errPrint("setUpTelemetry(): Failed to get TELEMETRY_MAX.\n");
        return true;
    }
    if (n > max) {
        errPrint("setUpTelemetry(): Too many telemetry entries.\n");
        return true;
    }
    boostGuard guard(gTelemetryMutex);
    // TODO implement
    return true;  // as long as it's incomplete
}

bool odt::stopTelemetry() {
    // Doesn't need to grab gTelemetryMutex unless the client code is
    // ridiculous.
    return put(OD_TELEMETRY_INTERVAL, 0);
}

void odt::getStatistics(Statistics &stat) {
    stat = gStat;
}

