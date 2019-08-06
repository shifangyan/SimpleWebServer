#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <string.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <errno.h>

#include "Util.h"

const int LISTENQ = 1000;
int Readn(int fd,void *buff,size_t n)
{
  //  printf("go there Readn start\n");
    size_t nleft = n;
    int nread = 0;
    size_t read_sum = 0;
    char *ptr = (char *)buff;
    while(nleft >0)
    {
        if((nread = read(fd,ptr,nleft)) < 0)
        {
          //  printf("%d\n",nread);
            if(errno == EINTR) //被系统中断打断
            {
               // printf("??????\n");
                nread = 0; //继续读
            }
            else if(errno == EAGAIN) //套接字缓存区被读空
            {
               // printf("/////////\n");
                return read_sum; //直接返回
            }
            else //未知情况 视为出错
            {
              //  printf("!!!!!!!!\n");
                return -1;
            }
        }
        else if(nread == 0) //对端关闭 不处理
        {
         //   printf("hhhhhh\n");
            break;
        }
      //  printf("%d\n",nread);
        nleft -= nread;
        read_sum += nread;
        ptr += nread;
      //  printf("%d\n",nread);
    }
   // printf("go there Readn end\n");
    return read_sum;
}

int Readn(int fd,std::string &str)
{
    int nread = 0;
    int read_sum = 0;
    while(1)
    {
    //    printf("readn\n");
        char buff[1000];
        if((nread = read(fd,buff,1000)) < 0)
        {
            if(errno == EINTR) //被系统中断打断
            {
               // printf("??????\n");
                nread = 0; //继续读
            }
            else if(errno == EAGAIN) //套接字缓存区被读空
            {
               // printf("/////////\n");
                return read_sum; //直接返回
            }
            else //未知情况 视为出错
            {
              //  printf("!!!!!!!!\n");
                return -1;
            }
        }
        else if(nread == 0) //对端关闭 不处理
        {    
         //   printf("readn1\n");
            return read_sum; //注意如果读到EOF，会返回0，且下次读还会返回0，不会出现EAGAIN
        }
        read_sum += nread;
        str += std::string(buff,buff+nread);
    }
}

int Writen(int fd,void *buff,size_t n)
{
   // printf("go there Write start\n");
    size_t nleft = n;
    int nwritten = 0;
    size_t written_sum = 0;
    char *ptr = static_cast<char *>(buff);
    while(nleft >0)
    {
        if((nwritten = write(fd,ptr,nleft)) <0)
        {
            if(errno == EINTR)//被系统中断打断
            {
                nwritten = 0; //继续写
            }
            else if(errno == EAGAIN) //缓存区被写满
            {
                return written_sum;
            }
            else //未知情况 视为出错
            {
                return -1;
            }
        }
        nleft -= nwritten;
        written_sum += nwritten;
        ptr += nwritten;
    }
   // printf("go there Write end\n");
    return written_sum;
}

int Writen(int fd,std::string &str)
{
   // printf("go there Write start\n");
    size_t nleft = str.size();
    int nwritten = 0;
    size_t written_sum = 0;
    const char *ptr = str.c_str();
    while(nleft >0)
    {
        if((nwritten = write(fd,ptr,nleft)) <0)
        {
            if(errno == EINTR)//被系统中断打断
            {
                nwritten = 0; //继续写
            }
            else if(errno == EAGAIN) //缓存区被写满
            {
                return written_sum;
            }
            else //未知情况 视为出错
            {
                return -1;
            }
        }
        nleft -= nwritten;
        written_sum += nwritten;
        ptr += nwritten;
    }
   // printf("go there Write end\n");
    return written_sum;
}

int socket_bind_listen(int port,bool NonBlocking)
{
  //  printf("go there socket_bind_listen start\n");
    int listenfd;
	if((listenfd = socket(AF_INET,SOCK_STREAM,IPPROTO_TCP)) < 0)
	{
		perror("socket error:");
		return -1;
	}
    struct sockaddr_in servaddr;
	memset(&servaddr,0,sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port = htons(port);
    int ret;
	if((ret = bind(listenfd,(struct sockaddr *)&servaddr,sizeof(sockaddr))) < 0)
	{
		perror("bind error:");
		return -1;
	}
    if((ret= listen(listenfd,LISTENQ)) < 0)
	{
		perror("listen error:");
		return -1;
	}
    if(NonBlocking)
    {
        SetNonBlocking(listenfd);
    }
   // printf("got there socket_bind_listeb end\n");
    return listenfd;
}

void SetNonBlocking(int fd)
{
    int val = fcntl(fd,F_GETFL,0);
    fcntl(fd,F_SETFL,val|O_NONBLOCK);
}

void SetNoDelay(int fd)
{
    int ret;
    int nodelay = 1;
    if((ret = setsockopt(fd,IPPROTO_TCP,TCP_NODELAY,(void *)(&nodelay),sizeof(nodelay))) <0)
    {
        perror("set no delay error:");
    }
}