#include "protocol.hpp"
#include <stdio.h>
#include <string.h>

int main() {
    const char *err = bc::init();
    if (err) {
        printf("init() error: %s\n", err);
        return 1;
    }
    bc::buffer message;
#if defined RDA_TARGET_linux
    uint8_t content[] = {
        'h', 'e', 'l', 'l', 'o',
        0xdb, 0xdc, 0xdd, 0xc0,
        'w', 'o', 'r', 'l', 'd', '!'
    };
#elif defined RDA_TARGET_imx27
    uint8_t content[] = {
        0, 0x80,  // get OD_VERSION (index 0, 4-bytes)
        7, 0x80,  // get OD_TIMER (index 7, 4 bytes)
        26, 0xc0,  // get OD_TELEMETRY_MAX (index 26, 2 bytes)
    };
#endif
    for (unsigned i = 0; i < sizeof(content); i++) {
        // should be bcopy() or something
        message.content()[i] = content[i];
    }
    message.setContentLength(sizeof(content));
    err = bc::send(message, 2, false);
    if (err) {
        printf("send() error: %s\n", err);
        return 1;
    }
#if 0
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
#endif
    return 0;
}

