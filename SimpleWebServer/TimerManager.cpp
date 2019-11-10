#include "TimerManager.h"
#include <sys/time.h>
#include <stdio.h>
#include <sys/timerfd.h>
#include <fcntl.h>

uint64 GetTimeOfNow()
{
	struct timeval tv;
    //struct timezone tz;
    ::gettimeofday(&tv,NULL);
    uint64 time_ms = tv.tv_sec*1000+tv.tv_usec/1000;
    return time_ms;
}

//int TimerCreate()
//{
//	int fd = timerfd_create(CLOCK_REALTIME, O_NONBLOCK);
//	if (fd < 0)
//	{
//		LOG_ERROR << "create timer fd error";
//		exit(0);
//	}
//	return fd;
//}

void TimerManager::DoTimeEvent()
{
    uint64 now_ms = GetTimeOfNow();
    while(!timer_queue_.empty())
    {
       // printf("start deal with timer event\n");
       // printf("timer event size:%d\n",timer_queue_.size());
        auto timer_sptr = timer_queue_.top().lock();
       // printf("use_count:%d\n",timer_sptr.use_count());
        if(timer_sptr != nullptr && timer_sptr->IsValid())
        {
       //     printf("use_count:%d\n",timer_sptr.use_count());
            if(now_ms < timer_sptr->GetTime() )
                break;
       //     printf("use_count:%d\n",timer_sptr.use_count());
            timer_sptr->Run();
        }
      //  printf("use_count:%d\n",timer_sptr.use_count());
        timer_queue_.pop();
      //  printf("use_count:%d\n",timer_sptr.use_count());
    }
}

uint64 TimerManager::GetNearestTime()
{
    while(!timer_queue_.empty())
    {
     //   printf("GetTime\n");
        auto timer_sptr = timer_queue_.top().lock();
    //    printf("use_count:%d\n",timer_sptr.use_count());
        if(timer_sptr != nullptr && timer_sptr->IsValid())
        {
      //      printf("use_count:%d\n",timer_sptr.use_count());
            return timer_sptr->GetTime();
        } 
     //   printf("use_count:%d\n",timer_sptr.use_count());
    }
    return 0; 
}