#include <sys/syscall.h>
#include <unistd.h>
#include <stdio.h>
#include <assert.h>

__thread int tid = 0;
__thread char tid_str[32] = { 0 };
__thread int tid_str_length = 0;

void CacheTid()
{
	tid = syscall(SYS_gettid);
	int len = snprintf(tid_str, sizeof(tid_str), "%05d", tid);
	assert(len == 5);
}
int GetTid()
{
	if (!tid)
		CacheTid();
	return tid;
}

const char *GetTidString()
{
	if (!tid)
		CacheTid();
	return tid_str;
}