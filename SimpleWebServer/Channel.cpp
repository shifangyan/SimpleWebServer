#include "Channel.h"
#include "EventLoop.h"

const int Channel::kReadEvent = EPOLLIN | EPOLLPRI | EPOLLET;
const int Channel::kWriteEvent = EPOLLOUT | EPOLLET;
const int Channel::kNoneEvent = 0;

void Channel::UpdateEvents()
{
	auto sptr = eventloop_wptr_.lock();
	if (sptr)
	{
		auto this_sptr = shared_from_this();
		sptr->UpdateChannelEvent(this_sptr,new_channel_);
		new_channel_ = false;
	}
}

void Channel::HandlerEvent()
{
	if (revents_ & (EPOLLERR | EPOLLHUP)) //本端出错
	{
		LOG_INFO << "EPOLLERR | EPOLLHUP";
		ErrorHandler();
	}
	if (revents_ & (EPOLLIN | EPOLLPRI)) //新连接或者数据到来
	{
		LOG_INFO << "EPOLLIN|EPOLLPRI";
		ReadHandler();
		//     printf("readhandle\n");
	}
	if (revents_ & EPOLLOUT)//可写
	{
		LOG_INFO << "EPOLLOUT";
		WriteHandler();
	}
}
