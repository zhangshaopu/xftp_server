#pragma once
#include "XFtpTask.h"
class XFtpLIST :
    public XFtpTask
{
    //����Э��
    virtual void Parse(std::string type, std::string msg);
    virtual void Write(struct bufferevent* bev);
    virtual void Event(struct bufferevent* bev, short what);

};
