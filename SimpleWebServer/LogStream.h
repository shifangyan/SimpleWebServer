#ifndef LOGSTREAM__
#define LOGSTREAM__
#include <string>
#include <functional>

using namespace std;

template <size_t N>
class Buffer
{
public:
	Buffer():len_(sizeof(data_),cur_len_(0)
	{
		memset(data_, len_, 0);
	}
	void Append(char *str, int len)
	{
		if (len_ - cur_len_ >= len) //日志太长 会拒绝写入
		{
			memcopy(data_ + cur_len_, str, len);
			cur_len_ += len;
		}
	}
	
	const char *data() const
	{
		return data_;
	}

	int length() const
	{
		return cur_len_;
	}

	void Clear()
	{
		cur_len_ = 0;
	}
private:
	char data_[N];
	int cur_len_;
	int len_;
};

class LogStream
{
public:
	typedef function<void(char *log, int len)> WriteLogFunc;
public:
	LogStream():buffer_(1024,'';
	~LogStream();
	/*void SetWriteLogFunc(WriteLogFunc func)
	{
		func_ = func;
	}*/


private:
	Buffer<1024> buffer_; //固定1KB，一条日志（包括附加信息）不能大于1KB 否则会忽视该日志，一条1KB的日志也太大了
	/*WriteLogFunc func_;*/
};

#endif

