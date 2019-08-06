/************************************** */
//此类为epoll事件循环类,每次循环需要处理套接字事件，时间事件
/************************************** */

#ifndef EVENTLOOP__
#define EVENTLOOP__

#include <queue>
#include <memory>
#include <signal.h>
#include "NonCopy.h"
#include "NotifyEventHandler.h"
#include "Epoll.h"
#include "Mutex.h"
#include "EventHandler.h"
#include "TimerManager.h"
#include "Timer.h"

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
    typedef EventHandler::EventHandler_WPtr EventHandler_WPtr;
    typedef EventHandler::EventHandler_SPtr EventHandler_SPtr;
    typedef EventHandler::EventLoop_WPtr EventLoop_WPtr;
    typedef EventHandler::EventLoop_SPtr EventLoop_SPtr;
private:
    Epoll epoll_;
    //Thread *thread_ptr_;
    bool is_quit_;
    Mutex mutex_;
    std::queue<EventHandler_SPtr> new_handler_queue_;
    std::shared_ptr<NotifyEventHandler> notify_handler_ptr_;
    std::shared_ptr<TimerManager> timer_manager_;
   
public:
    EventLoop();
    void Init();
    void Loop();
    void NewEventHandler();
    inline void WakeUp()
    {
        notify_handler_ptr_->EnableWrite();
        //notify_handler_ptr_->WriteHandle();
    }
    inline void Quit()
    {
        printf("Quit\n");
        is_quit_ = true;
        WakeUp();
    }
    void AddNewHandler(EventHandler_SPtr &event_ptr);
    // Epoll &GetEpoll()
    // {
    //     return epoll_;
    // }
    void UpdateEventHandlerForNew(EventHandler_SPtr &sh_ptr)
    {
        epoll_.UpdateEventHandlerForNew(sh_ptr);
    }
    void UpdateEventHandler(EventHandler_SPtr &sh_ptr)
    {
        epoll_.UpdateEventHandler(sh_ptr);
    }
    void DelEventHandler(EventHandler_SPtr &sh_ptr)
    {
        epoll_.DelEventHandler(sh_ptr);
    }
    std::weak_ptr<TimerManager> GetTimerManager()
    {
        return std::weak_ptr<TimerManager>(timer_manager_);
    }
    void AddTimer(std::weak_ptr<Timer> timer)
    {
        timer_manager_->AddTimer(timer);
    }
};
#endif