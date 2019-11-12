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
	//����writeҲ��Ҫ����д���ֽ�С��ָ���ֽ����⣬ԭ�򣺢�����洢���ʿռ䲻�� �ڱ��ź��ж�
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
	if (currect_buffer_->Avail() > len) //�������
	{
		currect_buffer_->Append(log_msg, len);
	}
	else //��ǰbuffer������ Ӧ��֪ͨд��־�̼߳�ʱд
	{
		buffer_vec_.push_back(std::move(currect_buffer_));
		if (alternate_buffer_) //���б���buffer
		{
			currect_buffer_ = std::move(alternate_buffer_); //���ñ���buffer
		}
		else //�������˵��д��־Ƶ�ʺܸ�  ����bufferҲ��д�� �������������������buffer
		{
			currect_buffer_.reset(new LargeBuffer);
		}
		currect_buffer_->Append(log_msg, len);
		//condition_.SignalAll();
		condition_.notify_all();
	}
}

void AsyncLogging::ThreadFunc() //д��־�߳�
{
	LargeBufferPtr new_buffer1(new LargeBuffer),new_buffer2(new LargeBuffer); //������buffer�����Խ�����־buffer����С�ٽ���
	new_buffer1->InitZero();
	new_buffer2->InitZero();
	BufferPtrVec write_buffer_vec;
	write_buffer_vec.reserve(10);
	//����ʹ��do while,��ֹ�����̽���ʱ����־�߳�ֱ�ӽ���
	do
	{
		{
			//MutexGuard guard(mutex_); //��������ʹ��ǰ��Ҫ����
			std::unique_lock<std::mutex> mutex_guard(mutex_);
			if (buffer_vec_.empty()) //���ڵȴ�ʱ�������������ʹ��whileѭ��
			{
				//condition_.TimeWait(flush_interval_); //��ǰ���̻߳��ѻ���ָ��ˢ��ʱ�䵽
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
			//������buffer��ǰ�˾Ϳ�������д��־�ˣ����صȵ���˽���־д������
		}

		if (write_buffer_vec.size() > 10) //��־���� ����
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
