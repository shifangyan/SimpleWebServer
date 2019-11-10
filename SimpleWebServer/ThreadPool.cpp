#include "ThreadPool.h"
#include <functional>
ThreadPool::ThreadPool(int thread_num):next_loop_(0)
{
    for(int i = 0;i < thread_num;i++)
    {
        std::shared_ptr<EventLoop> event_ptr(new EventLoop);
        event_ptr->Init();
        //std::shared_ptr<Thread> thread_ptr(new Thread(event_ptr));
		std::unique_ptr<Thread> thread_ptr(new Thread(std::bind(&EventLoop::Loop,event_ptr.get())));
        thread_uptr_vec_.push_back(std::move(thread_ptr));
		event_loop_sptr_vec_.push_back(std::move(event_ptr));
    }
}

ThreadPool::~ThreadPool()
{
    printf("ThreadPool destruct\n");
    for(int i = 0;i <event_loop_sptr_vec_.size();i++)
    {
		event_loop_sptr_vec_[i]->Quit();
		thread_uptr_vec_[i]->join();
        //pthread_join(thread_ptr_vec_[i]->GetThreadID(),NULL);
    }
}

void ThreadPool::AddNewConnect(Channel_WPtr channel_wptr)
{
	auto channel_sptr = channel_wptr.lock();
	if (channel_sptr)
	{
		channel_sptr->SetEventLoop(event_loop_sptr_vec_[next_loop_]);
		channel_sptr->EnableRead();
		event_loop_sptr_vec_[next_loop_++]->AddNewChannel(channel_wptr);
		//event_ptr->EnableRead();
		next_loop_ = (next_loop_ >= event_loop_sptr_vec_.size() ? 0 : next_loop_);
	}
}
