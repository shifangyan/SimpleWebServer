#ifndef UTIL__
#define UTIL__
#include <string>
//typedef unsigned long size_t;
//extern const int SERVER_PORT;
//extern const int LISTENQ;
size_t Readn(int fd,void *buff,size_t n);
size_t Readn(int fd,std::string &str);
size_t Writen(int fd,std::string &str);
size_t Writen(int fd,void *buff,size_t n);
int socket_bind_listen(unsigned short port,bool NonBlocking = true);
void SetNonBlocking(int fd);
void SetNoDelay(int fd);

#endif