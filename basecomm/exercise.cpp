#include "odTransport.hpp"
#include "dataLink.hpp"
#include <stdio.h>

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

int main() {
    dl::init();
    odt::init();
    dl::start();
    odt::setOdVersion();

    unsigned badValues = 0;
    for (unsigned i = 0; i < odt::NUM_OD_ENTRIES; i++) {
        uint32_t value = odt::get((odt::OdName) i);
        if (value == 0xffff) {
            badValues++;
        }
    }
    printf("%u OD entries fetched\n", odt::NUM_OD_ENTRIES);
    printf("%u I/O errors\n", badValues);

    dl::finish();
    printStats();
    return 0;
}

