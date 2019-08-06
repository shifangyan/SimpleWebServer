#include "EventHandler.h"
#include "EventLoop.h"

const int EventHandler::kReadEvent = EPOLLIN |EPOLLPRI | EPOLLET;
const int EventHandler::kWriteEvent = EPOLLOUT | EPOLLET;
const int EventHandler::kNoneEvent = 0;

void EventHandler::UpdateEvents()
{
    auto sptr = eventloop_.lock();
    if(sptr)
    {
        if(first_flag)
        {
            auto this_sptr = shared_from_this();
            sptr->UpdateEventHandlerForNew(this_sptr);
            first_flag = false;
        }
        else
        {
            auto this_sptr = shared_from_this();
            sptr->UpdateEventHandler(this_sptr);
        }
    }
}

void EventHandler::CloseHandle()
{
	//DisableAll();
    auto sptr = eventloop_.lock();
    if(sptr)
    {
       // printf("close event1\n");
        auto sptr_this = shared_from_this();
        sptr->DelEventHandler(sptr_this);
    }
}