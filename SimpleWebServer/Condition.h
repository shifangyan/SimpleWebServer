
/***********************/
//此类封装线程的条件变量
/***********************/

#ifndef CONDITION__
#define CONDITION__

#include <pthread.h>
#include "Mutex.h"
#include "NonCopy.h"

class Condition:public NonCopy
{
private:
    Mutex &mutex_;
    pthread_cond_t p_condition_;
public:
    Condition(Mutex &mutex);
    ~Condition();
    void Wait();
    //void TimeWait()
    void Signal();
    void SignalAll();
};

class CountDownLatch:public NonCopy
{
private:
    mutable Mutex mutex_;
    Condition condition_;
    int count_;
public:
    CountDownLatch(int count):mutex_(),condition_(mutex_),count_(count) {}
    void Wait()
    {
        MutexGuard lock(mutex_);
        while(count_ > 0)
            condition_.Wait();
    }
    void CountDown()
    {
        MutexGuard lock(mutex_);
        --count_;
        if(count_ == 0)
            condition_.SignalAll();
    }
};
#endif