
//#include <sys/types.h>
//#include <unistd.h>
//#include <errno.h>
#include "HttpServer.h"
//#include <iostream>
#include "logger/Logger.h"

const int SERVER_PORT = 9000;

int main(int argc,char *argv[])
{
	HttpServer server(SERVER_PORT, 4, "log_test", 3); //�˿� �߳�����
	server.Run();
}

