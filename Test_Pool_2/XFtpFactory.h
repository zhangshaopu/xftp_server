#pragma once
#include "XTask.h"
class XFtpFactory
{
public:
	//����ģʽ
	static XFtpFactory* Get()
	{
		static XFtpFactory f;
		return &f;
	}
	XTask* CreateTask();
private:
	XFtpFactory();
};

