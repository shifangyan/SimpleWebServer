/***************** */
//主要负责线程的创建，销毁，任务分发
/**************** */

#ifndef THREADPOOL__
#define THREADPOOL__
#include <vector>
#include <memory>
#include "Thread.h"
#include "EventLoop.h"

class ThreadPool:public NonCopy
{
private:
    std::vector<std::shared_ptr<Thread>> thread_ptr_vec_;
    std::vector<std::shared_ptr<EventLoop>> event_loop_ptr_vec_;
    int next_loop_;
    typedef std::weak_ptr<EventLoop> wptr;
public:
    ThreadPool(int thread_num = 10);
    ~ThreadPool();
    template<typename T>
    void AddNewConnect(int fd)
    {
        std::shared_ptr<EventHandler> event_ptr(new T(fd,wptr(event_loop_ptr_vec_[next_loop_])));
        event_loop_ptr_vec_[next_loop_++]->AddNewHandler(event_ptr);
        //event_ptr->EnableRead();
        next_loop_ = (next_loop_>=event_loop_ptr_vec_.size()?0:next_loop_);
    }
};
#endif