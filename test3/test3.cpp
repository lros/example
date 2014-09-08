#include "protocol.hpp"
#include <stdio.h>

int main() {
    const char *err = bc::init();
    if (err) {
        printf("init() error: %s\n", err);
        return 1;
    }
    uint8_t message[bc::BUFFER_SIZE] = {
        1, 2, 3, 4, 5, 6, 7,
        'h', 'e', 'l', 'l', 'o',
        0xdb, 0xdc, 0xdd, 0xc0,
        'w', 'o', 'r', 'l', 'd', '!'
    };
    err = bc::send(message, 15, 2, true);
    if (err) {
        printf("send() error: %s\n", err);
        return 1;
    }
    err = bc::finish();
    if (err) {
        printf("finish() error: %s\n", err);
        return 1;
    }
    return 0;
}

