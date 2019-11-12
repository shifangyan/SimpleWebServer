#ifndef TIMER__
#define TIMER__

#include <functional>
#include <memory>
#include "logger/NonCopy.h"
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
	
	uint64 when_; //����ʱ��
	func callback_func_;
	const uint32 interval_time_; //���ʱ�� 0����ִֻ��һ�� �������ѭ��ִ��
	bool use_;
	std::weak_ptr<TimerManager> timer_manager_;
public:
	Timer(uint32 interval_time,bool every,func callback_func,std::weak_ptr<TimerManager> sptr);
	void Run(); //ʱ�䵽�ڵ�ִ�к��� ����ûص������Լ��Ƿ����¼�ʱ�Ȳ���
	void SetInvalid() //���ö�ʱ�����Ϊ��ʹ��
	{
		use_ = false;
	}
	void Reset(); //���¼�ʱ ֻ��interval_time_��Ϊ0ʱ��ִ��
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