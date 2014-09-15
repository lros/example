// TODO include specific subsets
#include <boost/thread.hpp>
#include <boost/chrono.hpp>
//#include <boost/chrono_io.hpp>
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
            gCond.notify_one();
        }
        boost::this_thread::sleep_for(boost::chrono::seconds(1));
    }
}

int main() {
    std::cerr << "******* Using wait_until()" << std::endl;
    gGotData = false;
    gValue = 999;
    boost::thread t(threadfn);
    while (true) {
        boost::chrono::steady_clock::time_point now
                = boost::chrono::steady_clock::now();
        std::cerr << "now is " << now << std::endl;
        boost::chrono::steady_clock::time_point deadline
            = now + boost::chrono::seconds(2);
        std::cerr << "deadline is " << deadline << std::endl;
        boost::unique_lock<boost::mutex> lock(gMutex);
        unsigned nWakeups = 0;
        while(!gGotData) {
            bool ret = gCond.wait_until(lock, deadline);
            nWakeups++;
            if (ret) std::cerr << "wait_until() returned true on wakeup #"
                    << nWakeups << std::endl;
            now = boost::chrono::steady_clock::now();
            if (now > deadline) break;
        }
        std::cerr << "main() exited inner loop with " << nWakeups
                << " wakeups; gGotData is "
                << (gGotData ? "true" : "false") << std::endl;
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
