#include "odTransport.hpp"
#include "dataLink.hpp"
#include <stdio.h>
#include <string.h>
#include <time.h>

float fixpoint32_16(uint32_t value) {
    return ((float) value) / 0x10000;
}

int main(int argc, char **argv) {
    printf("argc is %d\n", argc);
    dl::init();
    odt::init();
    dl::start();
    if (odt::put(odt::OD_EEPROM_ERROR, 0)) {
        printf("Failed to reset EEPROM_ERROR.\n");
        return 1;
    }
    odt::OdName names[] = {
        odt::OD_EEPROM_ERROR,
        odt::OD_VGO_INVENTORY,
        odt::OD_EEPROM_30,
    };
    uint32_t values[3];
    if (odt::getMultiple(names, values, 3)) {
        printf("Failed to get current status.\n");
        return 1;
    }
    printf("EEPROM error code is %u\n", values[0]);
    printf("Current motor type is %u\n", (values[1] >> 16) & 0xff);
    printf("Current motor type in EEPROM staging is %u\n", values[2]);

    // Try writing the EEPROM although we haven't changed anything.
    if (odt::stopTelemetry()) {
        printf("Failure to turn off telemetry.\n");
    } else {
        unsigned err = odt::eeWrite();
        printf("eeWrite() returns %u\n", err);
        printf("OD_EEPROM_ERROR is 0x%04x\n", odt::get(odt::OD_EEPROM_ERROR));
    }
    //nanosleep(&aWhile, NULL);
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

