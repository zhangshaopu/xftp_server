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

	//BEV_OPT_CLOSE_ON_FREE表示当调用bufferevent_free()，相应的 socket 描述符也会被close()掉。
	//如果 socket 可读，bufferevent会自动读取 socket 中的数据，并放到输入缓冲区中。
	//如果 socket 可写，bufferevent会自动将输出缓冲区中的数据写到 socket 中。
	bufferevent* bev = bufferevent_socket_new(base, sock, BEV_OPT_CLOSE_ON_FREE);
	this->bev = bev;

	this->SetCallback(bev);

	//添加超时
	timeval rt = { 60,0 }; // 秒和微秒
	bufferevent_set_timeouts(bev, &rt, 0);

	string msg = "220 Welcome to libevent XFtpServer\r\n";
	bufferevent_write(bev, msg.c_str(), msg.size());

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
		//分析出类型 USER anonymous
		string type = "";
		for (int i = 0; i < len; i++)
		{
			if (data[i] == ' ' || data[i] == '\r')
				break;
			type += data[i];
		}
		cout << "type is [" << type << "]" << endl;
		if (calls.find(type) != calls.end())
		{
			XFtpTask* t = calls[type];
			t->cmdTask = this; //用来处理回复命令和目录

			t->ip = ip;
			t->port = port;
			t->base = base;

			t->Parse(type, data);

			if (type == "PORT")
			{
				ip = t->ip;
				port = t->port;
			}
		}
		else
		{
			string msg = "200 OK\r\n";
			bufferevent_write(bev, msg.c_str(), msg.size());
		}

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

void XFtpServerCMD::Reg(std::string cmd, XFtpTask* call)
{
	if (!call)
	{
		cout << "XFtpServerCMD::Reg call is null " << endl;
		return;
	}
	if (cmd.empty())
	{
		cout << "XFtpServerCMD::Reg cmd is null " << endl;
		return;
	}
	//已经注册的是否覆盖 不覆盖，提示错误
	if (calls.find(cmd) != calls.end())
	{
		cout << cmd << " is alreay register" << endl;
		return;
	}
	calls[cmd] = call;
}
