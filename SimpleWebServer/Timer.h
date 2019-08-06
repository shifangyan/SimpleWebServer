#ifndef TIMER__
#define TIMER__

#include <functional>
#include <memory>
#include "NonCopy.h"
//#include "TimerManager.h"

class TimerManager;
class Timer:public NonCopy,public std::enable_shared_from_this<Timer>
{
public:
	typedef unsigned long long uint64;
	typedef unsigned int uint32;
	typedef std::function<void()> func;
	typedef std::weak_ptr<Timer> wptr;
	typedef std::shared_ptr<Timer> sptr;
private:
	
	uint64 when_; //到期时间
	func callback_func_;
	const uint32 interval_time_; //间隔时间 0代表只执行一次 否则代表循环执行
	bool use_;
	std::weak_ptr<TimerManager> timer_manager_;
public:
	Timer(uint32 interval_time,bool every,func callback_func,std::weak_ptr<TimerManager> sptr);
	void Run(); //时间到期的执行函数 会调用回调函数以及是否重新计时等操作
	void SetInvalid() //将该定时器标记为不使用
	{
		use_ = false;
	}
	void Reset(); //重新计时 只在interval_time_不为0时才执行
	uint64 GetTime()
	{
		return when_;
	}
	bool IsValid()
	{
		return use_;
	}
};
#endif