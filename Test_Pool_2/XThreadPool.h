#pragma once
#include <vector>
class XThread;
class XTask;
class XThreadPool
{
public:
	//����ģʽ
	static XThreadPool* get() {
		static XThreadPool p;
		return &p;
	}
	//��ʼ�������̲߳������߳�
	void Init(int ThreadCount);

	//�ַ��߳�
	void Dispatch(XTask* task);


private:
	//�߳�����
	int ThreadCount = 0;

	int	lastThread = -1;

	//�̳߳��߳�
	std::vector<XThread*> threads;
	XThreadPool() {};

	
};

