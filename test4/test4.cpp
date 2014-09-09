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

//bool 

#define WAIT
//#define TIMED_WAIT

int main() {
    gGotData = false;
    gValue = 999;
    boost::thread t(threadfn);
    while (true) {
        boost::system_time deadline(
                boost::posix_time::microsec_clock::local_time());
        std::cerr << "now is " << deadline << std::endl;
        deadline += boost::posix_time::seconds(2);
        std::cerr << "deadline is " << deadline << std::endl;
        boost::unique_lock<boost::mutex> lock(gMutex);
        unsigned nWakeups = 0;
        while(!gGotData) {
            //timedOut = ! gCond.timed_wait(lock, deadline);
            //std::cerr << "main(), gCond.wait() returned, gGotData is "
                    //<< gGotData << std::endl;
#if defined TIMED_WAIT
            bool ret = gCond.timed_wait(lock, deadline);
            nWakeups++;
            if (ret) std::cerr << "timed_wait() returned true on wakeup #"
                    << nWakeups << std::endl;
#elif defined WAIT
            gCond.wait(lock);
            nWakeups++;
#endif
        }
        std::cerr << "main() exited inner loop with " << nWakeups
                << " wakeups; gGotData is "
                << (gGotData ? "true" : "false") << std::endl;
        if (gGotData) {
            std::cerr << "main(), notified, gValue is " << gValue << std::endl;
            gGotData = false;
        }
    }
    // not reached in this incarnation of the code
    t.join();
    std::cerr << "main(), after joining" << std::endl;
}
