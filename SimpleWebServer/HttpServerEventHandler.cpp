#include <sys/mman.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <string.h>
#include "HttpServerEventHandler.h"
#include "Util.h"
#include "EventLoop.h"
using namespace std;

const int HttpServerEventHandler::kLongLinkTime = 60;
pthread_once_t FileType::once_control_ = PTHREAD_ONCE_INIT;
std::map<std::string, std::string> FileType::type_;
HttpServerEventHandler::HttpServerEventHandler(int fd,EventLoop_WPtr event_loop):EventHandler(fd,event_loop),in_buff_(),out_buff_(),is_close_(false)
{
    SetNonBlocking(fd);
    SetNoDelay(fd);
    Init();
}

void HttpServerEventHandler::Init()
{
    http_state_ = STATE_WAIT_REQUEST;
    parse_state_ = STATE_PARSE_REQUEST_LINE;
    parse_request_line_state_ = STATE_PARSE_Request_SUCCESS;
    parse_header_state_ = STATE_PARSE_Header_SUCCESS;
    parse_body_state_ = STATE_PARSE_BODY_SUCCESS;
    method_ = METHOD_GET;
    file_name_.clear();
    http_version_ = HTTP_10;
    header_.clear();
    analysis_state_ = STATE_ANALYSIS_SUCCESS;
    have_body_ = false;
    //keep_alive_ = false;
}
void HttpServerEventHandler::WriteHandle()
{
 //   printf("http write\n");
    
 //   printf("out_buffe size %d\n",out_buff_.size());
//    if(http_state_ != STATE_SEND_RESPONSE)
//    {
//        printf("error:http state error:%d\n",http_state_);
//        ErrorHandle();
//        return
//    }
   // printf("go write\n");
  //  printf("%s\n",out_buff_.c_str());
   // printf("httpserver use_count:%d\n",timer_.use_count());
    int write_size;
    if((write_size = Writen(fd_,out_buff_)) <0)
    {    
        perror("writen error:");
        ErrorHandle();
    }
   else if(write_size < out_buff_.size())
   {
       out_buff_ = out_buff_.substr(write_size);
   }
   else
   {
     //  printf("%d:http write1\n",fd_);
        if(is_close_ || !keep_alive_)
        {
      //      printf("%d:http write2\n",fd_);
            auto event_loop = eventloop_.lock();
            if(event_loop)
            {
                auto sptr = shared_from_this();
          //      printf("%d:http write2_1\n",fd_);
                event_loop->DelEventHandler(sptr);
           //     printf("%d:http write2_2\n",fd_);
            }
            
        }
        else
        {
        //    printf("http write3\n");
            out_buff_.clear();
            DisableWrite();
        }
   }
 //  printf("%d:http write4\n",fd_);
  // printf("httpserver use_count:%d\n",timer_.use_count());
}

void HttpServerEventHandler::ReadHandle()
{
 //   printf("go read\n");
    if(http_state_ != STATE_WAIT_REQUEST)
    {
        printf("error state1:\n");
        HttpError();
        return;
    }
    int read_num = Readn(fd_,in_buff_);
    if(read_num <0)
    {
        printf("%d\n",read_num);
        perror("http read error:");
        ErrorHandle();
        return;
    }
    else if(read_num == 0)
    {
   //     printf("%d:read 0\n",fd_);
   //     printf("in_buff_:%s\n",in_buff_.c_str());
        if(out_buff_.size() == 0)
        {
    //        printf("%d:read 0=1\n",fd_);
            CloseHandle();
        }
        else
            is_close_ = true;   
        return;
    }
   // printf("go read1\n");
   // printf("in_buff:%s\n",in_buff_.c_str());
    //printf("%s\n",in_buff_.c_str());
    //解析请求行
    http_state_ = STATE_PARSE_REQUEST;
    if(parse_state_ != STATE_PARSE_REQUEST_LINE)
    {
        printf("error state2:\n");
       // std::cout << parse_state_ << std::endl;
        HttpError();
        return;
    }
    
    ParseRequestLine();
    if(parse_request_line_state_ != STATE_PARSE_Request_SUCCESS)
    {
        printf("parse request line error:%d\n",parse_request_line_state_);
        HttpError();
        return;
    }
    //解析首部
    parse_state_ = STATE_PARSE_HEADER;
    ParseHeader();
    if(parse_header_state_ != STATE_PARSE_Header_SUCCESS)
    {
        printf("parse header error:%d\n",parse_header_state_);
        HttpError();
        return;
    }

    if(have_body_)
    {
        parse_state_ = STATE_PARSE_BODY;
        //printf("error:unsupport request\n");
        ParseBody();
        if(parse_body_state_ != STATE_PARSE_BODY_SUCCESS)
        {
            printf("parse body error:\n");
            HttpError();
            return;
        }
    }
    http_state_ = STATE_ANALYSIS_REQUEST;
    AnalysisRequest();
    if(analysis_state_ != STATE_ANALYSIS_SUCCESS)
    {
        printf("analysis request error:\n");
        HttpError();
        return;
    }
    //in_buff_.clear();
    Init();
  //  printf("in_buff:%s\n",in_buff_.c_str());
   // SetEvents(GetEvents() | EPOLLOUT|EPOLLET);
   //timer_ = new Timer(3000,false,std::bind(&HttpServerEventHandler::TimeHandler,this),) 
   // printf("read end\n");
    return;
}

void HttpServerEventHandler::ErrorHandle()
{
    perror("Http connect socket error:");
   // epoll_ptr_->DelEventHandler(fd_);
    auto event_loop = eventloop_.lock();
    if(event_loop)
    {
        auto sptr = shared_from_this();
        event_loop->DelEventHandler(sptr);
    }
}

void HttpServerEventHandler::CloseHandle()
{
    auto sptr = eventloop_.lock();
    if(sptr)
    {
      //  printf("close event1\n");
        auto sptr_this = shared_from_this();
        sptr->DelEventHandler(sptr_this);
    }
}
void HttpServerEventHandler::HttpError()
{
    char send_buff[4096];
    string short_msg = " 404 Not found!";
    string body_buff, header_buff;
    body_buff += "<html><title>哎~出错了</title>";
    body_buff += "<body bgcolor=\"ffffff\">";
    body_buff += short_msg;
    body_buff += "<hr><em> Web Server</em>\n</body></html>";

    header_buff += "HTTP/1.1" +short_msg + "\r\n";
    header_buff += "Content-Type: text/html\r\n";
    header_buff += "Connection: Close\r\n";
    header_buff += "Content-Length: " + std::to_string(body_buff.size()) + "\r\n";
    header_buff += "Server: LinYa's Web Server\r\n";;
    header_buff += "\r\n";
    // 错误处理不考虑writen不完的情况
    sprintf(send_buff, "%s", header_buff.c_str());
    Writen(fd_, send_buff, strlen(send_buff));
    sprintf(send_buff, "%s", body_buff.c_str());
    Writen(fd_, send_buff, strlen(send_buff));
    Init();
}

//解析请求行
void HttpServerEventHandler::ParseRequestLine()
{
    //取出第一行作为请求行
    auto pos = in_buff_.find('\n');
    if(pos == string::npos)
    {
        parse_request_line_state_ = STATE_PARSE_ERROR;
        return;
    }
    string request_line = in_buff_.substr(0,pos+1);
    in_buff_ = in_buff_.substr(pos+1);

    //method
    auto pos_get = request_line.find("GET");
    auto pos_head = request_line.find("HEADER");

    if(pos_get != string::npos)
    {
        method_ = METHOD_GET;
        pos = pos_get;
    }
    else if(pos_head != string::npos)
    {
        method_ = METHOD_HEAD;
        pos = pos_head;
    }
    else
    {
        parse_request_line_state_ = STATE_UNSUPPORTED_METHOD;
        return;
    }
    
    //request file
    pos = request_line.find('/',pos);
    if(pos == string::npos)
    {
        parse_request_line_state_ = STATE_UNSUPPORTED_URL;
        return;
    }
    else
    {
        auto _pos = request_line.find(' ',pos);
        if(pos == string::npos) 
        {
            parse_request_line_state_ = STATE_UNSUPPORTED_URL;
            return;
        }
        else
        {
            file_name_ = request_line.substr(pos+1,_pos-pos-1);
            auto __pos = file_name_.find('?');
            if(pos != string::npos)
            {
                file_name_ = file_name_.substr(0,__pos);
            }
        }
        pos = _pos;
    }

    //HTTP version
    pos = request_line.find('/',pos);
    if(pos == string::npos)
    {
        parse_request_line_state_ = STATE_UNSUPPORTED_HTTP_VERSION;
        return;
    }
    else
    {
        if(request_line.size()-pos <6)
        {
            parse_request_line_state_ = STATE_UNSUPPORTED_HTTP_VERSION;
            return;
        }
        else
        {
            string version = request_line.substr(pos+1,3);
            if(version == "1.0")
                http_version_ = HTTP_10;
            else if(version == "1.1")
                http_version_ = HTTP_11;
            else
            {
                parse_request_line_state_ = STATE_UNSUPPORTED_HTTP_VERSION;
                return;
            }
        } 
    }
    parse_request_line_state_ = STATE_PARSE_Request_SUCCESS;
}

void HttpServerEventHandler::ParseHeader()
{
    string::size_type key_start = 0,key_end = 0,value_start = 0,value_end = 0;
    //bool finish = false;
    //int pos = 0;
    while(1)
    {
        //key_start = value_end +2;
        key_end = in_buff_.find(':',key_start);
        if(key_end == string::npos)
        {
            if(in_buff_[key_start] == '\r' && in_buff_[key_start+1] == '\n')
            {
                in_buff_ = in_buff_.substr(key_start+2);
                if(in_buff_.size()>0)
                {
                    have_body_ = true;
                }
                parse_header_state_ = STATE_PARSE_Header_SUCCESS;
            }
            else
            {
                parse_header_state_ = STATE_PARSE_KEY_ERROR;
            }
            return;
            // string::size_type header_end;
            // if((header_end = in_buff_.find('\n',key_start)) != string::npos)
            // {
            //     parse_header_state_ = STATE_PARSE_Header_SUCCESS;
            //     if(in_buff_.size()-header_end >2)
            //     {
            //         have_body_ = true;
            //         in_buff_ = in_buff_.substr(header_end+2);
            //     }
            // }
            // else
            //     parse_header_state_ = STATE_PARSE_KEY_ERROR;
            // break;
        }
        else
        {
            value_start = key_end +2;
            value_end = in_buff_.find('\r',value_start);
            if(value_end == string::npos)
            {
                parse_header_state_ = STATE_PARSE_VALUE_ERROR;
                break;
            }
            else
            {
                string key(in_buff_.begin()+key_start,in_buff_.begin()+key_end);
                if(key.size() == 0)
                {
                    parse_header_state_ = STATE_PARSE_KEY_ERROR;
                    break;
                }
                string value(in_buff_.begin()+value_start,in_buff_.begin()+value_end);
                if(value.size() == 0)
                {
                    parse_header_state_ = STATE_PARSE_VALUE_ERROR;
                    break;
                }
                header_[key] = value;
                key_start = value_end +2;
            }
        }
    }
}

//暂时不接受主体 直接将主体丢弃
void HttpServerEventHandler::ParseBody()
{
    int content_length = -1;
    if(header_.find("Content-length") != header_.end())
    {
        content_length = std::stoi(header_["content-length"]);
    }
    else
    {
        parse_body_state_ = STATE_PARSE_BODY_ERROR;
        return;
    }
    if(content_length == in_buff_.size())
        in_buff_.clear();
    else
        in_buff_ = in_buff_.substr(content_length);
    parse_body_state_ = STATE_PARSE_BODY_SUCCESS;
}

void HttpServerEventHandler::AnalysisRequest()
{
    if(method_ == METHOD_GET || method_ == METHOD_HEAD)
    {
        string header;
        header += "HTTP/1.1 200 ok\r\n";
        if(header_.find("Connection") != header_.end())
        {
            header += string("Connection: ") + to_string(kLongLinkTime) 
            +string("\r\n");
            //header += string("Connection: Close\r\n"); //暂时不接受长连接
            keep_alive_ = true;
            auto eventloop = eventloop_.lock();
            if(eventloop)
            {
                timer_ = std::shared_ptr<Timer>(new Timer(kLongLinkTime*1000,false,std::bind(&HttpServerEventHandler::TimeHandler,this),
                eventloop->GetTimerManager()));
                eventloop->AddTimer(timer_);
            }
         //   printf("httpserver use_count:%d\n",timer_.use_count());
        }
        else
        {
            header += string("Connection: Close\r\n");
            keep_alive_ = false;
        }
        
        auto dot_pos = file_name_.find('.');
        string file_type;
        if(dot_pos == string::npos)
            file_type = FileType::GetType("default");
        else
            file_type = FileType::GetType(file_name_.substr(dot_pos));

        if(file_name_ == "hello")
        {
            //printf("send hello\n");
            string content = "Hello World";

            out_buff_ = string("HTTP/1.1 200 OK\r\nContent-type: text/plain\r\n")
            +"Content-Length: " + std::to_string(content.size()) +"\r\n"
            +"\r\n" +content; 
            EnableWrite();
            analysis_state_ = STATE_ANALYSIS_SUCCESS;
            return; 
        }
        else
        {
            struct stat sbuf;
            if (stat(file_name_.c_str(), &sbuf) < 0)
            {
                analysis_state_ = STATE_ANALYSIS_ERROR;
                return;
            }
            header += "Content-Type: " + file_type + "\r\n";
            header += "Content-Length: " + std::to_string(sbuf.st_size) + "\r\n";
            header += "Server: Shifangyan's Web Server\r\n";
            header += "\r\n";
            out_buff_ = header;

            if(method_ == METHOD_HEAD)
            {
                analysis_state_ = STATE_ANALYSIS_SUCCESS;
                return; 
            }

            int send_file_fd = open(file_name_.c_str(),O_RDONLY,0);
            if(send_file_fd < 0)
            {
                out_buff_.clear();
                analysis_state_ = STATE_ANALYSIS_ERROR;
                return;
            }
            void *mmapRet = mmap(NULL, sbuf.st_size, PROT_READ, MAP_PRIVATE, send_file_fd, 0);
            close(send_file_fd);
            if(mmapRet == (void *)-1)
            {
                munmap(mmapRet, sbuf.st_size);
                out_buff_.clear();
                analysis_state_ = STATE_ANALYSIS_ERROR;
                return;
            }
            char *src_addr = static_cast<char *>(mmapRet);
            out_buff_ += string(src_addr,src_addr+sbuf.st_size);
            munmap(mmapRet,sbuf.st_size);
            EnableWrite();
            analysis_state_ = STATE_ANALYSIS_SUCCESS;
            return; 
        }
        
    }   
}

void HttpServerEventHandler::TimeHandler()
{
   // printf("timer start\n");
    is_close_ = true;
    if(out_buff_.empty())
    {
  //      printf("close event\n");
        CloseHandle();
    } 
}