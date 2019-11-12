/************************************** */
//此类为epoll事件循环类,每次循环需要处理套接字事件，时间事件
/************************************** */

#ifndef EVENTLOOP__
#define EVENTLOOP__

#include <queue>
#include <memory>
#include <signal.h>
#include "logger/NonCopy.h"
#include "NotifyEvent.h"
#include "Epoll.h"
#include "Mutex.h"
#include "Timer.h"
#include "Channel.h"

class TimerManager;
class IgnoreSigPipe
{
 public:
  IgnoreSigPipe()
  {
    ::signal(SIGPIPE, SIG_IGN);
    // LOG_TRACE << "Ignore SIGPIPE";
  }
};

class EventLoop:public NonCopy,public std::enable_shared_from_this<EventLoop>
{
public:
	typedef std::shared_ptr<EventLoop> EventLoop_SPtr;
	typedef std::weak_ptr<EventLoop> EventLoop_WPtr;
	typedef Channel::Channel_SPtr Channel_SPtr;
	typedef Channel::Channel_WPtr Channel_WPtr;
	
private:
    Epoll epoll_;
    //Thread *thread_ptr_;
    bool is_quit_;
    Mutex mutex_;
    std::queue<Channel_WPtr> new_channel_queue_;
    NotifyEvent notify_event_;
    std::shared_ptr<TimerManager> timer_manager_;
   
public:
    EventLoop();
    void Init();
    void Loop();
    void NewConnectHandler();
	void AddNewChannel(Channel_WPtr channel_wptr);
    inline void WakeUp()
    {
		notify_event_.WriteHandle();
    }
    inline void Quit()
    {
        printf("Quit\n");
		LOG_INFO << "Quit eventloop";
        is_quit_ = true;
        WakeUp();
    }
    void UpdateChannelEvent(Channel_WPtr channel_wptr,bool new_channel)
    {
		if (new_channel)
			epoll_.AddNewChannel(channel_wptr);
		else
			epoll_.UpdateChannelEvent(channel_wptr);
    }
    void DelChannel(Channel_WPtr channel_wptr)
    {
        epoll_.DelChannel(channel_wptr);
    }
	std::weak_ptr<TimerManager> GetTimerManager();
	void AddTimer(std::weak_ptr<Timer> timer);
};
#endif