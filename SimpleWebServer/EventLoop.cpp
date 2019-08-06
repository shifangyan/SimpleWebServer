#include <functional>
#include "EventLoop.h"
#include "Epoll.h"

IgnoreSigPipe initObj;

EventLoop::EventLoop():epoll_(),is_quit_(false),mutex_(),new_handler_queue_(),
notify_handler_ptr_(),timer_manager_(new TimerManager)
{
}

void EventLoop::Init()
{
	notify_handler_ptr_ = std::shared_ptr<NotifyEventHandler>(new NotifyEventHandler(EventLoop_WPtr(shared_from_this()),std::bind(&EventLoop::NewEventHandler,this)));
	notify_handler_ptr_->EnableRead();
}
void EventLoop::Loop()
{
    while(!is_quit_)
	{
		//printf("go there2-1\n");
		uint64 nearest_time = timer_manager_->GetNearestTime();
		if(nearest_time == 0)
			nearest_time = 10; //默认10ms
		else
			nearest_time = nearest_time - GetTimeOfNow();
		//uint64 start_t = GetTimeOfNow();
		//获取活跃套接字
		epoll_.GetActiveEvents(nearest_time);
		//uint64 end_t = GetTimeOfNow();
		//printf("%d\n",end_t-start_t);
		//printf("go there2\n");
		//处理套接字事件
		epoll_.DoActiveEvents();

		//处理时间事件
		timer_manager_->DoTimeEvent();
	//	printf("go there3\n");
	}
	printf("close server\n");
}

void EventLoop::NewEventHandler()
{
    MutexGuard mutex_guard(mutex_);
    int n = new_handler_queue_.size();
    for(int i = 0;i < n;i++)
    {
        EventHandler_SPtr new_handler = new_handler_queue_.front();
        new_handler_queue_.pop();
        new_handler->EnableRead();
    }
}

//inline void EventLoop::WakeUp()


//inline void EventLoop::Quit()

void EventLoop::AddNewHandler(EventHandler_SPtr &event_ptr)
{
    MutexGuard mutex_guard(mutex_);
    new_handler_queue_.push(event_ptr);
    WakeUp();
}



