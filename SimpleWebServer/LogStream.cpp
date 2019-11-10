#include "LogStream.h"

using namespace std;

LogStream & LogStream::operator<<(const char *str)
{
	int len = strlen(str);
	if (len)
	{
		buffer_.Append(str, len);
	}
	else
	{
		buffer_.Append("(NULL)", 6);
	}
	return *this;
}

LogStream & LogStream::operator<<(string &str)
{
	*this << str.c_str();
	return *this;
}

LogStream & LogStream::operator<<(bool b)
{
	*this << (b ? "true" : "false");
	return *this;
}
LogStream & LogStream::operator<<(short n)
{
	*this << static_cast<int>(n);
	return *this;
}
LogStream & LogStream::operator<<(unsigned short n)
{
	*this << static_cast<unsigned int>(n);
	return *this;
}
LogStream & LogStream::operator<<(int n)
{
	Convert(n);
	return *this;
}
LogStream & LogStream::operator<<(unsigned int n)
{
	Convert(n);
	return *this;
}
LogStream & LogStream::operator<<(long n)
{
	Convert(n);
	return *this;
}
LogStream & LogStream::operator<<(unsigned long n)
{
	Convert(n);
	return *this;
}
LogStream & LogStream::operator<<(long long n)
{
	Convert(n);
	return *this;
}
LogStream & LogStream::operator<<(unsigned long long n)
{
	Convert(n);
	return *this;
}

LogStream & LogStream::operator<<(float n)
{
	*this << static_cast<double>(n);
	return *this;
}
LogStream & LogStream::operator<<(double n)
{
	Convert(n);
	return *this;
}
