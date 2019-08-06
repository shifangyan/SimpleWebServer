

#ifndef SERVER__
#define SERVER__

#include <memory>
#include "EventLoop.h"
#include "ThreadPool.h"
#include "NonCopy.h"

class Server:public NonCopy
{
private:
    typedef std::shared_ptr<EventLoop> EventLoop_SPtr;
    typedef std::weak_ptr<EventLoop> EventLoop_WPtr;
    typedef std::shared_ptr<ThreadPool> ThreadPool_SPtr;
    typedef std::weak_ptr<ThreadPool> ThreadPool_WPtr;
    EventLoop_SPtr listen_event_loop_;
    ThreadPool_SPtr thread_pool_;
public:
    Server(int port,int thread_num = 10);
    void Run();
};
#endif