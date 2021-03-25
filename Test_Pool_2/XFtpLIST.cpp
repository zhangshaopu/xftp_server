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
		//1.连接数据通道  2.150回应  3.发送目录数据通道 4.发送完成226  5.关闭连接
		//命令通道回复消息 使用数据通道发送目录
		//-rwxrwxrwx 1 root group 64463 Mar 14 09:53 101.jpg\r\n
		//1. 连接数据通道 
		ConnectPORT();

		//2. 1502 150回应
		ResCMD("150 Here comes the directory listing.\r\n");
		string listdata = "-rwxrwxrwx 1 root group 64463 Mar 14 09:53 101.jpg\r\n";

		//3. 数据通道发送
		Send(listdata);
	}
}

void XFtpLIST::Write(bufferevent* bev)
{
	//4 226 Transfer complete发送完成
	ResCMD("226 Transfer complete\r\n");
	//5 关闭连接
	Close();
}

void XFtpLIST::Event(bufferevent* bev, short what)
{
	//如果对方网络断掉，或者机器死机有可能收不到BEV_EVENT_EOF数据
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
