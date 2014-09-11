#include <pthread.h>
#include <time.h>
#include <iostream>

#define ETIMEDOUT 110

#define ERR_CHECK(err, message) \
        if (err) std::cerr << message " error: " << err << std::endl

pthread_cond_t gCond = PTHREAD_COND_INITIALIZER;
pthread_mutex_t gMutex;
bool gGotData;
unsigned gValue;

void *threadfn(void *unused) {
    timespec oneSecond = { 1, 0 };
    for (unsigned i = 0; i < 7; i++) {
        std::cerr << "hello from threadfn(), #" << i << std::endl;
        int err = pthread_mutex_lock(&gMutex);
        ERR_CHECK(err, "threadfn mutex lock");
        gGotData = true;
        gValue = i;
        err = pthread_cond_signal(&gCond);
        ERR_CHECK(err, "threadfn cond_signal");
        err = pthread_mutex_unlock(&gMutex);
        ERR_CHECK(err, "threadfn mutex unlock");
        err = nanosleep(&oneSecond, NULL);
        ERR_CHECK(err, "threadfn nanosleep");
    }
    return NULL;
}

int main() {
    std::cerr << "******* Using pthreads" << std::endl;
    gGotData = false;
    gValue = 999;
    pthread_t t;
    int err = pthread_create(&t, NULL, threadfn, NULL);
    ERR_CHECK(err, "main pthread_create");
    bool timedOut = false;
    while (!timedOut) {
        timespec deadline;
        err = clock_gettime(CLOCK_REALTIME, &deadline);
        ERR_CHECK(err, "main gettime");
        deadline.tv_sec += 2;
        err = pthread_mutex_lock(&gMutex);
        ERR_CHECK(err, "main mutex lock");
        unsigned nWakeups = 0;
        while(!gGotData && !timedOut) {
            err = pthread_cond_timedwait(&gCond, &gMutex, &deadline);
            nWakeups++;
            if (err == ETIMEDOUT) {
                timedOut = true;
                err = 0;
                std::cerr << "main() timed out" << std::endl;
            }
            ERR_CHECK(err, "main timed_wait");
        }
        std::cerr << "main() exited inner loop with " << nWakeups
                << " wakeups; gGotData is "
                << (gGotData ? "true" : "false") << std::endl;
        if (gGotData) {
            std::cerr << "main(), notified, gValue is " << gValue << std::endl;
            gGotData = false;
        }
        err = pthread_mutex_unlock(&gMutex);
        ERR_CHECK(err, "main mutex unlock");
    }
    err = pthread_join(t, NULL);
    ERR_CHECK(err, "main join");
}
