#include "XFtpFactory.h"
#include "XFtpServerCMD.h"
#include "XFtpUSER.h"
#include "XFtpLIST.h"
#include "XFtpPORT.h"
#include <iostream>
using namespace std;
XTask* XFtpFactory::CreateTask()
{
	cout << " XFtfFactory produce a ftpServerCMD " << endl;
	XFtpServerCMD* x = new XFtpServerCMD();

	//注册ftp消息处理对象
	x->Reg("USER", new XFtpUSER());
	XFtpLIST* list = new XFtpLIST();
	x->Reg("PWD", list);
	x->Reg("LIST", list);
	x->Reg("PORT", new XFtpPORT());

	return x;
}

XFtpFactory::XFtpFactory()
{
}
