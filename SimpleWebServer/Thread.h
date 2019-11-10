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
#include "EventLoop.h"



class Thread:public NonCopy
{
public:
	typedef std::function<void()> ThreadFunction;
private:
	struct ThreadData
	{
		typedef std::function<void()> func;
		ThreadFunction thread_function_;
		ThreadData(ThreadFunction thread_function) :thread_function_(thread_function) {}
	};
    //bool is_exit_;
    //bool is_detach_;
    //bool is_start_;
    pthread_t thread_id_;
    //pid_t pid_;
    //Mutex mutex_;
   // EventLoop event_loop_;
    ThreadData thread_data_;
	
    static void *ThreadRunning(void *p);
public:
    //Thread(const std::shared_ptr<EventLoop> &event_loop);
	Thread(ThreadFunction func);
    pthread_t GetThreadID() const
    {
        return thread_id_;
    }

	void join()
	{
		pthread_join(thread_id_,NULL);
	}
    //void Start();
    //inline int NewHandlerNum();
    //std::shared_ptr<EventHandler> GetNewHandler();
};
#endif