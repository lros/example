#include "odTransport.hpp"
#include "dataLink.hpp"
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <stdlib.h>

void usageExit(const char *msg) {
    if (msg) printf("%s\n", msg);
    printf("Usage: motorType [TYPE]\n"
        "  where TYPE is 1 or 2\n"
        "If TYPE is not present, prints the current motor type setting.\n");
    exit(1);
}

#if defined RDA_CONFIG_debug
    void printStats() {
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
    }
#else
    #define printStats() do { } while (0)
#endif

void fail(const char *msg) {
    fprintf(stderr, "%s\n", msg);
    dl::finish();
    printStats();
    exit(1);
}

// Convert a 32.16 fixed point to float
float fixpoint32_16(uint32_t value) {
    return value / (float) 0x10000;
}

int main(int argc, char **argv) {
    unsigned newMotorType = 0;
    // Parse command line args
    if (argc > 2) usageExit("Too many arguments.");
    if (argc == 2) {
        if (0 == strcmp(argv[1], "1")) newMotorType = 1;
        else if (0 == strcmp(argv[1], "2")) newMotorType = 2;
        else usageExit("TYPE must be 1 or 2.");
    }

    dl::init();
    odt::init();
    dl::start();
    odt::setOdVersion();

    if (odt::put(odt::OD_EEPROM_ERROR, 0))
        fail("Failed to reset EEPROM_ERROR.");
    odt::OdName names[] = {
        odt::OD_EEPROM_ERROR,
        odt::OD_VGO_INVENTORY,
        odt::OD_EEPROM_30,
    };
    uint32_t values[3];
    if (odt::getMultiple(names, values, 3))
        fail("Failed to get current status.");
    #if defined RDA_CONFIG_debug
        printf("EEPROM error code is %u\n", values[0]);
        printf("Motor type in EEPROM staging is %u\n", values[2]);
    #endif
    printf("Motor type is %u\n", (values[1] >> 16) & 0xff);

    if (newMotorType > 0) {
        printf("Setting motor type to %u...\n", newMotorType);
        if (odt::stopTelemetry())
            fail("Failed to turn off telemetry.\n");
        if (odt::put(odt::OD_EEPROM_30, newMotorType))
            fail("Failed to set new motor type in EEPROM staging area.");
        unsigned err = odt::eeWrite();
        if (err) {
            #if defined RDA_CONFIG_debug
                printf("eeWrite() returned %u\n", err);
            #endif
            fail("Failed to rewrite EEPROM.");
        }
        #if defined RDA_CONFIG_debug
            printf("OD_EEPROM_ERROR is 0x%04x\n",
                    odt::get(odt::OD_EEPROM_ERROR));
        #endif
        if (odt::getMultiple(names, values, 3))
            fail("Failed to get current status.");
        #if defined RDA_CONFIG_debug
            printf("EEPROM error code is %u\n", values[0]);
            printf("Motor type in EEPROM staging is %u\n", values[2]);
        #endif
        printf("Motor type is %u\n", (values[1] >> 16) & 0xff);
    }
    //nanosleep(&aWhile, NULL);
    dl::finish();
    printStats();
    return 0;
}

