
//#include <sys/types.h>
//#include <unistd.h>
//#include <errno.h>
#include "HttpServer.h"
//#include <iostream>
#include "Logger.h"

const int SERVER_PORT = 9000;

int main(int argc,char *argv[])
{
	LOG_DEBUG << "start main";
	HttpServer server(SERVER_PORT, 4, "log_test", 3); //端口 线程数量
	server.Run();
}

