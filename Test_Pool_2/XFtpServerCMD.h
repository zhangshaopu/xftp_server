#pragma once
#include "XFtpTask.h"
#include <map>
class XFtpServerCMD :public XFtpTask
{
public:
    //��ʼ������ �麯��
    virtual bool Init();

    virtual void Read(struct bufferevent* bev);
    virtual void Event(struct bufferevent* bev, short what);

    //ע���������� ����Ҫ�����̰߳�ȫ������ʱ��δ�ַ����߳�
    void Reg(std::string, XFtpTask* call);


private:
    std::map<std::string, XFtpTask*> calls;

};

