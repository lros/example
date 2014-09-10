// TODO include specific subsets
#include <boost/thread.hpp>
#include <boost/date_time.hpp>
#include <iostream>

boost::condition_variable gCond;
boost::mutex gMutex;
bool gGotData;
unsigned gValue;

void threadfn() {
    for (unsigned i = 0; i < 7; i++) {
        std::cerr << "hello from threadfn(), #" << i << std::endl;
        {
            boost::lock_guard<boost::mutex> guard(gMutex);
            gGotData = true;
            gValue = i;
        }
        gCond.notify_one();
        boost::this_thread::sleep(boost::posix_time::seconds(1));
    }
}

// Pick one:
//#define WAIT
#define TIMED_WAIT
//#define TIMED_WAIT_PRED

#if defined TIMED_WAIT_PRED
unsigned gPredCount;

bool gotDataPred() {
    gPredCount++;
    return gGotData;
}
#endif

int main() {
#if defined WAIT
    std::cerr << "******* Using wait(), no timeout" << std::endl;
#elif defined TIMED_WAIT
    std::cerr << "******* Using timed_wait()" << std::endl;
#elif defined TIMED_WAIT_PRED
    std::cerr << "******* Using timed_wait(), with a predicate" << std::endl;
#else
#error Must define one of WAIT, TIMED_WAIT, TIMED_WAIT_PRED
#endif
    gGotData = false;
    gValue = 999;
    boost::thread t(threadfn);
    while (true) {
#if defined TIMED_WAIT || defined TIMED_WAIT_PRED
        boost::system_time now(
                boost::posix_time::microsec_clock::local_time());
        std::cerr << "now is " << now << std::endl;
        boost::system_time deadline(now + boost::posix_time::seconds(2));
        std::cerr << "deadline is " << deadline << std::endl;
#endif
        boost::unique_lock<boost::mutex> lock(gMutex);
        unsigned nWakeups = 0;
#if defined TIMED_WAIT_PRED
        gPredCount = 0;
#endif
        while(!gGotData) {
            //timedOut = ! gCond.timed_wait(lock, deadline);
            //std::cerr << "main(), gCond.wait() returned, gGotData is "
                    //<< gGotData << std::endl;
#if defined WAIT
            gCond.wait(lock);
            nWakeups++;
#elif defined TIMED_WAIT
            bool ret = gCond.timed_wait(lock, deadline);
            nWakeups++;
            if (ret) std::cerr << "timed_wait() returned true on wakeup #"
                    << nWakeups << std::endl;
            now = boost::posix_time::microsec_clock::local_time();
            if (now > deadline) break;
#elif defined TIMED_WAIT_PRED
            //gPredCount = 0;
            bool ret = gCond.timed_wait(lock, deadline, gotDataPred);
            //std::cerr << "timed_wait() called predicate " << gPredCount
                    //<< " times" << std::endl;
            nWakeups++;
            if (ret) std::cerr << "timed_wait() (predicate) returned true"
                    << " on wakeup #" << nWakeups << std::endl;
#endif
        }
        std::cerr << "main() exited inner loop with " << nWakeups
                << " wakeups; gGotData is "
                << (gGotData ? "true" : "false") << std::endl;
#if defined TIMED_WAIT_PRED
        std::cerr << "timed_wait() called predicate " << gPredCount
                << " times" << std::endl;
#endif
        if (gGotData) {
            std::cerr << "main(), notified, gValue is " << gValue << std::endl;
            gGotData = false;
        }
        if (now > deadline) break;
    }
    // not reached in this incarnation of the code
    t.join();
    std::cerr << "main(), after joining" << std::endl;
}
