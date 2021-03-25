#pragma once
#include "XTask.h"
#include "XFtpTask.h"
#include <event2/buffer.h>
#include <iostream>
class XFtpTask : public XTask
{
public:

	std::string curDir = "/";
	std::string rootDir = ".";

	//PORT ����ͨ����IP�Ͷ˿�
	std::string ip = "";
	int port = 0;

	//����ͨ��
	XFtpTask* cmdTask = 0;

	//����Э��
	virtual void Parse(std::string type, std::string msg) {}

	//�ظ�cmd��Ϣ
	void ResCMD(std::string msg);

	//��������ͨ��
	void ConnectPORT();

	//�������ͽ��������ӵ�����ͨ��
	void Send(std::string data);
	void Send(const char* data, int datasize);

	// 
	void Close();

	virtual void Read(struct bufferevent* bev) {}
	virtual void Write(struct bufferevent* bev) {}
	virtual void Event(struct bufferevent* bev, short what) {}
	void SetCallback(struct bufferevent* bev);
	bool Init() { return true; }

protected: // �̳��߿��Է���
	static void ReadCB(bufferevent* bev, void* arg);
	static void WriteCB(bufferevent* bev, void* arg);
	static void EventCB(struct bufferevent* bev, short what, void* arg);

	// ����bev ÿ��ָ�����һ��bev
	struct bufferevent* bev = 0;

};

