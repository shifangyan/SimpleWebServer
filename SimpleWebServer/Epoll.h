/*************** */
//epoll类，主要实现事件的添加，修改，删除，事件获取
/***************** */
#ifndef EPOLL__
#define EPOLL__

#include <sys/epoll.h>
#include <map>
#include <vector>
#include <memory>
#include "NonCopy.h"
//#include "EventHandler.h"
#include "Channel.h"

//class EventHandler;
class Epoll:public NonCopy
{
public:
	typedef Channel::Channel_WPtr Channel_WPtr;
	typedef Channel::Channel_SPtr Channel_SPtr;
    typedef unsigned long long uint64;
private:
    int epoll_fd_;
    std::vector<struct epoll_event> active_events_;
   // std::map<int,EventHandler_SPtr> fd2event_handler_;
	static const int kMaxFd = 100000; //每个epoll最多只处理100000连接
	static const int kActiveEventSize = 1000;
	std::vector<Channel_WPtr> fd2Channel_;
   // std::map<int,sptr_handler> active_fd2event_;
    std::vector<Channel_WPtr> active_handler_;
    void EpollAdd(int fd, struct epoll_event &event); 
    void EpollMod(int fd, struct epoll_event &event);
    void EpollDel(int fd);
public:
    Epoll();
    ~Epoll(); 
	void AddNewChannel(Channel_WPtr channel_wptr);
    void UpdateChannelEvent(Channel_WPtr channel_wptr);
    void DelChannel(Channel_WPtr channel_wptr);
    //void EpollAdd(const EpollNode &epoll_node);
   
    //std::vector<sptr_handler > EpollWait(uint64 ms);
    void HandlerActiveEvents(uint64 ms);
    //void DoActiveEvents();
};
#endif