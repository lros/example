#include <errno.h>
#include <stdio.h>
#include "boostly.hpp"

namespace boostly {

// class function
void *Thread::helper(void *pdata) {
    Thread *that = (Thread *) pdata;
    (*that->pfn)();
    return NULL;
}

void Thread::start(threadfn_t *pthreadfn) {
    pfn = pthreadfn;
    int err = pthread_create(&pt, NULL, helper, (void *) this);
    if (err) throw "pthread_create error";
}

Thread::Thread(threadfn_t *pthreadfn) {
    start(pthreadfn);
}

void Thread::join() {
    int err = pthread_join(pt, NULL);
    if (err) throw "pthread_join error";
}

Guard::Guard(Mutex &m)
: m(m)
{
    int err = pthread_mutex_lock(&m.pm);
    if (err) throw "pthread_mutex_lock error";
}

Guard::~Guard() {
    int err = pthread_mutex_unlock(&m.pm);
    if (err) throw "pthread_mutex_unlock error";
}

void Time::setToNow() {
    int err = clock_gettime(CLOCK_REALTIME, &ts);
    if (err) throw "clock_gettime error";
}

void Time::addSeconds(unsigned sec) {
    ts.tv_sec += sec;
}

Condition::Condition() {
    int err = pthread_cond_init(&pc, NULL);
    if (err) throw "pthread_cond_init error";
}

void Condition::notify() {
    int err = pthread_cond_signal(&pc);
    if (err) throw "pthread_cond_signal error";
}

bool Condition::timed_wait(Guard &g, Time &deadline) {
    int err = pthread_cond_timedwait(&pc, &g.m.pm, &deadline.ts);
    if (err == ETIMEDOUT) return true;
    if (err) throw "pthread_cond_timedwait error";
    return false;
}

}  // end namespace boostly

