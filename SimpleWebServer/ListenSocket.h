#ifndef LISTENSOCKET__
#define LISTENSOCKET__
#include "Channel.h"
#include "EventLoop.h"
#include "Util.h"
#include <vector>
#include "ThreadPool.h"
#include <sys/socket.h>
#include "HttpServer.h"


template <typename T>
class ListenSocket:public NonCopy
{
public:
	typedef EventLoop::EventLoop_WPtr EventLoop_WPtr;
	typedef Channel::Channel_SPtr Channel_SPtr;
	typedef ThreadPool::ThreadPool_WPtr ThreadPool_WPtr;

	//template <typename T>
	ListenSocket(int port, EventLoop_WPtr wptr, ThreadPool_WPtr thread_pool_wptr, HttpServer *server);
//	template <typename T>
	void NewConnect();
//	template <typename T>
	~ListenSocket()
	{
		if (fd_ > 0)
			close(fd_);
	}

//	template <typename T>
	void EnableRead()
	{
		channel_sptr_->EnableRead();
	}
private:
	int fd_;
	Channel_SPtr channel_sptr_;
	ThreadPool_WPtr thread_pool_wptr_;
	HttpServer *http_server_;
};

template <typename T>
ListenSocket<T>::ListenSocket(int port, EventLoop_WPtr wptr, ThreadPool_WPtr thread_pool_wptr,HttpServer *server) :
	fd_(socket_bind_listen(port)),
	channel_sptr_(new Channel(fd_, wptr)),
	thread_pool_wptr_(thread_pool_wptr),
	http_server_(server)
{
	channel_sptr_->SetReadFunc(std::bind(&ListenSocket<T>::NewConnect, this));
	channel_sptr_->EnableRead();
}

template<typename T>
void ListenSocket<T>::NewConnect()
{
	while (1)
	{
		int connfd;
		if ((connfd = accept(fd_, NULL, NULL)) <= 0)
		{
			if (errno == EINTR) //被系统中断打断
			{
				continue; //继续读
			}
			else if (errno == EAGAIN) //套接字队列取完
			{
				// printf("/////////\n");
				break; //直接返回
			}
			else
			{
				LOG_ERROR << "listen socket error";
				break;
			}
		}
		else
		{
			//SetNonBlocking(connfd);
			std::unique_ptr<T> client(new T(connfd, http_server_));
			auto thread_pool = thread_pool_wptr_.lock();
			
			if (thread_pool)
			{
				thread_pool->AddNewConnect(client->channel());
			}
			http_server_->AddConnection(client);
		}
	}
}
#endif
