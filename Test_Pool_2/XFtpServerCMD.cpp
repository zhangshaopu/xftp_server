#include "XFtpServerCMD.h"
#include <iostream>
#include <event2/bufferevent.h>
#include <event2/event.h>
#include <string.h>

using namespace std;


/*
void EventCB(struct bufferevent* bev, short what, void* arg)
{
	XFtpServerCMD* cmd = (XFtpServerCMD*)arg;

	//如果对方网络断掉，或者机器死机有可能收不到BEV_EVENT_EOF数据
	if (what & (BEV_EVENT_EOF | BEV_EVENT_ERROR | BEV_EVENT_TIMEOUT))
	{
		cout << "BEV_EVENT_EOF | BEV_EVENT_ERROR |BEV_EVENT_TIMEOUT" << endl;
		bufferevent_free(bev);
		delete cmd;
	}
}

//子线程XThread  event事件分发
static void ReadCB(bufferevent* bev, void* arg)
{
	XFtpServerCMD* cmd = (XFtpServerCMD*)arg;
	char data[1024] = { 0 };
	for (;;)
	{
		int len = bufferevent_read(bev, data, sizeof(data) - 1);
		if (len <= 0) break;
		data[len] = '\0';
		cout << data << flush;

		//测试代码，要清理掉
		if (strstr(data, "quit")) // 检测到quit 清理bufev 删除XFTpServerCMD 对象
		{
			bufferevent_free(bev);
			delete cmd;
			break;
		}
	}
}
*/
bool XFtpServerCMD::Init()
{
	cout << "XFtpServerCMD::Init()" << endl;
	//监听socket bufferevent
	// base socket

	bufferevent* bev = bufferevent_socket_new(base, sock, BEV_OPT_CLOSE_ON_FREE);
	//bufferevent_setcb(bev, ReadCB, 0, EventCB, this);
	//bufferevent_enable(bev, EV_READ | EV_WRITE);
	this->SetCallback(bev);

	//添加超时
	timeval rt = { 60,0 }; // 秒和微秒
	bufferevent_set_timeouts(bev, &rt, 0);
	return true; 

}

void XFtpServerCMD::Read(bufferevent* bev)
{
	char data[1024] = { 0 };
	for (;;)
	{
		int len = bufferevent_read(bev, data, sizeof(data) - 1);
		if (len <= 0) break;
		data[len] = '\0';
		cout << "Recv CMD:" << data << flush;
		//分发到处理对象

	}
}

void XFtpServerCMD::Event(bufferevent* bev, short what)
{
	//如果对方网络断掉，或者机器死机有可能收不到BEV_EVENT_EOF数据
	if (what & (BEV_EVENT_EOF | BEV_EVENT_ERROR | BEV_EVENT_TIMEOUT))
	{
		cout << "BEV_EVENT_EOF | BEV_EVENT_ERROR |BEV_EVENT_TIMEOUT" << endl;
		bufferevent_free(bev);
		delete this;
	}
}
