#include <unistd.h>
#include "Thread.h"
//Thread::Thread(const std::shared_ptr<EventLoop> &event_loop):thread_id_(-1),thread_data_(bind(&EventLoop::Loop,event_loop))
//{
//    int ret;
//    if((ret = pthread_create(&thread_id_,NULL,ThreadFunction,&thread_data_)) != 0)
//    {
//        perror("pthread_create error:");
//        exit(-1);
//    }
//}
Thread::Thread(ThreadFunction func):thread_data_(func)
{
	int ret;
	if ((ret = pthread_create(&thread_id_, NULL, ThreadRunning, &thread_data_)) != 0)
	{
		perror("pthread_create error:");
		exit(-1);
	}
}

void *Thread::ThreadRunning(void *p)
{
    ThreadData *thread_data = static_cast<ThreadData *>(p);
    thread_data->thread_function_();
}

// int Thread::NewHandlerNum()
// {
//     MutexGuard(mutex_);
//     return new_handler_queue_.size();
// }

// std::shared_ptr<EventHandler> Thread::GetNewHandler()
// {
//     MutexGuard(mutex_);
//     auto ret = new_handler_queue_.front();
//     new_handler_queue_.pop();
//     return ret;
// }