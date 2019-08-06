#include <errno.h>
#include <stdio.h>
#include "Condition.h"

Condition::Condition(Mutex &mutex):mutex_(mutex)
{
    pthread_cond_init(&p_condition_,NULL);
}

Condition::~Condition()
{
    pthread_cond_destroy(&p_condition_);
}

void Condition::Wait()
{
    pthread_cond_wait(&p_condition_,&mutex_.Get());
}

void Condition::Signal()
{
    pthread_cond_signal(&p_condition_);
}

void Condition::SignalAll()
{
    pthread_cond_broadcast(&p_condition_);
}