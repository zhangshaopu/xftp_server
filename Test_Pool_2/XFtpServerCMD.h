#pragma once
#include "XFtpTask.h"
#include <map>
// 用于接受客户端发送过来的指令
class XFtpServerCMD :public XFtpTask
{
public:
    //初始化任务 虚函数
    virtual bool Init();

    virtual void Read(struct bufferevent* bev);
    virtual void Event(struct bufferevent* bev, short what);

    //注册命令处理对象 不需要考虑线程安全，调用时还未分发到线程
    void Reg(std::string, XFtpTask* call);

    ~XFtpServerCMD();
private:
    // 注册的处理对象
    std::map<std::string, XFtpTask*> calls;
    // 用来做空间清理
    std::map< XFtpTask*, int > calls_dell;
    
};

