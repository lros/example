#include "odTransport.hpp"
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
boostly::Mutex gTransportMutex;

// We have to provide one buffer so we can hold on to the responses
// after the responseCallback returns.
static dl::Buffer ourBuffer;

// Use this mutex for accessing gotResponse and pResponseBuffer
boostly::Mutex gResponseMutex;
bool gGotResponse;
static dl::Buffer *gpResponseBuffer;
// Signal from receive thread to whoever sent a packet,
// the response is come.
boostly::Condition gResponseSignal;

static dl::Buffer *responseCallback(dl::Buffer *message);
static dl::Buffer *telemetryCallback(dl::Buffer *message);

void odt::init() {
    gpResponseBuffer = &ourBuffer;
    gGotResponse = false;
    dl::registerCallback(responseCallback, dl::COMMAND_CHANNEL);
    dl::registerCallback(telemetryCallback, dl::TELEMETRY_CHANNEL);
}

void odt::putMultiple(OdName *names, uint32_t *values, unsigned n) {
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
    boostly::Guard transportGuard(gTransportMutex);
    // discard any pending response
    gGotResponse = false;
    uint8_t seq = dl::send(request, dl::COMMAND_CHANNEL, true);
    // wait for ACK
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
            // TODO actually look at gResponseBuffer
            printf("Got a response!\n");
            break;
        } else {
            // timed out
            printf("Base response timeout.\n");
        }
    }
}

void odt::getMultiple(OdName *names, uint32_t *valuesBack, unsigned n) {
    dl::Buffer request;
    request.contentLength(0);
    for (unsigned i = 0; i < n; i++) {
        request.add16(odGetCommand(names[i]));
    }
    boostly::Guard guard(gTransportMutex);
    // discard any pending response
    gGotResponse = false;
    uint8_t seq = dl::send(request, dl::COMMAND_CHANNEL, false);
    // wait for response and parse it
    bool timedOut = false;
    while (!timedOut) {
        boostly::Time deadline;
        deadline.setToNow();
        deadline.addSeconds(1);
        boostly::Guard responseGuard(gResponseMutex);
        while (!gGotResponse && !timedOut) {
            timedOut = gResponseSignal.timed_wait(responseGuard, deadline);
            //printf("in wait loop, timedOut is %s\n",
                    //timedOut ? "true" : "false");
        }
        if (gGotResponse) {
            // TODO actually look at gResponseBuffer
            printf("Got a response!\n");
            break;
        } else {
            // timed out
            printf("Base response timeout.\n");
        }
    }
}

static dl::Buffer *responseCallback(dl::Buffer *message) {
    // TODO implement
    gGotResponse = true;
    dl::Buffer *tmp = gpResponseBuffer;
    gpResponseBuffer = message;
    printf("Got a response packet.\n");
    boostly::Guard responseGuard(gResponseMutex);
    gResponseSignal.notify();
    return tmp;
}

static dl::Buffer *telemetryCallback(dl::Buffer *message) {
    // TODO implement
    printf("Got a telemetry packet.\n");
    return message;
}

uint16_t odt::odGetCommand(OdName name) {
    uint16_t cmd = name;
    if (odSize[name] == 2)
        cmd |= 0xc000;
    else
        cmd |= 0x4000;
    return cmd;
}

