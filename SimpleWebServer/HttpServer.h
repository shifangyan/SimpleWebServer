

#ifndef SERVER__
#define SERVER__

#include <memory>
#include "EventLoop.h"
#include "ThreadPool.h"
#include "NonCopy.h"
#include "AsyncLogging.h"
#include <vector>
#include "HttpConnection.h"
//#include "ListenSocket.h"

//class HttpConnection;
template<typename T>
class ListenSocket;
class HttpServer:public NonCopy
{
private:
    typedef std::shared_ptr<EventLoop> EventLoop_SPtr;
    typedef std::weak_ptr<EventLoop> EventLoop_WPtr;
    typedef std::shared_ptr<ThreadPool> ThreadPool_SPtr;
    typedef std::weak_ptr<ThreadPool> ThreadPool_WPtr;
	typedef std::shared_ptr<ListenSocket<HttpConnection>> ListenSocket_SPtr;
	typedef std::unique_ptr<HttpConnection>  HttpConnection_UPtr;
    EventLoop_SPtr listen_event_loop_;
    ThreadPool_SPtr thread_pool_;
	AsyncLogging async_logging_;
	ListenSocket_SPtr listen_sptr_;
	std::vector<HttpConnection_UPtr> http_vec_;
	static const int kMaxHttpConnection = 100000;
public:
	HttpServer(int port,int thread_num,std::string log_file_name,unsigned int flush_interval);
    void Run();
	void DelConnection(int fd);
	void AddConnection(HttpConnection_UPtr &connection);
};
#endif