/******************* */
//此类实现http的请求和回应
/********************* */

#ifndef HTTPSERVEREVENTHANDLER__
#define HTTPSERVEREVENTHANDLER__
#include <string>
#include <map>
#include <memory>
#include <pthread.h>
#include "EventHandler.h"
#include "Timer.h"

enum HttpState
{
    STATE_WAIT_REQUEST,
    STATE_PARSE_REQUEST,
    STATE_ANALYSIS_REQUEST,
    //STATE_SEND_RESPONSE
};
enum ParseState
{
    STATE_PARSE_REQUEST_LINE,
    STATE_PARSE_HEADER,
    STATE_PARSE_BODY
};
enum ParseRequestLineState
{
    STATE_PARSE_Request_SUCCESS,
    STATE_PARSE_ERROR,
    STATE_UNSUPPORTED_METHOD,
    STATE_UNSUPPORTED_URL,
    STATE_UNSUPPORTED_HTTP_VERSION
};

enum ParseHeaderState
{
    STATE_PARSE_Header_SUCCESS,
    STATE_PARSE_KEY_ERROR,
    STATE_PARSE_VALUE_ERROR
};
enum Method
{
    METHOD_GET,
    METHOD_HEAD,
    METHOD_POST,
    METHOD_PUT,
    METHOD_DELETE
};
enum HttpVersion
{
    HTTP_10,
    HTTP_11
};
enum ParseBodyState
{
    STATE_PARSE_BODY_ERROR,
    STATE_PARSE_BODY_SUCCESS
};
enum AnalysisState
{
    STATE_ANALYSIS_ERROR,
    STATE_ANALYSIS_SUCCESS
};
class FileType:public NonCopy
{
private:
    
    static std::map<std::string,std::string> type_;
    static pthread_once_t once_control_;
    FileType() {}
    static void Init()
    {
        type_[".html"] = "text/html";
        type_[".htm"] = "text/html";
        type_["default"] = "text/html";
    }
public:
    static std::string GetType(const std::string s)
    {
        pthread_once(&once_control_,FileType::Init);
        if(type_.find(s) == type_.end())
            return type_["default"];
        else
            return type_[s];
    }
};
//class Epoll;
class HttpServerEventHandler:public EventHandler
{
private:
    static const int kLongLinkTime; //单位为秒
    std::string in_buff_;
    std::string out_buff_;
    HttpState http_state_;
    ParseState parse_state_;
    ParseRequestLineState parse_request_line_state_;
    ParseHeaderState parse_header_state_;
    ParseBodyState parse_body_state_;
    Method method_;
    std::string file_name_;
    HttpVersion http_version_;
    std::map<std::string,std::string> header_;
    AnalysisState analysis_state_;
    typedef std::shared_ptr<Timer> sptr;
    sptr timer_;
    bool have_body_;
    bool is_close_;
    bool keep_alive_;
    void ParseRequestLine();
    void ParseHeader();
    void ParseBody();
    void AnalysisRequest();
    void Init();
    void HttpError();
public:
    //typedef std::weak_ptr<EventLoop> wptr;
    //typedef std::shared_ptr<EventLoop> sptr;
    HttpServerEventHandler(int fd,EventLoop_WPtr event_loop);
    virtual void WriteHandle() override;
    virtual void ReadHandle() override;
    virtual void ErrorHandle() override;
    virtual void CloseHandle() override;
    void TimeHandler();
};
#endif