#include "protocol.hpp"
#include <stdio.h>

int main() {
    const char *err = bc::init();
    if (err) {
        printf("init() error: %s\n", err);
        return 1;
    }
    err = bc::finish();
    if (err) {
        printf("finish() error: %s\n", err);
        return 1;
    }
    return 0;
}

