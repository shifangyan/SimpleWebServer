#ifndef MUTEX__
#define MUTEX__
/*************************************/
//此类用于实现线程互斥量
/*************************************/
#include <pthread.h>
#include "NonCopy.h"

class Mutex:public NonCopy
{
private:
    pthread_mutex_t p_mutex_;
public:
    Mutex();
    ~Mutex();
    void  Lock();
    //int TryLock()
    void UnLock();
    pthread_mutex_t& Get() { return p_mutex_;}
};

class MutexGuard:public NonCopy
{
private:
    Mutex& mutex_;
public:
    explicit MutexGuard(Mutex &mutex):mutex_(mutex) 
    {
        mutex_.Lock();
    }
    ~MutexGuard() 
    {
        mutex_.UnLock();
    }
};

#endif
