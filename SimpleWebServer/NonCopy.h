/*********************/
//实现一个不可复制的基类，所有不需要拷贝的类都直接继承这个类
/**********************/
#ifndef NONCOPY__
#define NONCOPY__
class NonCopy
{
public:
	NonCopy() {}
private:
	NonCopy(const NonCopy &);
	NonCopy& operator=(const NonCopy&) ;
};

#endif