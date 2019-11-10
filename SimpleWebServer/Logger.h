#ifndef LOGGER__
#define LOGGER__
#include "LogStream.h"
#include <functional>

class Logger
{
public:
	enum Level
	{
		TRACE,
		DEBUG,
		INFO,
		WARN,
		ERROR,
		FATAL
	};
	class LogFormat
	{
	public:
		LogFormat(const char * source_file, int line, Level level) :
			source_file_(source_file),
			line_(line),
			level_(level)
		{
			FormatTime();
			FormatTid();
			FormatLevel();
		}
		LogStream &GetLogStream()
		{
			return log_stream_;
		}
		void Finish();
	private:
		const char *source_file_;
		int line_;
		Level level_;
		LogStream log_stream_;
		void FormatTime();
		void FormatTid();
		void FormatLevel();
	};
	typedef std::function<void(const char * mag, int len)> OutputFunc;
	typedef std::function<void()> FlushFunc;
public:
	Logger(const char *source_file,int line,Level level):
		log_format_(source_file, line, level),
		currect_level_(level)
	{};
	~Logger() 
	{
		Finish();
		auto &buffer = GetLogStream().GetBuffer();
		output_func_(buffer.Data(), buffer.Length());
		if (currect_level_ == FATAL || currect_level_ == DEBUG)
		{
			flush_func_();
		}
	}
	static Level GetLevel()
	{
		return level_;
	}
	static void SetLevel(Level level)
	{
		level_ = level;
	}
	LogStream &GetLogStream()
	{
		return log_format_.GetLogStream();
	}
	static void SetOutput(OutputFunc func)
	{
		output_func_ = func;
	}
	static void SetFlush(FlushFunc func)
	{
		flush_func_ = func;
	}
private:
	void Finish()
	{
		log_format_.Finish();
	}
	static Level level_;
	static OutputFunc output_func_;
	static FlushFunc flush_func_;
	//LogStream log_stream_;
	LogFormat log_format_;
	Level currect_level_;
};

#define LOG_TRACE if(Logger::GetLevel() <= Logger::Level::TRACE) \
Logger(__FILE__,__LINE__,Logger::Level::TRACE).GetLogStream()
#define LOG_DEBUG if(Logger::GetLevel() <= Logger::Level::DEBUG) \
Logger(__FILE__,__LINE__,Logger::Level::DEBUG).GetLogStream()
#define LOG_INFO if (Logger::GetLevel() <= Logger::Level::INFO) \
Logger(__FILE__,__LINE__,Logger::Level::INFO).GetLogStream()
#define LOG_WARN if (Logger::GetLevel() <= Logger::Level::WARN) \
Logger(__FILE__,__LINE__,Logger::Level::WARN).GetLogStream()
#define LOG_ERROR if (Logger::GetLevel() <= Logger::Level::ERROR) \
Logger(__FILE__,__LINE__,Logger::Level::ERROR).GetLogStream()
#define LOG_FATEL if (Logger::GetLevel() <= Logger::Level::FATAL) \
Logger(__FILE__,__LINE__,Logger::Level::FATAL).GetLogStream()
#endif

