#pragma once
class XTask // 接口类
{
public:
	struct event_base* base = 0;
	int sock = 0;
	int thread_id = 0;
	//初始化任务 虚函数
	virtual bool Init() = 0;
};

