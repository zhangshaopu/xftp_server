#pragma once
#include "XFtpTask.h"
class XFtpServerCMD :public XFtpTask
{
public:
    //��ʼ������ �麯��
    virtual bool Init();

    virtual void Read(struct bufferevent* bev);
    virtual void Event(struct bufferevent* bev, short what);

};

