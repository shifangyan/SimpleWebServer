#include <unistd.h>
#include <memory>
#include "HttpServer.h"
#include "Util.h"
#include "ListenSocket.h"
#include "Logger.h"
#include "HttpConnection.h"

using namespace std;
using namespace placeholders;
HttpServer::HttpServer(int port, int thread_num, std::string log_file_name, unsigned int flush_interval):
	listen_event_loop_(new EventLoop),
	thread_pool_( new ThreadPool(thread_num)),
	async_logging_(log_file_name,flush_interval),
	listen_sptr_(new ListenSocket<HttpConnection>(port, EventLoop_WPtr(listen_event_loop_), ThreadPool_WPtr(thread_pool_), this)),
	http_vec_(kMaxHttpConnection)
{
    //int listenfd = socket_bind_listen(port); //默认非阻塞
	//if(listenfd <=0)
	//{
	//	perror("socket_bind_listen error:");
	//	exit(-1);
	//}
	//设置日志
	Logger::SetLevel(Logger::Level::DEBUG);
	Logger::SetOutput(std::bind(&AsyncLogging::Append, &async_logging_, _1, _2));
	Logger::SetFlush(std::bind(&AsyncLogging::Flush, &async_logging_));
    listen_event_loop_->Init();
   // listen_event_loop_.AddNewHandler(listen_handler);
//	listen_sptr_->EnableRead();
	//printf("size:%d", http_vec_.size());
	LOG_DEBUG << "size:" << http_vec_.size();
	LOG_DEBUG << "size:" << http_vec_.size();
}

void HttpServer::Run()
{
    listen_event_loop_->Loop();
}

void HttpServer::DelConnection(int fd)
{
	http_vec_[fd].reset();
}

void HttpServer::AddConnection(HttpConnection_UPtr &connection)
{
	
	LOG_DEBUG << http_vec_.size() << " " << connection->fd() << " " << kMaxHttpConnection;
	http_vec_[connection->fd()] = std::move(connection);
}

