/******************* */
//此类为ECHO服务器的已连接套接字事件处理
/******************** */
#ifndef ECHOSERVEREVENTHANDLER__
#define ECHOSERVEREVENTHANDLER__

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include "EventHandler.h"
#include "Util.h"
#include "EventLoop.h"

class EchoServerEventHandler:public EventHandler
{
private:
    static const int BUFSIZE = 1000; 
    char buf[BUFSIZE];
public:
    EchoServerEventHandler(int fd,std::weak_ptr<EventLoop> wptr):EventHandler(fd,wptr) {}
    //EchoServerEventHandler(int fd,__uint32_t events,Epoll *epoll_ptr):EventHandler(fd,events,events) {}
    virtual void WriteHandle() {}
    virtual void ReadHandle()
    {
        int n;
		if((n = Readn(fd_,buf,BUFSIZE)) < 0)
		{
            ErrorHandle();
		}
		else if(0 == n)
		{
            ErrorHandle();
		}
		else
		{
            //printf("go there write");
            int ret;
			if((ret = Writen(fd_,buf,n)) < 0)
			{
				//close(epfd);
                ErrorHandle();
				perror("write error:");
				//exit(-1);
			}
		}
    }
    virtual void ErrorHandle() 
    {
        perror("EchoServer socket error:");
        //this->epoll_ptr_->DelEventHandler(fd_);
        auto eventloop = eventloop_.lock();
        if(eventloop)
        {
            auto sptr = shared_from_this();
            eventloop->DelEventHandler(sptr);
        }
    }
};
#endif