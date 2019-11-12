#include "Logger.h"
#include <sys/time.h>
#include <assert.h>
#include "ThreadTid.h"
#include "AsyncLogging.h"

inline void DefaultOutput(const char *msg, int len)
{
	fwrite(msg, 1, len, stdout);
}

inline void DefaultFlush()
{
	fflush(stdout);
}
Logger::Level Logger::level_ = Logger::INFO;
Logger::OutputFunc Logger::output_func_ = DefaultOutput;
Logger::FlushFunc Logger::flush_func_ = DefaultFlush;



__thread time_t last_second;
__thread char time_str[26];
void Logger::LogFormat::FormatTime()
{
	struct timeval tv;
	::gettimeofday(&tv, NULL);
	time_t second = static_cast<time_t>(tv.tv_sec);
	int micro_second = static_cast<int>(tv.tv_usec);
	if (second != last_second)
	{
		last_second = second;
		struct tm time;
		localtime_r(&second,&time); //带r后缀的是在参数中返回结果的，不带的以返回值返回
		//gmtime_r(&second,&time);
		int len = snprintf(time_str, sizeof(time_str), "%4d%02d%02d %02d:%02d:%02d:%06d ",
			time.tm_year + 1900, time.tm_mon + 1, time.tm_mday, time.tm_hour, time.tm_min, time.tm_sec,micro_second); //格式化year:mon:day hour:min:sec
		assert(len == 25);
	}
	else
	{
		int len = snprintf(time_str + 18, sizeof(time_str) - 18, "%06d ", micro_second);
		assert(len == 7);
	}
	log_stream_ << time_str;
}
void Logger::LogFormat::FormatTid()
{
	const char *tid = GetTidString();
	log_stream_ << tid << " ";
}
void Logger::LogFormat::FormatLevel()
{
	static char *level_str[6] = { "TRACE ","DEBUG ","INFO  ","WARN  ","ERROR ","FATAL " };
	log_stream_ << level_str[level_];
}
void Logger::LogFormat::Finish()
{
	log_stream_ << " - " << source_file_ << ":" << line_ << "\n";
}