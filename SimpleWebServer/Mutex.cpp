#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include "Mutex.h"

Mutex::Mutex()
{
    pthread_mutex_init(&p_mutex_,NULL);
}

Mutex::~Mutex()
{
    pthread_mutex_destroy(&p_mutex_);
}

void Mutex::Lock()
{
    pthread_mutex_lock(&p_mutex_);
}

void Mutex::UnLock()
{
    pthread_mutex_unlock(&p_mutex_);
}