#include "XThread.h"
#include <thread>
#include <iostream>
#include <event2/event.h>
#include <unistd.h>
#include "XTask.h"
using namespace std;


//激活线程任务的回调函数
static void NotifyCB(evutil_socket_t fd, short which, void* arg)
{
	XThread* t = (XThread*)arg;
	t->Notify(fd, which);
}

void XThread::Notify(evutil_socket_t fd, short which)
{
	//水平触发 只要没有接受完成，会再次进来
	char buf[2] = { 0 };
#ifdef _WIN32
	int re = recv(fd, buf, 1, 0);
#else
	//linux中是管道，不能用recv
	//从fd[0]中读一个字节到buf
	int re = read(fd, buf, 1);
#endif
	if (re <= 0)
		return;
	cout << id << " thread " << buf << endl;

	XTask* task = NULL;

	//如果该线程的任务队列不为空，获取任务，并初始化任务
	tasks_mutex.lock();
	if (tasks.empty())
	{
		tasks_mutex.unlock();
		return;
	}
	task = tasks.front(); //先进先出
	tasks.pop_front();
	tasks_mutex.unlock();
	task->Init();

}

void XThread::Activate()
{
#ifdef _WIN32
	int re = send(this->notify_send_fd, "c", 1, 0);
#else
	// 往fd[0]写入1个字符 'c'，表示接受到任务
	int re = write(this->notify_send_fd, "c", 1);
#endif
	if (re <= 0)
	{
		cerr << "XThread::Activate() failed!" << endl;
	}
}

void XThread::AddTask(XTask *t)
{
	cout << " add task to this thread's task_list " << endl;
	if (!t)return;
	t->base = this->base;
	//任务队列互斥访问
	tasks_mutex.lock();
	tasks.push_back(t);
	tasks_mutex.unlock();
}

XThread::XThread()
{

}

void XThread::Start()
{
	Setup(); 
	//启动线程
	thread th(&XThread::Main, this);

	//断开与主线程联系
	th.detach();
}
//线程入口函数
void XThread::Main()
{
	std::cout << id << " XThread::Main() begin" << std::endl;
	event_base_dispatch(base);
	event_base_free(base);
	
	std::cout << id << " XThread::Main() end" << std::endl;
}

bool XThread::Setup()
{
	//创建的管道 不能用send recv读取 read write
	int fds[2];
	if (pipe(fds))
	{
		cerr << "pipe failed!" << endl;
		return false;
	}
	//读取绑定到event事件中，写入要保存
	notify_send_fd = fds[1];

	//创建libevent上下文（无锁）
	event_config* ev_conf = event_config_new();
	event_config_set_flag(ev_conf, EVENT_BASE_FLAG_NOLOCK);

	// 使用上下文创建base
	this->base = event_base_new_with_config(ev_conf);

	event_config_free(ev_conf);
	if (!this->base)
	{
		cerr << "event_base_new_with_config failed in thread!" << endl;
		return false;
	}

	//添加管道监听事件，用于激活线程执行任务
	// 只要fd[0]接受到消息 调用NotifyCB
	event* ev = event_new(this->base, fds[0], EV_READ | EV_PERSIST, NotifyCB, this);
	
	event_add(ev, 0);

	return true;
}
