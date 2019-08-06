#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include "Epoll.h"
#include "EventHandler.h"

const int EVENTS_SIZE = 1000;
const int EPOLLWAIT_TIME = 10000;

Epoll::Epoll():active_events_(EVENTS_SIZE)
{
    if((epoll_fd_ = epoll_create(1)) < -1)
    {
        perror("epoll_create error:");
        exit(0);
    }
}

Epoll::~Epoll()
{
    close(epoll_fd_);
}

void Epoll::UpdateEventHandlerForNew(sptr_handler &sh_ptr)
{
   // printf("fd:%d\n",sh_ptr->GetFd());
    int fd = sh_ptr->GetFd();
    fd2event_handler_[fd] = sh_ptr;
    //fd2event_handler_[sh_ptr->GetFd()] = sh_ptr;
    EpollAdd(sh_ptr);
}

void Epoll::UpdateEventHandler(sptr_handler &sh_ptr)
{
   // printf("fd:%d\n",sh_ptr->GetFd());
    int fd = sh_ptr->GetFd();
   // fd2event_handler_.insert({fd,sh_ptr});
    if(fd2event_handler_[fd]==nullptr )
        printf("shared ptr is null\n");
    EpollMod(sh_ptr);
   
}
void Epoll::DelEventHandler(sptr_handler &sh_ptr)
{
   // printf("Delete event\n");
   // EpollDel(sh_ptr);
   // printf("del handler1\n");
    int ret = fd2event_handler_.erase(sh_ptr->GetFd());
    if(ret == 0)
        printf("%d dont find",sh_ptr->GetFd());
   // printf("del handler2\n");
}
// void Epoll::EpollAdd(const EpollNode& epoll_node)
// {
//     struct epoll_event event;
//     int fd = epoll_node.fd_;
//     event.data.fd = epoll_node.fd_;
//     event.events = epoll_node.events_;
//     int ret;
//     if((ret = epoll_ctl(epoll_fd_,EPOLL_CTL_ADD,fd,&event)) < 0)
//     {
//         perror("epoll_ctl error:");
//         exit(0);
//     }
// }

void Epoll::EpollAdd(sptr_handler &sh_ptr)
{
    struct epoll_event event;
    //int fd = epoll_node.fd_;
    int fd = sh_ptr->GetFd();
    event.data.fd = fd;
    event.events = sh_ptr->GetEvents();
    event.data.ptr = static_cast<void *>(&(fd2event_handler_[sh_ptr->GetFd()]));
    int ret;
    if((ret = epoll_ctl(epoll_fd_,EPOLL_CTL_ADD,fd,&event)) < 0)
    {
       // perror("epoll_add error:");
       // exit(0);
    }
}

void Epoll::EpollMod(sptr_handler &sh_ptr)
{
    struct epoll_event event;
    //int fd = epoll_node.fd_;
    int fd = sh_ptr->GetFd();
    event.data.fd = fd;
    event.events = sh_ptr->GetEvents();
    event.data.ptr = static_cast<void *>(&(fd2event_handler_[sh_ptr->GetFd()]));
    int ret;
    if((ret = epoll_ctl(epoll_fd_,EPOLL_CTL_MOD,fd,&event)) == -1)
    {
        perror("epoll_mod error:");
        exit(0);
    }
}

void Epoll::EpollDel(EventHandler_SPtr &sh_ptr)
{
    struct epoll_event event;
    //int fd = epoll_node.fd_;
    int fd = sh_ptr->GetFd();
    event.data.fd = fd;
    event.events = sh_ptr->GetEvents();
    int ret;
    if((ret = epoll_ctl(epoll_fd_,EPOLL_CTL_DEL,fd,&event)) < 0)
    {
        perror("epoll_del error:");
        exit(0);
    }
}

void Epoll::GetActiveEvents(uint64 ms)
{
      //  printf("%d\n",static_cast<int>(fd2event_handler_.size()));
   // printf("link socket size:%d\n",fd2event_handler_.size());  
    int nready;
    if((nready = epoll_wait(epoll_fd_,&(*(active_events_.begin())),active_events_.size(),ms)) < 0)
    {
        perror("epoll_wait error:");
        exit(0);
    }
       // printf("%d\n",nready);
    //std::vector<std::shared_ptr<EventHandler> > result_vec;
    for(int i = 0;i < nready;i++)
    {
        EventHandler_SPtr *event_sptr =static_cast<EventHandler_SPtr *>(active_events_[i].data.ptr);
        (*event_sptr)->SetREvents(active_events_[i].events);
        active_handler_.push_back(*event_sptr);
        //auto ret = fd2event_handler_.find(events_[i].data.fd);
        // if(ret != fd2event_handler_.end())
        // {
        //     ret->second->SetREvents(events_[i].events);
        //     result_vec.push_back(ret->second);
        // }
           // req_event.revents_ = events_[i].events;
           // req_events.push_back(req_event);
    }
       // int s = req_events.size();
        //printf("%d  %d\n",s,nready);
    //if(result_vec.size() > 0)
}

void Epoll::DoActiveEvents()
{
    for(int i = 0;i<active_handler_.size();++i)
    {
		__uint32_t revents = (active_handler_[i])->GetREvents();
		if(revents & (EPOLLERR |EPOLLHUP)) //本端出错
		{
		//	perror("go there error\n");
			(active_handler_[i])->ErrorHandle();
		}
		if(revents & (EPOLLIN|EPOLLPRI)) //新连接或者数据到来
		{
			//	printf("")
			(active_handler_[i])->ReadHandle();
       //     printf("readhandle\n");
		}
        if(revents & EPOLLOUT)//可写
        {
       //     printf("before writehandle\n");
            (active_handler_[i])->WriteHandle();
      //      printf("writehandle\n");
        }
        // if(revents & (EPOLLIN | EPOLLRDHUP)) //对端关闭
        // {
        //     printf("close\n");
        // }
    }
    active_handler_.clear();
}