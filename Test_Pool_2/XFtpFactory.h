#pragma once
#include "XTask.h"
class XFtpFactory
{
public:
	//单件模式
	static XFtpFactory* Get()
	{
		static XFtpFactory f;
		return &f;
	}
	XTask* CreateTask();
private:
	XFtpFactory();
};

