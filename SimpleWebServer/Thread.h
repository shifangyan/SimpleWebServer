/***********************/
//此类实现线程的封装
/**********************/

#ifndef THREAD__
#define THREAD__

#include <pthread.h>
#include <queue>
#include <memory>
#include <functional>
#include "NonCopy.h"
#include "EventHandler.h"
#include "EventLoop.h"

struct ThreadData
{
    typedef std::function<void()> func;
    func thread_function_; 
    ThreadData(func thread_function):thread_function_(thread_function) {}
};
class Thread:public NonCopy
{
private:
    //bool is_exit_;
    //bool is_detach_;
    //bool is_start_;
    pthread_t thread_id_;
    pid_t pid_;
    //Mutex mutex_;
   // EventLoop event_loop_;
    ThreadData thread_data_;
    static void *ThreadFunction(void *p);
public:
    Thread(const std::shared_ptr<EventLoop> &event_loop);
    pthread_t GetThreadID() const
    {
        return thread_id_;
    }
    //void Start();
    //inline int NewHandlerNum();
    //std::shared_ptr<EventHandler> GetNewHandler();
};
#endif