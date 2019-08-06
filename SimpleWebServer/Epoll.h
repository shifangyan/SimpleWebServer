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
#include "EventHandler.h"

//class EventHandler;
class Epoll:public NonCopy
{
public:
    typedef std::shared_ptr<EventHandler> sptr_handler;
    typedef EventHandler::EventHandler_SPtr EventHandler_SPtr;
    typedef EventHandler::EventHandler_WPtr EventHandler_WPtr;
    typedef unsigned long long uint64;
private:
    int epoll_fd_;
    std::vector<struct epoll_event> active_events_;
    std::map<int,EventHandler_SPtr> fd2event_handler_;
   // std::map<int,sptr_handler> active_fd2event_;
    std::vector<EventHandler_SPtr> active_handler_;
    void EpollAdd(EventHandler_SPtr &sh_ptr);
    void EpollMod(EventHandler_SPtr &sh_ptr);
    void EpollDel(EventHandler_SPtr &sh_ptr);
public:
    Epoll();
    ~Epoll(); 
    void UpdateEventHandlerForNew(EventHandler_SPtr &sh_ptr);
    void UpdateEventHandler(EventHandler_SPtr &sh_ptr);
    void DelEventHandler(EventHandler_SPtr &sh_ptr);
    //void EpollAdd(const EpollNode &epoll_node);
   
    //std::vector<sptr_handler > EpollWait(uint64 ms);
    void GetActiveEvents(uint64 ms);
    void DoActiveEvents();
};
#endif