#include "XFtpFactory.h"
#include "XFtpServerCMD.h"
XTask* XFtpFactory::CreateTask()
{
	XFtpServerCMD* x = new XFtpServerCMD();

	//ע��ftp��Ϣ�������


	return x;
}

XFtpFactory::XFtpFactory()
{
}
