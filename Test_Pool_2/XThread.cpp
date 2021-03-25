#include "XThread.h"
#include <thread>
#include <iostream>
#include <event2/event.h>
#include <unistd.h>
#include "XTask.h"
using namespace std;


//�����߳�����Ļص�����
static void NotifyCB(evutil_socket_t fd, short which, void* arg)
{
	XThread* t = (XThread*)arg;
	t->Notify(fd, which);
}

void XThread::Notify(evutil_socket_t fd, short which)
{
	//ˮƽ���� ֻҪû�н�����ɣ����ٴν���
	char buf[2] = { 0 };
#ifdef _WIN32
	int re = recv(fd, buf, 1, 0);
#else
	//linux���ǹܵ���������recv
	//��fd[0]�ж�һ���ֽڵ�buf
	int re = read(fd, buf, 1);
#endif
	if (re <= 0)
		return;
	cout << id << " thread " << buf << endl;

	XTask* task = NULL;

	//������̵߳�������в�Ϊ�գ���ȡ���񣬲���ʼ������
	tasks_mutex.lock();
	if (tasks.empty())
	{
		tasks_mutex.unlock();
		return;
	}
	task = tasks.front(); //�Ƚ��ȳ�
	tasks.pop_front();
	tasks_mutex.unlock();
	task->Init();

}

void XThread::Activate()
{
#ifdef _WIN32
	int re = send(this->notify_send_fd, "c", 1, 0);
#else
	// ��fd[0]д��1���ַ� 'c'����ʾ���ܵ�����
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
	//������л������
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
	//�����߳�
	thread th(&XThread::Main, this);

	//�Ͽ������߳���ϵ
	th.detach();
}
//�߳���ں���
void XThread::Main()
{
	std::cout << id << " XThread::Main() begin" << std::endl;
	event_base_dispatch(base);
	event_base_free(base);
	
	std::cout << id << " XThread::Main() end" << std::endl;
}

bool XThread::Setup()
{
	//�����Ĺܵ� ������send recv��ȡ read write
	int fds[2];
	if (pipe(fds))
	{
		cerr << "pipe failed!" << endl;
		return false;
	}
	//��ȡ�󶨵�event�¼��У�д��Ҫ����
	notify_send_fd = fds[1];

	//����libevent�����ģ�������
	event_config* ev_conf = event_config_new();
	event_config_set_flag(ev_conf, EVENT_BASE_FLAG_NOLOCK);

	// ʹ�������Ĵ���base
	this->base = event_base_new_with_config(ev_conf);

	event_config_free(ev_conf);
	if (!this->base)
	{
		cerr << "event_base_new_with_config failed in thread!" << endl;
		return false;
	}

	//��ӹܵ������¼������ڼ����߳�ִ������
	// ֻҪfd[0]���ܵ���Ϣ ����NotifyCB
	event* ev = event_new(this->base, fds[0], EV_READ | EV_PERSIST, NotifyCB, this);
	
	event_add(ev, 0);

	return true;
}
