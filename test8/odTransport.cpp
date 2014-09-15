#include "odTransport.hpp"
#include "boostly.hpp"

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
// after the receiveCallback returns.
static dl::Buffer ourBuffer;

// Use this mutex for accessing gotResponse and pResponseBuffer
boostly::Mutex gResponseMutex;
bool gGotResponse;
static dl::Buffer *gpResponseBuffer;

static dl::Buffer *receiveCallback(dl::Buffer *message);

void odt::init() {
    gpResponseBuffer = &ourBuffer;
    gGotResponse = false;
    dl::registerCallback(receiveCallback, dl::COMMAND_CHANNEL);
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
    boostly::Guard guard(gTransportMutex);
    uint8_t seq = dl::send(request, dl::COMMAND_CHANNEL, true);
    // TODO wait for ACK
}

void odt::getMultiple(OdName *names, uint32_t *valuesBack, unsigned n) {
    dl::Buffer request;
    request.contentLength(0);
    for (unsigned i = 0; i < n; i++) {
        request.add16(odGetCommand(names[i]));
    }
    boostly::Guard guard(gTransportMutex);
    uint8_t seq = dl::send(request, dl::COMMAND_CHANNEL, false);
    // TODO wait for response and parse it
}

static dl::Buffer *receiveCallback(dl::Buffer *message) {
    // TODO implement
    gGotResponse = true;
    dl::Buffer *tmp = gpResponseBuffer;
    gpResponseBuffer = message;
    return tmp;
}

uint16_t odt::odGetCommand(OdName name) {
    uint16_t cmd = name;
    if (odSize[name] == 2)
        cmd |= 0xc000;
    else
        cmd |= 0x4000;
    return cmd;
}

