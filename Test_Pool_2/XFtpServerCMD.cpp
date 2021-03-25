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

	//����Է�����ϵ������߻��������п����ղ���BEV_EVENT_EOF����
	if (what & (BEV_EVENT_EOF | BEV_EVENT_ERROR | BEV_EVENT_TIMEOUT))
	{
		cout << "BEV_EVENT_EOF | BEV_EVENT_ERROR |BEV_EVENT_TIMEOUT" << endl;
		bufferevent_free(bev);
		delete cmd;
	}
}

//���߳�XThread  event�¼��ַ�
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

		//���Դ��룬Ҫ�����
		if (strstr(data, "quit")) // ��⵽quit ����bufev ɾ��XFTpServerCMD ����
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
	//����socket bufferevent
	// base socket

	//BEV_OPT_CLOSE_ON_FREE��ʾ������bufferevent_free()����Ӧ�� socket ������Ҳ�ᱻclose()����
	//��� socket �ɶ���bufferevent���Զ���ȡ socket �е����ݣ����ŵ����뻺�����С�
	//��� socket ��д��bufferevent���Զ�������������е�����д�� socket �С�
	bufferevent* bev = bufferevent_socket_new(base, sock, BEV_OPT_CLOSE_ON_FREE);
	this->bev = bev;

	this->SetCallback(bev);

	//��ӳ�ʱ
	timeval rt = { 60,0 }; // ���΢��
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
		//�ַ����������
		//���������� USER anonymous
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
			t->cmdTask = this; //��������ظ������Ŀ¼

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
	//����Է�����ϵ������߻��������п����ղ���BEV_EVENT_EOF����
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
	//�Ѿ�ע����Ƿ񸲸� �����ǣ���ʾ����
	if (calls.find(cmd) != calls.end())
	{
		cout << cmd << " is alreay register" << endl;
		return;
	}
	calls[cmd] = call;
}
