/*********************/
//ʵ��һ�����ɸ��ƵĻ��࣬���в���Ҫ�������඼ֱ�Ӽ̳������
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