#ifndef UTIL__
#define UTIL__
#include <string>
typedef unsigned long size_t;
//extern const int SERVER_PORT;
//extern const int LISTENQ;
int Readn(int fd,void *buff,size_t n);
int Readn(int fd,std::string &str);
int Writen(int fd,std::string &str);
int Writen(int fd,void *buff,size_t n);
int socket_bind_listen(int port,bool NonBlocking = true);
void SetNonBlocking(int fd);
void SetNoDelay(int fd);

#endif