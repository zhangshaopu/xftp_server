#include "XFtpLIST.h"
#include <iostream>
#include <event2/event.h>
#include <event2/bufferevent.h>
using namespace std;
void XFtpLIST::Parse(std::string type, std::string msg)
{
	string resmsg = "";
	if (type == "PWD")
	{
		//257 "/" is current directory.
		resmsg = "257 \"";
		resmsg += cmdTask->curDir;
		resmsg += "\" is current dir.\r\n";

		ResCMD(resmsg);
	}
	else if (type == "LIST")
	{
		//1.��������ͨ��  2.150��Ӧ  3.����Ŀ¼����ͨ�� 4.�������226  5.�ر�����
		//����ͨ���ظ���Ϣ ʹ������ͨ������Ŀ¼
		//-rwxrwxrwx 1 root group 64463 Mar 14 09:53 101.jpg\r\n
		//1. ��������ͨ�� 
		ConnectPORT();

		//2. 1502 150��Ӧ
		ResCMD("150 Here comes the directory listing.\r\n");
		string listdata = "-rwxrwxrwx 1 root group 64463 Mar 14 09:53 101.jpg\r\n";

		//3. ����ͨ������
		Send(listdata);
	}
}

void XFtpLIST::Write(bufferevent* bev)
{
	//4 226 Transfer complete�������
	ResCMD("226 Transfer complete\r\n");
	//5 �ر�����
	Close();
}

void XFtpLIST::Event(bufferevent* bev, short what)
{
	//����Է�����ϵ������߻��������п����ղ���BEV_EVENT_EOF����
	if (what & (BEV_EVENT_EOF | BEV_EVENT_ERROR | BEV_EVENT_TIMEOUT))
	{
		cout << "BEV_EVENT_EOF | BEV_EVENT_ERROR |BEV_EVENT_TIMEOUT" << endl;
		//Close();
	}
	else if (what & BEV_EVENT_CONNECTED)
	{
		cout << "XFtpLIST BEV_EVENT_CONNECTED success" << endl;
	}
}
