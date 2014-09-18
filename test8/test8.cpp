#include "odTransport.hpp"
#include "dataLink.hpp"
#include <stdio.h>
#include <string.h>
#include <time.h>

float fixpoint32_16(uint32_t value) {
    return ((float) value) / 0x10000;
}

int main() {
    dl::init();
    odt::init();
    dl::start();
    dl::Buffer message;
    printf("Sizes: int = %zu, long int = %zu, long long int = %zu\n",
        sizeof (int), sizeof(long int), sizeof(long long int));
    uint8_t content[] = {
        0, 0x80,  // get OD_VERSION (index 0, 4-bytes)
        7, 0x80,  // get OD_TIMER (index 7, 4 bytes)
        26, 0xc0,  // get OD_TELEMETRY_MAX (index 26, 2 bytes)
    };
    memcpy(message.content(), content, sizeof(content));
    message.contentLength(sizeof(content));
    uint8_t seq = dl::send(message, 2, false);
    printf("seq is %hhu\n", seq);
    timespec aWhile = {
        2,  // seconds
        0    // nanoseconds
    };
    nanosleep(&aWhile, NULL);
    uint32_t values[10];
    odt::OdName names[] = {
        odt::OD_VERSION,
        odt::OD_TIMER,
        odt::OD_TELEMETRY_MAX,
    };
    if (odt::getMultiple(names, values, 3)) {
        printf("Error return from getMultiple()\n");
    } else {
        printf("OD_VERSION is %u\n", values[0]);
        printf("OD_TIMER is %u\n", values[1]);
        printf("OD_TELEMETRY_MAX is %u\n", values[2]);
    }
    odt::OdName caps[] = {
        odt::OD_CAPS_WHEEL_BASE,
        odt::OD_CAPS_WHEEL_RADIUS,
        odt::OD_CAPS_ENCODER_COUNTS,
        odt::OD_CAPS_GEAR_RATIO,
        odt::OD_CAPS_PI_SAMPLING_INTERVAL,
        odt::OD_CAPS_TELEMETRY_COUNTER_INTERVAL,
    };
    if (odt::getMultiple(caps, values, 6)) {
        printf("Error return from getMultiple()\n");
    } else {
        printf("OD_CAPS_WHEEL_BASE is %f\n", fixpoint32_16(values[0]));
        printf("OD_CAPS_WHEEL_RADIUS is %f\n", fixpoint32_16(values[1]));
        printf("OD_CAPS_ENCODER_COUNTS is %u\n", values[2]);
        printf("OD_CAPS_GEAR_RATIO is %f\n", fixpoint32_16(values[3]));
        printf("OD_CAPS_PI_SAMPLING_INTERVAL is %u\n", values[4]);
        printf("OD_CAPS_TELEMETRY_COUNTER_INTERVAL is %u\n", values[5]);
    }
    // Try writing the EEPROM although we haven't changed anything.
    if (odt::stopTelemetry()) {
        printf("Failure to turn off telemetry.\n");
    } else {
        unsigned err = odt::eeWrite();
        printf("eeWrite() returns %u\n", err);
        printf("OD_EEPROM_ERROR is 0x%04x\n", odt::get(odt::OD_EEPROM_ERROR));
    }
    nanosleep(&aWhile, NULL);
    dl::finish();
    dl::Statistics dlStat;
    dl::getStatistics(dlStat);
    printf("%llu bytes sent.\n", dlStat.sentBytes);
    printf("%llu packets sent.\n", dlStat.sentPackets);
    printf("%llu bytes received.\n", dlStat.recvBytes);
    printf("%llu good packets received.\n", dlStat.recvPackets);
    printf("%llu bad packets received.\n", dlStat.badPackets);
    printf("%llu good packets dropped.\n", dlStat.dropped);
    odt::Statistics odtStat;
    odt::getStatistics(odtStat);
    printf("%llu unexpected sequence numbers.\n", odtStat.badSequence);
    printf("%llu unexpected packet lengths.\n", odtStat.badLength);
    printf("%llu response timeouts.\n", odtStat.timeout);
    return 0;
}

