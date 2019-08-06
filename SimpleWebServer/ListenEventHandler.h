/******************* */
//此类为监听套接字的事件处理类
/******************** */

#ifndef LISTENEVENTHANDLER__
#define LISTENEVENTHANDLER__

#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <memory>
#include "EventHandler.h"
#include "EchoServerEventHandler.h"
#include "ThreadPool.h"
#include "HttpServerEventHandler.h"

class ListenEventHandler:public EventHandler
{
private:
    typedef std::weak_ptr<ThreadPool> ThreadPool_WPtr;
    typedef std::shared_ptr<ThreadPool> ThreadPool_SPtr;
    ThreadPool_WPtr thread_pool_;
	int port_;
public:
    ListenEventHandler(int port,EventLoop_WPtr wptr,ThreadPool_WPtr thread_pool):thread_pool_(thread_pool),
    EventHandler(-1,wptr) 
	{
		fd_ = socket_bind_listen(port);
		if(fd_ <=0)
		{
			perror("socket_bind_listen error:");
			exit(-1);
		}
	}
    //ListenEventHandler(int fd,__uint32_t events,Epoll *epoll_ptr):epoll_ptr_(epoll_ptr),EventHandler(fd,events) {}
    virtual void WriteHandle() {
        printf("listen write\n");
    }
    virtual void ReadHandle() 
    {
       // printf("go there read handle\n");
       // struct sockaddr_in cliaddr;
		//socklen_t clilen = sizeof(cliaddr);	
        while(1)
        {
            int connfd;			
		    if((connfd = accept(fd_,NULL,NULL)) <= 0)
		    {
                if(errno == EINTR) //被系统中断打断
                {
               // printf("??????\n");
                    continue; //继续读
                }
                 else if(errno == EAGAIN) //套接字队列取完
                {
               // printf("/////////\n");
                    break; //直接返回
                }
                else
                {
                    perror("accept error:");
			        ErrorHandle();
                }
		    }
            else
            {
                SetNonBlocking(connfd);
                ThreadPool_SPtr thread_pool = thread_pool_.lock();
                if(thread_pool)
                {
                    thread_pool->AddNewConnect<HttpServerEventHandler>(connfd);
                }
                else
                {
                    printf("error:threadpoll not exist\n");
                }
                
                // EventHandler *temp = new EchoServerEventHandler(connfd,epoll_ptr_);
                // std::shared_ptr<EventHandler> cli_handler(temp);
                // cli_handler->SetEvents(EPOLLIN|EPOLLET);
                // epoll_ptr_->AddEventHandler(cli_handler);
            }   
        }
		
      //  printf("got there read handler end\n");
    }
    virtual void ErrorHandle() 
    {
        perror("listen socket error:");
        exit(-1);
    }
};
#endif