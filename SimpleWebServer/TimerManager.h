#ifndef TIMERMANAGER__
#define TIMERMANAGER__

#include<vector>
#include <queue>
#include "NonCopy.h"
#include "Timer.h"
#include "Channel.h"
#include "EventLoop.h"

typedef unsigned long long uint64;
typedef unsigned int uint32;
uint64 GetTimeOfNow();
class TimerManager:public NonCopy
{
public:
	//typedef unsigned int uint32;
	//typedef unsigned long long uint64;
	typedef Channel::Channel_SPtr Channel_SPtr;
	typedef Channel::EventLoop_WPtr EventLoop_WPtr;
private:
	typedef std::weak_ptr<Timer> timer_wptr;
	class cmp
	{
	public:
		bool operator()(const timer_wptr &t1,const timer_wptr &t2)
		{

			if(t1.lock() != nullptr && t2.lock() != nullptr)
			{
				return t1.lock()->GetTime() > t2.lock()->GetTime();
			}
			else
			{
				return (t1.lock()==nullptr)?false:true;
			}
		}
	};
	std::priority_queue<timer_wptr,std::vector<timer_wptr>,cmp> timer_queue_;
	
public:
	void AddTimer(timer_wptr timer)
	{
		timer_queue_.push(timer);
	}
	void DoTimeEvent();
	uint64 GetNearestTime(); //获取最近发生事件的时间，返回时间为绝对时间,单位ms 如果返回0，说明没有时间事件
};

#endif