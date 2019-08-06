#include <sys/syscall.h> //SYS_gettid
#include <unistd.h> //syscall

__thread int tid = 0;
//__thread char tid_str[32];

int Gettid()
{
    if(!tid)
    {
        tid = syscall(SYS_gettid);
  //      sprintf()
    }
    return tid;
}


