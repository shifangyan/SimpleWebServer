#ifndef LOGSTREAM__
#define LOGSTREAM__
#include <string>
#include <functional>
#include <cstring>
#include <algorithm>
#include "NonCopy.h"

template <size_t N>
class Buffer
{
public:
	Buffer():cur_(data_)
	{
		memset(data_,0, N);
	}
	Buffer(const Buffer &buffer)
	{
		printf("???????????\n");
	}
	void Append(const char *str, int len)
	{
		if (Avail() >= len) //日志太长 会拒绝写入
		{
			memcpy(cur_, str, len);
			cur_ += len;
		}
	}
	
	char *Data() 
	{
		return data_;
	}

	char *Current()
	{
		return cur_;
	}
	int Length() const
	{
		//printf("%p  %p", &data_,cur_);
		return static_cast<int>(cur_-data_);
	}

	int Avail()
	{
		return static_cast<int>(data_ + N - cur_);
	}
	void Add(int len)
	{
		cur_ += len;
	}
	void Clear()
	{
		cur_= data_;
	}
	void InitZero()
	{
		memset(data_, 0, N);
	}
private:
	char data_[N];
	char * cur_;
};

class LogStream:public NonCopy
{
public:
	typedef std::function<void(char *log, int len)> WriteLogFunc;
	typedef Buffer<1024> SmallBuffer;
public:
	LogStream() :buffer_() {}
	/*void SetWriteLogFunc(WriteLogFunc func)
	{
		func_ = func;
	}*/

	LogStream & operator<<(const char *str);
	LogStream & operator<<(std::string &str);
	LogStream & operator<<(bool b);
	LogStream & operator<<(short n);
	LogStream & operator<<(unsigned short n);
	LogStream & operator<<(int n);
	LogStream & operator<<(unsigned int n);
	LogStream & operator<<(long n);
	LogStream & operator<<(unsigned long n);
	LogStream & operator<<(long long n);
	LogStream & operator<<(unsigned long long n);
	LogStream & operator<<(float n);
	LogStream & operator<<(double n);
	
	template <typename T>
	void Convert(T n);

	SmallBuffer &GetBuffer()
	{
		return buffer_;
	}
private:
	SmallBuffer buffer_; //固定1KB，一条日志（包括附加信息）不能大于1KB 否则会忽视该日志，一条1KB的日志也太大了
	static const int kMaxNumericSize = 32;
	/*WriteLogFunc func_;*/
};

template <typename T>
void LogStream::Convert(T n)
{
	if (buffer_.Avail() >= kMaxNumericSize)
	{
		static const char num2char[19] = { '9','8','7','6','5','4','3','2','1','0','1','2','3','4','5','6','7','8','9' };
		const char *zero = num2char + 9;
		char *p, *cur;
		p = cur = buffer_.Current();
		T temp = n;
		do
		{
			*p++ = zero[temp % 10];
			temp = temp / 10;
		} while (temp != 0); //当n=0时也能正常执行
		if (n < 0)
			*p++ = '-';
		*p = '\0';
		std::reverse(cur, p);
		buffer_.Add(p - cur);
		//printf("%d\n", buffer_.Length());
	}
}

template <>
inline void LogStream::Convert(double n)
{
	if (buffer_.Avail() >= kMaxNumericSize)
	{
		int len = snprintf(buffer_.Current(), kMaxNumericSize, "%.12g", n);
		buffer_.Add(len);
	}
}
#endif

