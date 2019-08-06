# SimpleWebServer
a simple WebServer for study

参考muduo网络库以及https://github.com/linyacool/WebServer.git <br>

目前完成功能：1.基于Reactor模式的多线程epoll服务端模型 <br>
             2.基于小根堆的定时任务处理 <br>
             3.http请求解析与响应（只支持Get） <br>
             4.http短连接和长连接选择 <br>
   ![image](https://github.com/shifangyan/SimpleWebServer/raw/master/短连接测试.png)
             
接下来会参考muduo网络库进行改进 <br>
接下来任务：1.日志输出 <br>
           2.性能对比以及调整 <br>
             
