#include "XThreadPool.h"
#include "XThread.h"
#include <iostream>
#include <thread>
#include "XTask.h"
using namespace std;
void XThreadPool::Init(int ThreadCount)
{
	this->ThreadCount = ThreadCount;
	this->lastThread = -1;
	for (int i = 0; i < ThreadCount; i++) {
		XThread *t = new XThread();
		t->id = i + 1;
		std::cout << "Create thread " << t->id << std::endl;

		//启动线程,包括线程的setup()
		t->Start();
		this->threads.push_back(t);

		//因为new XThread()是并发执行 当前线程休眠一段时间
		std::this_thread::sleep_for(std::chrono::milliseconds(100));
	}
}

void XThreadPool::Dispatch(XTask* task)
{
	//通过轮询
	if (!task) return;
	// task 0x621d30 地址
	int tid = (lastThread + 1) % ThreadCount;
	lastThread = tid;
	XThread* t = threads[tid];

	t->AddTask(task);

	//激活线程
	t->Activate();

}
