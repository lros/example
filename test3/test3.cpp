#include "protocol.hpp"
#include <stdio.h>
#include <string.h>

int main() {
    const char *err = bc::init();
    if (err) {
        printf("init() error: %s\n", err);
        return 1;
    }
#if defined RDA_TARGET_linux
    uint8_t message[bc::BUFFER_SIZE] = {
        1, 2, 3, 4, 5, 6, 7,
        'h', 'e', 'l', 'l', 'o',
        0xdb, 0xdc, 0xdd, 0xc0,
        'w', 'o', 'r', 'l', 'd', '!'
    };
    const int contentLength = 15;
#elif defined RDA_TARGET_imx27
    uint8_t message[bc::BUFFER_SIZE] = {
        1, 2, 3, 4, 5, 6, 7,
        0, 0x80,  // get OD_VERSION (index 0, 4-bytes)
        7, 0x80,  // get OD_TIMER (index 7, 4 bytes)
        26, 0xc0,  // get OD_TELEMETRY_MAX (index 26, 2 bytes)
    };
    const int contentLength = 6;
#endif
    err = bc::send(message, contentLength, 2, false);
    if (err) {
        printf("send() error: %s\n", err);
        return 1;
    }
    uint8_t recvBuffer[bc::BUFFER_SIZE];
    memset(recvBuffer, 0, sizeof(recvBuffer));
    unsigned recvLength = sizeof(recvBuffer);
    err = bc::receive(recvBuffer, recvLength);
    if (err) {
        printf("receive() error: %s\n", err);
        return 1;
    }
    printf("received %d bytes\n", recvLength);
    err = bc::finish();
    if (err) {
        printf("finish() error: %s\n", err);
        return 1;
    }
    return 0;
}

