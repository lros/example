#define BOOST_TEST_MODULE timed wait
#include <boost/test/unit_test.hpp>

#include <boost/thread.hpp>
#include <boost/date_time.hpp>
#include <iostream>

using namespace std;

const unsigned MAX_SPURIOUS_WAKEUPS = 3;
boost::condition_variable gCond;
boost::mutex gMutex;
bool gNotified;

void threadfn() {
    boost::this_thread::sleep(boost::posix_time::seconds(1));
    boost::lock_guard<boost::mutex> guard(gMutex);
    gNotified = true;
    gCond.notify_one();
}

BOOST_AUTO_TEST_CASE(timed_wait) {
    gNotified = false;
    boost::thread t(threadfn);
    bool notTimedOut = true;

    boost::system_time now(boost::posix_time::microsec_clock::local_time());
    boost::system_time deadline(now + boost::posix_time::seconds(2));
    unsigned nWakeups = 0;
    {
        boost::unique_lock<boost::mutex> lock(gMutex);
        while(!gNotified && notTimedOut) {
            notTimedOut = gCond.timed_wait(lock, deadline);
            nWakeups++;
        }
    }
    now = boost::posix_time::microsec_clock::local_time();
    BOOST_CHECK(now >= deadline);
    BOOST_CHECK(nWakeups <= MAX_SPURIOUS_WAKEUPS);

    t.join();
}

