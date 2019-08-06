/****************************** */
//此类为线程间消息通知类
/****************************** */
#ifndef NOTIFYEVENTHANDLER__
#define NOTIFYEVENTHANDLER__
#include <functional>
#include <memory>
#include <unistd.h>
#include "EventHandler.h"
#include "Util.h"
#include "sys/types.h"
#include <sys/eventfd.h>

class NotifyEventHandler:public EventHandler 
{
public:
    typedef std::function<void()> func;
    typedef std::weak_ptr<EventLoop> wptr;
    NotifyEventHandler(wptr event_loop,func handle_read):handle_read_(handle_read),EventHandler(-1,event_loop) 
	{
		fd_ = eventfd(0,EFD_NONBLOCK);
		if(fd_ <0)
		{
			exit(-1);
		}
	}
    virtual void WriteHandle() 
    {
        uint64_t one = 1;
        int n = Writen(fd_, &one, sizeof(one));
        if (n != sizeof(one))
        {
            printf("NotifyEvent written error:\n");
        }
        DisableWrite();
    }
    virtual void ReadHandle() 
    {
        uint64_t one = 1;
        int n = Readn(fd_, &one, sizeof(one));
        if (n != sizeof one)
        {
            printf("NotifyEvent written error:%d bytes instead of 8\n",n);
        }
        handle_read_();
    }
    virtual void ErrorHandle() 
    {
        perror("listen socket error:");
        exit(-1);
    }
private:
    func handle_read_;
};

#endif