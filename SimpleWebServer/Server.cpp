#include <unistd.h>
#include <memory>
#include "Server.h"
#include "Util.h"
#include "ListenEventHandler.h"

Server::Server(int port,int thread_num):listen_event_loop_(new EventLoop),
thread_pool_( new ThreadPool(thread_num))
{
    //int listenfd = socket_bind_listen(port); //默认非阻塞
	//if(listenfd <=0)
	//{
	//	perror("socket_bind_listen error:");
	//	exit(-1);
	//}
    listen_event_loop_->Init();
    std::shared_ptr<EventHandler> listen_handler(new ListenEventHandler(port,EventLoop_WPtr(listen_event_loop_),
    ThreadPool_WPtr(thread_pool_)));
   // listen_event_loop_.AddNewHandler(listen_handler);
    listen_handler->EnableRead();
}

void Server::Run()
{
    listen_event_loop_->Loop();
}

