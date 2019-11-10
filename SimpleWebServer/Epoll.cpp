#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include "Epoll.h"
#include "Logger.h"
#include <assert.h>

Epoll::Epoll():
	active_events_(kActiveEventSize),
	fd2Channel_(kMaxFd)
{
    if((epoll_fd_ = epoll_create(1)) < -1)
    {
        LOG_ERROR << "epoll_create error:";
        exit(0);
    }
}

Epoll::~Epoll()
{
    close(epoll_fd_);
}

void Epoll::AddNewChannel(Channel_WPtr channel_wptr)
{
	auto channel_sptr = channel_wptr.lock();
	if (channel_sptr)
	{
		int fd = channel_sptr->fd();
		//assert(fd2Channel_[fd] ==  nullptr);
		fd2Channel_[fd] = channel_wptr;
		struct epoll_event event;
		event.events = channel_sptr->events();
		event.data.fd = fd;
		EpollAdd(fd, event);
	}
}
void Epoll::UpdateChannelEvent(Channel_WPtr channel_wptr)
{
	auto channel_sptr = channel_wptr.lock();
	if (channel_sptr)
	{

		int fd = channel_sptr->fd();
		//assert(fd2Channel[fd] != nullptr);
		//fd2Channel[fd] = channel_wptr;
		struct epoll_event event;
		event.events = channel_sptr->events();
		event.data.fd = fd;
		EpollMod(fd, event);
	}
}


void Epoll::DelChannel(Channel_WPtr channel_wptr)
{
	auto channel_sptr = channel_wptr.lock();
	if (channel_sptr)
	{
		int fd = channel_sptr->fd();
		//assert(fd2Channel[fd] != nullptr);
		fd2Channel_[fd].reset();
		EpollDel(fd);
	}
}

 void Epoll::EpollAdd(int fd, struct epoll_event &event)
 {
     int ret;
     if((ret = epoll_ctl(epoll_fd_,EPOLL_CTL_ADD,fd,&event)) < 0)
     {
         perror("epoll_ctl error:");
         exit(0);
     }
 }

void Epoll::EpollMod(int fd, struct epoll_event &event)
{
    int ret;
    if((ret = epoll_ctl(epoll_fd_,EPOLL_CTL_MOD,fd,&event)) == -1)
    {
		LOG_ERROR <<  "epoll_mod error:";
        exit(0);
    }
}

void Epoll::EpollDel(int fd)
{
	int ret;
    if((ret = epoll_ctl(epoll_fd_,EPOLL_CTL_DEL,fd,NULL)) < 0)
    {
		LOG_ERROR << "epoll_del error:";
        exit(0);
    }
}

void Epoll::HandlerActiveEvents(uint64 ms)
{
    int nready;
   /* if((nready = epoll_wait(epoll_fd_,&(*(active_events_.begin())),active_events_.size(),ms)) < 0)
    {
		LOG_ERROR << "epoll_wait error:";
        exit(0);
    }*/
	if ((nready = epoll_wait(epoll_fd_, active_events_.data(), active_events_.size(), ms)) < 0)
	{
		LOG_ERROR << "epoll_wait error:";
		exit(0);
	}
       // printf("%d\n",nready);
    //std::vector<std::shared_ptr<EventHandler> > result_vec;
    for(int i = 0;i < nready;i++)
    {
		auto channel_sptr = fd2Channel_[active_events_[i].data.fd].lock();
		if (channel_sptr)
		{
			channel_sptr->SetREvents(active_events_[i].events);
			channel_sptr->HandlerEvent();
		}
    }
}

