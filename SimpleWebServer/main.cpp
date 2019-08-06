//epoll版本echo server

#include <sys/types.h>
//#include "../writenReadn.h"
#include <unistd.h>
#include <errno.h>
//#include "Epoll.h"
//#include "ListenEventHandler.h"
//#include "Util.h"
#include "Server.h"
#include <iostream>
const int SERVER_PORT = 9000;

int main(int argc,char *argv[])
{
	Server server(SERVER_PORT,4); //端口 线程数量
	server.Run();
	printf("go there\n");
}

