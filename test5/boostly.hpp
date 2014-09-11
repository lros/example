// boost-ish wrapper of pthreads

#include <pthread.h>
#include <time.h>

namespace boostly {

class Thread {
public:
    typedef void threadfn_t(void);
    Thread(threadfn_t *pthreadfn);
    void join();
private:
    static void *helper(void *);
    pthread_t pt;
    threadfn_t *pfn;
};

class Mutex {
    friend class Guard;
    friend class Condition;
private:
    pthread_mutex_t pm;
};

class Guard {
    friend class Condition;
public:
    Guard(Mutex &m);
    ~Guard();
private:
    Mutex &m;
};

class Time {
    friend class Condition;
public:
    void setToNow();
    void addSeconds(unsigned sec);
private:
    struct timespec ts;
};

class Condition {
public:
    Condition();
    void notify();
    bool timed_wait(Guard &g, Time &deadline);  // true == timeout, not spurious
private:
    pthread_cond_t pc;
};

}  // end namespace boostly

