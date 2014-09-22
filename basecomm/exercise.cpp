#include "odTransport.hpp"
#include "dataLink.hpp"
#include <stdio.h>
#include <stdlib.h>
#include <climits>

static void usageExit() {
    fprintf(stderr, "Usage: exercise [COUNT]\n"
            "COUNT is the number of MPIC gets to transact before exiting.\n"
            "COUNT defaults to 100\n"
            );
    exit(1);
}

void printStats() {
    dl::Statistics dlStat;
    dl::getStatistics(dlStat);
    fprintf(stderr, "%llu bytes sent.\n", dlStat.sentBytes);
    fprintf(stderr, "%llu packets sent.\n", dlStat.sentPackets);
    fprintf(stderr, "%llu bytes received.\n", dlStat.recvBytes);
    fprintf(stderr, "%llu good packets received.\n", dlStat.recvPackets);
    fprintf(stderr, "%llu bad packets received.\n", dlStat.badPackets);
    fprintf(stderr, "%llu good packets dropped.\n", dlStat.dropped);
    odt::Statistics odtStat;
    odt::getStatistics(odtStat);
    fprintf(stderr, "%llu unexpected sequence numbers.\n", odtStat.badSequence);
    fprintf(stderr, "%llu unexpected packet lengths.\n", odtStat.badLength);
    fprintf(stderr, "%llu response timeouts.\n", odtStat.timeout);
}

int main(int argc, char **argv) {
    unsigned long count = 100;
    if (argc > 2) usageExit();
    if (argc == 2) {
        count = strtoul(argv[1], NULL, 0);
        if (count == 0 || count == ULONG_MAX) usageExit();
    }

    dl::init();
    odt::init();
    dl::start();
    odt::setOdVersion();

    unsigned badValues = 0;
    unsigned index = 0;
    for (unsigned i = 0; i < count; i++) {
        uint32_t value = odt::get((odt::OdName) i);
        if (value == 0xffff) {
            badValues++;
        }
        index++;
        if (index >= odt::NUM_OD_ENTRIES) index = 0;
    }
    fprintf(stderr, "%lu OD entry fetches attempted\n", count);
    fprintf(stderr, "%u I/O errors\n", badValues);

    dl::finish();
    printStats();
    return 0;
}

