/***************** */
//主要负责线程的创建，销毁，任务分发
/**************** */

#ifndef THREADPOOL__
#define THREADPOOL__
#include <vector>
#include <memory>
#include "Thread.h"
#include "EventLoop.h"
#include "Channel.h"

class ThreadPool:public NonCopy
{
public:
	typedef std::shared_ptr<ThreadPool> ThreadPool_SPtr;
	typedef std::weak_ptr<ThreadPool> ThreadPool_WPtr;
	typedef EventLoop::EventLoop_WPtr EventLoop_WPtr;
	typedef Channel::Channel_WPtr Channel_WPtr;
private:
    std::vector<std::unique_ptr<Thread>> thread_uptr_vec_;
    std::vector<std::shared_ptr<EventLoop>> event_loop_sptr_vec_;
    int next_loop_;
   
public:
    ThreadPool(int thread_num = 10);
    ~ThreadPool();
	void AddNewConnect(Channel_WPtr channel_wptr);
 
};
#endif