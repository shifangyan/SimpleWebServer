/**************************/
//此类为套接字事件处理基类，主要处理可读，可写及错误事件
/**************************/
//
#ifndef EVENTHANDLER__
#define EVENTHANDLER__
#include <sys/epoll.h>
#include <unistd.h>
#include <memory>
//#include "Epoll.h"
#include "NonCopy.h"
//#include "EventLoop.h"

class EventLoop;
class EventHandler:public NonCopy,public std::enable_shared_from_this<EventHandler>
{

private:
    __uint32_t events_;
    __uint32_t revents_;
    bool first_flag;
    static const int kReadEvent;
    static const int kWriteEvent;
    static const int kNoneEvent;
public:
    typedef std::weak_ptr<EventLoop> EventLoop_WPtr;
    typedef std::shared_ptr<EventLoop> EventLoop_SPtr;
    typedef std::weak_ptr<EventHandler> EventHandler_WPtr;
    typedef std::shared_ptr<EventHandler> EventHandler_SPtr;
protected:
    int fd_;
    //Epoll * epoll_ptr_;
    
    EventLoop_WPtr eventloop_;
    //EventHandler();
public:
    EventHandler(int fd):fd_(fd) {};
    EventHandler(int fd,EventLoop_WPtr eventloop):fd_(fd),eventloop_(eventloop),first_flag(true) {};
    virtual ~EventHandler()
    {
       // printf("destruct\n");
        close(fd_);
    }
    int GetFd() {return fd_;}
    // void SetEvents(__uint32_t events) 
    // { 
    //     events_ = events;
    //     if(first_flag)
    //     {
    //         first_flag = false;
    //         epoll_ptr_->EpollAdd(fd_,events_);
    //     }
    //     else
    //         epoll_ptr_->EpollMod(fd_,events_);
    // }
    __uint32_t GetEvents() const
    {
        return events_;
    }
    void SetREvents(__uint32_t revents) { revents_ = revents;}
    __uint32_t GetREvents() const
    {
        return revents_;
    }
    void EnableRead()
    {
        events_ |= kReadEvent;
        UpdateEvents();
    }
    void EnableWrite()
    {
        events_ |= kWriteEvent;
        UpdateEvents();
    }
    void EnableAll()
    {
        events_ |= (kReadEvent|kWriteEvent);
        UpdateEvents();
    }
    void DisableRead()
    {
        events_ &= ~kReadEvent;
        UpdateEvents();
    }
    void DisableWrite()
    {
        events_ &= ~kWriteEvent;
        UpdateEvents(); 
    }
    void DisableAll()
    {
        events_ &= kNoneEvent;
        UpdateEvents();
    }
    void UpdateEvents();
    virtual void WriteHandle() =0;
    virtual void ReadHandle() = 0;
    virtual void ErrorHandle() = 0;
    virtual void CloseHandle();
};
#endif