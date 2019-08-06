#include "Timer.h"
#include "TimerManager.h"

Timer::Timer(uint32 interval_time,bool every,func callback_func,std::weak_ptr<TimerManager> wptr):
when_(GetTimeOfNow()+interval_time),callback_func_(callback_func),
use_(true),interval_time_((every==true)?interval_time:0),timer_manager_(wptr)
{}

void Timer::Run()
{
    callback_func_();
	if(interval_time_ !=0)
	{
		Reset();
	}
}

void Timer::Reset()
{
    when_ += interval_time_;
	auto timer_sptr = timer_manager_.lock();
	if(timer_sptr!=nullptr)
	{
		timer_sptr->AddTimer(wptr(shared_from_this()));
	}
}