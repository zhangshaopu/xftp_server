#include "XFtpFactory.h"
#include "XFtpServerCMD.h"
XTask* XFtpFactory::CreateTask()
{
	XFtpServerCMD* x = new XFtpServerCMD();

	//注册ftp消息处理对象


	return x;
}

XFtpFactory::XFtpFactory()
{
}
