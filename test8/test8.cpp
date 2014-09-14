#include "odTransport.hpp"
#include <stdio.h>
#include <string.h>
#include <time.h>

int main() {
    dl::init();
    dl::start();
    dl::Buffer message;
    printf("Sizes: int = %zu, long int = %zu, long long int = %zu\n",
        sizeof (int), sizeof(long int), sizeof(long long int));
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
    uint8_t seq = dl::send(message, 2, false);
    printf("seq is %hhu\n", seq);
    timespec aWhile = {
        2,  // seconds
        0    // nanoseconds
    };
    nanosleep(&aWhile, NULL);
    dl::finish();
    dl::Statistics stat;
    dl::statistics(stat);
    printf("%llu bytes sent.\n", stat.sentBytes);
    printf("%llu packets sent.\n", stat.sentPackets);
    printf("%llu bytes received.\n", stat.recvBytes);
    printf("%llu good packets received.\n", stat.recvPackets);
    printf("%llu bad packets received.\n", stat.badPackets);
    printf("%llu good packets dropped.\n", stat.dropped);
    return 0;
}

