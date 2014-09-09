#include <boost/thread.hpp>
#include <boost/date_time.hpp>
#include <stdio.h>

boost::condition_variable ping;
boost::mutex mx;
bool got_data;
unsigned value;

void threadfn() {
    for (unsigned i = 0; i < 10; i++) {
        printf("hello from threadfn() #%d\n", i);
        {
            boost::lock_guard<boost::mutex> guard(mx);
            got_data = true;
            value = i;
        }
        ping.notify_one();
        boost::this_thread::sleep(boost::posix_time::seconds(1));
    }
}

int main() {
    got_data = false;
    value = 999;
    printf("main(), before launching.\n");
    boost::thread t(threadfn);
    printf("main(), after launching.\n");
    bool timedOut = false;
    while (!timedOut) {
        boost::posix_time::ptime deadline(
                boost::posix_time::microsec_clock::local_time()
                    + boost::posix_time::seconds(2));
        boost::unique_lock<boost::mutex> guard(mx);
        while(!got_data && !timedOut) {
            timedOut = ! ping.timed_wait(guard, deadline);
        }
        if (!timedOut) {
            printf("main(), pinged, value is %d\n", value);
        } else {
            printf("main(), timed out\n");
        }
    }
    t.join();
    printf("main(), after joining.\n");
}
