#include "AsyncLogging.h"
#include <sys/fcntl.h>
#include <unistd.h>
#include <chrono>

using namespace std;
AsyncLogging::LogFile::LogFile(std::string file_name)
{
	fd_ = ::open(file_name.c_str(), O_WRONLY | O_CREAT | O_TRUNC | O_APPEND);
	if (fd_ < 0)
	{
		printf("can't open log file\n");
		exit(0);
	}
}

AsyncLogging::LogFile::~LogFile()
{
	if (fd_ > 0)
	{
		Flush();
		close(fd_);
	}
}

void AsyncLogging::LogFile::WriteLog(const char *data,size_t len)
{
	//阻塞write也需要考虑写入字节小于指定字节问题，原因：①物理存储介质空间不够 ②被信号中断
	ssize_t writed_size = 0;
	while (writed_size != len)
	{
		ssize_t ret = write(fd_, data + writed_size, len - writed_size);
		if (ret < 0)
		{
			printf("error:write file error\n");
			exit(0);
		}
		writed_size += ret;
	}
}

void AsyncLogging::LogFile::Flush()
{
	fsync(fd_);
}
void AsyncLogging::Append(const char *log_msg, int len)
{
	//MutexGuard mutex_guard(mutex_);
	std::lock_guard<std::mutex> mutex_guard(mutex_);
	if (currect_buffer_->Avail() > len) //正常情况
	{
		currect_buffer_->Append(log_msg, len);
	}
	else //当前buffer被填满 应当通知写日志线程及时写
	{
		buffer_vec_.push_back(std::move(currect_buffer_));
		if (alternate_buffer_) //还有备用buffer
		{
			currect_buffer_ = std::move(alternate_buffer_); //启用备用buffer
		}
		else //这种情况说明写日志频率很高  备用buffer也被写满 不正常情况，考虑扩容buffer
		{
			currect_buffer_.reset(new LargeBuffer);
		}
		currect_buffer_->Append(log_msg, len);
		//condition_.SignalAll();
		condition_.notify_all();
	}
}

void AsyncLogging::ThreadFunc() //写日志线程
{
	LargeBufferPtr new_buffer1(new LargeBuffer),new_buffer2(new LargeBuffer); //两个新buffer，用以交换日志buffer，减小临界区
	new_buffer1->InitZero();
	new_buffer2->InitZero();
	BufferPtrVec write_buffer_vec;
	write_buffer_vec.reserve(10);
	//必须使用do while,防止主进程结束时，日志线程直接结束
	do
	{
		{
			//MutexGuard guard(mutex_); //条件变量使用前需要上锁
			std::unique_lock<std::mutex> mutex_guard(mutex_);
			if (buffer_vec_.empty()) //基于等待时间的条件变量不使用while循环
			{
				//condition_.TimeWait(flush_interval_); //被前端线程唤醒或者指定刷新时间到
				std::chrono::seconds sec(3);
				condition_.wait_for(mutex_guard, sec);
			}
			buffer_vec_.push_back(std::move(currect_buffer_));
			swap(write_buffer_vec, buffer_vec_);
			currect_buffer_ = std::move(new_buffer1);
			if (!alternate_buffer_)
			{
				alternate_buffer_ = std::move(new_buffer2);
			}
			//交换完buffer后，前端就可以正常写日志了，不必等到后端将日志写到磁盘
		}

		if (write_buffer_vec.size() > 10) //日志过多 报警
		{
			char warn_msg[100] = "log message is too many\n";
			log_file_.WriteLog(warn_msg, strlen(warn_msg));
		}

		for (auto &buffer : write_buffer_vec)
		{
			log_file_.WriteLog(buffer->Data(), buffer->Length());
		}

		if (write_buffer_vec.size() > 2)
			write_buffer_vec.resize(2);
		new_buffer1 = std::move(write_buffer_vec.back());
		write_buffer_vec.pop_back();
		new_buffer1->Clear();
		if (!new_buffer2)
		{
			new_buffer2 = std::move(write_buffer_vec.back());
			new_buffer2->Clear();
		}
		write_buffer_vec.clear();
		//Flush();
		log_file_.Flush();
	} while (running_);
}
