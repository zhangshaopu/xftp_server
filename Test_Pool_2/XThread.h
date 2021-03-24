#pragma once
#include <event2/event.h>
#include <list>
#include <mutex>
class XTask;
class XThread
{
public:
	XThread();

	//�����߳�
	void Start();

	//�߳���ں���
	void Main();

	//��װ�̣߳���ʼ��event_base�͹ܵ������¼����ڼ���
	bool Setup();

	//�̱߳��
	int id = 0;

	//�յ����̷߳����ļ�����Ϣ���̳߳صķַ���
	void Notify(evutil_socket_t fd, short which);

	//�̼߳���
	void Activate();

	//��Ӵ������� һ���߳̿��Դ��������� ����һ��event_base
	void AddTask(XTask *t);


private:

	int notify_send_fd = 0;

	struct event_base* base = 0;

	//�����б�
	std::list<XTask*> tasks;
	//�̰߳�ȫ ����
	std::mutex tasks_mutex;
};

