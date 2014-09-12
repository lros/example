#include "boostly.hpp"
#include <iostream>

boostly::Mutex gMutex;
boostly::Condition gCond;
bool gGotData;
unsigned gValue;

void threadfn() {
    timespec oneSecond = { 1, 0 };
    for (unsigned i = 0; i < 7; i++) {
        std::cerr << "hello from threadfn(), #" << i << std::endl;
        {
            boostly::Guard g(gMutex);
            gGotData = true;
            gValue = i;
            gCond.notify();
        }
        int err = nanosleep(&oneSecond, NULL);
        if (err) throw "threadfn nanosleep";
    }
}

int main() {
    std::cerr << "******* Using pthreads" << std::endl;
    gGotData = false;
    gValue = 999;
    boostly::Thread t(threadfn);
    bool timedOut = false;
    while (!timedOut) {
        boostly::Time deadline;
        deadline.setToNow();
        deadline.addSeconds(2);
        boostly::Guard g(gMutex);
        unsigned nWakeups = 0;
        while(!gGotData && !timedOut) {
            timedOut = gCond.timed_wait(g, deadline);
            nWakeups++;
            if (timedOut) std::cerr << "timed out" << std::endl;
        }
        std::cerr << "main() exited inner loop with " << nWakeups
                << " wakeups; gGotData is "
                << (gGotData ? "true" : "false") << std::endl;
        if (gGotData) {
            std::cerr << "main(), notified, gValue is " << gValue << std::endl;
            gGotData = false;
        }
    }
    t.join();
}
