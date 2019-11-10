#include <functional>
#include "EventLoop.h"
#include "Epoll.h"
#include "Logger.h"
#include "Channel.h"
#include "TimerManager.h"

IgnoreSigPipe initObj;

EventLoop::EventLoop():epoll_(),is_quit_(false),mutex_(), new_channel_queue_(),
notify_event_(std::bind(&EventLoop::NewConnectHandler,this)),timer_manager_(new TimerManager)
{
}

void EventLoop::Init()
{
	notify_event_.SetEventLoop(shared_from_this());
	notify_event_.EnableRead();
}
void EventLoop::Loop()
{
    while(!is_quit_)
	{
		
		//LOG_INFO << "eventloop num:" << i++;
		//printf("go there2-1\n");
		uint64 nearest_time = timer_manager_->GetNearestTime();
		if(nearest_time == 0)
			nearest_time = -1; //默认永久等待时间
		else
			nearest_time = nearest_time - GetTimeOfNow();
		//uint64 start_t = GetTimeOfNow();
		//获取活跃套接字
		epoll_.HandlerActiveEvents(nearest_time);
		//处理时间事件
		timer_manager_->DoTimeEvent();
	//	printf("go there3\n");
	}
	printf("close server\n");
}

void EventLoop::NewConnectHandler()
{
	LOG_DEBUG << "NewConnectHandler";
    MutexGuard mutex_guard(mutex_);
    int n = new_channel_queue_.size();
    for(int i = 0;i < n;i++)
    {
		Channel_WPtr new_channel = new_channel_queue_.front();
		new_channel_queue_.pop();
		auto channel_sptr = new_channel.lock();
		if(channel_sptr)
			channel_sptr->EnableRead();
    }
}

//inline void EventLoop::WakeUp()


//inline void EventLoop::Quit()

void EventLoop::AddNewChannel(Channel_WPtr channel)
{
	LOG_DEBUG << "AddNewChannel";
    MutexGuard mutex_guard(mutex_);
	new_channel_queue_.push(channel);
    WakeUp();
}

std::weak_ptr<TimerManager> EventLoop::GetTimerManager()
{
	return std::weak_ptr<TimerManager>(timer_manager_);
}
void EventLoop::AddTimer(std::weak_ptr<Timer> timer)
{
	timer_manager_->AddTimer(timer);
}



