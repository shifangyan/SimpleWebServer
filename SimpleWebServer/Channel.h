#ifndef CHANNEL__
#define CHANNEL__

#include "logger/NonCopy.h"
//#include "EventLoop.h"
#include <memory>
#include <functional>
#include "logger/Logger.h"

class EventLoop;
class Channel:public NonCopy,public std::enable_shared_from_this<Channel>
{
public:
	//typedef EventLoop::EventLoop_WPtr EventLoop_WPtr;
	typedef std::weak_ptr<EventLoop> EventLoop_WPtr;
	//typedef std::shared_ptr<EventLoop> EventLoop_SPtr;
	typedef std::shared_ptr<Channel> Channel_SPtr;
	typedef std::weak_ptr<Channel> Channel_WPtr;
	typedef std::function<void()> WriteFunc;
	typedef std::function<void()> ReadFunc;
	typedef std::function<void()> ErrorFunc;
	typedef std::function<void()> CloseFunc;
	Channel(int fd):
		events_(0),
		revents_(0),
		fd_(fd),
		eventloop_wptr_(),
		write_func_(),
		read_func_(),
		error_func_(),
		close_func_(),
		new_channel_(true)
	{}
	Channel(int fd, EventLoop_WPtr wptr):
		events_(0),
		revents_(0),
		fd_(fd),
		eventloop_wptr_(wptr),
		write_func_(),
		read_func_(),
		error_func_(),
		close_func_(),
		new_channel_(true)
	{}

	void SetEventLoop(EventLoop_WPtr wptr)
	{
		eventloop_wptr_ = wptr;
	}
	void SetWriteFunc(WriteFunc func)
	{
		write_func_ = func;
	}
	void SetReadFunc(ReadFunc func)
	{
		read_func_ = func;
	}
	void SetErrorFunc(ErrorFunc func)
	{
		error_func_ = func;
	}
	void SetCloseFunc(CloseFunc func)
	{
		close_func_ = func;
	}
	void WriteHandler()
	{
		if (write_func_)
			write_func_();
		else
			LOG_WARN << "Not exist write_func_";
	}
	void ReadHandler()
	{
		if(read_func_)
			read_func_();
		else
			LOG_WARN << "Not exist read_func_";
	}
	void ErrorHandler()
	{
		if(error_func_)
			error_func_();
		else
			LOG_WARN << "Not exist error_func_";
	}
	/*void CloseHandler()
	{
		close_func_();
	}*/
	int fd()
	{
		return fd_;
	}
	__uint32_t events() const
	{
		return events_;
	}
	void SetREvents(__uint32_t revents) 
	{ 
		revents_ = revents; 
	}
	__uint32_t revents() const
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
		events_ |= (kReadEvent | kWriteEvent);
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
	void HandlerEvent();
private:
	static const int kReadEvent;
	static const int kWriteEvent;
	static const int kNoneEvent;
	__uint32_t events_;
	__uint32_t revents_;
	const int fd_;
	EventLoop_WPtr eventloop_wptr_;
	WriteFunc write_func_;
	ReadFunc read_func_;
	ErrorFunc error_func_;
	CloseFunc close_func_;
	bool new_channel_;
};

#endif
