#include "ThreadPool.h"
#include "EchoServerEventHandler.h"
ThreadPool::ThreadPool(int thread_num):next_loop_(0)
{
    for(int i = 0;i < thread_num;i++)
    {
        std::shared_ptr<EventLoop> event_ptr(new EventLoop);
        event_ptr->Init();
        event_loop_ptr_vec_.push_back(event_ptr);
        std::shared_ptr<Thread> thread_ptr(new Thread(event_ptr));
        thread_ptr_vec_.push_back(thread_ptr);
    }
}

ThreadPool::~ThreadPool()
{
    printf("ThreadPool destruct\n");
    for(int i = 0;i <event_loop_ptr_vec_.size();i++)
    {
        event_loop_ptr_vec_[i]->Quit();
        pthread_join(thread_ptr_vec_[i]->GetThreadID(),NULL);
    }
}

// template<typename T>
// void ThreadPool::AddNewConnect(int fd)
// {
//     std::shared_ptr<EventHandler> event_ptr(new T(fd,&(event_loop_ptr_vec_[next_loop_]->GetEpoll())));
//     event_ptr->SetEvents(EPOLLIN|EPOLLET);
//     event_loop_ptr_vec_[next_loop_++]->AddNewHandler(event_ptr);
//     next_loop_ = (next_loop_>=event_loop_ptr_vec_.size()?0:next_loop_);
// }