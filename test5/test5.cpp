#include "protocol.hpp"
#include <stdio.h>
#include <string.h>
#include <time.h>

int main() {
    bc::init();
    bc::start();
    bc::Buffer message;
    uint8_t content[] = {
        0, 0x80,  // get OD_VERSION (index 0, 4-bytes)
        7, 0x80,  // get OD_TIMER (index 7, 4 bytes)
        26, 0xc0,  // get OD_TELEMETRY_MAX (index 26, 2 bytes)
    };
    for (unsigned i = 0; i < sizeof(content); i++) {
        // should be bcopy() or something
        message.content()[i] = content[i];
    }
    message.contentLength(sizeof(content));
    uint8_t seq = bc::send(message, 2, false);
    printf("seq is %hhu\n", seq);
    timespec aWhile = {
        2,  // seconds
        0    // nanoseconds
    };
    nanosleep(&aWhile, NULL);
    bc::finish();
    bc::Statistics stat;
    bc::statistics(stat);
    printf("%lld packets sent.\n", stat.sendPackets);
    printf("%lld good packets received.\n", stat.recvPackets);
    printf("%lld bad packets received.\n", stat.badPackets);
    return 0;
}

