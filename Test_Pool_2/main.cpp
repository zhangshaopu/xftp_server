#include <event2/event.h>
#include <event2/listener.h>
#include <string.h>
#include <iostream>

#include "XThreadPool.h"
#include "XFtpFactory.h"

#ifndef _WIN32
#include <signal.h>
#endif

using namespace std;
#define SPORT 5001


void listen_cb(struct evconnlistener* ev, evutil_socket_t socket, struct sockaddr* addr, int socklen, void* arg)
{
	cout << "listen_cb 被调用" << endl;
	
	//当有一个客户端连接 证明接下来会有cmd任务,创建一个task变量接受它
	XTask *task = XFtpFactory::Get()->CreateTask(); // 创建一个工厂生产出这样一个任务
	task->sock = socket;

	// 向线程池中线程分发任务：往线程中添加任务 激活该线程
	XThreadPool::get()->Dispatch(task);
}


int main()
{
#ifdef _WIN32 
	//初始化socket库
	WSADATA wsa;
	WSAStartup(MAKEWORD(2, 2), &wsa);
#else
	//忽略管道信号，发送数据给已关闭的socket
	if (signal(SIGPIPE, SIG_IGN) == SIG_ERR)
		return 1;
#endif
	//1 初始化线程池
	XThreadPool::get()->Init(10);
	std::cout << "test threadpool init success!\n";

	event_base* base = event_base_new();

	if (base) {
		cout << "event_base_new success!" << endl;
	}
	
	//监听端口
	//socket bind listen绑定
	sockaddr_in sin;
	memset(&sin, 0, sizeof(sin));
	sin.sin_family = AF_INET;
	sin.sin_port = htons(SPORT);

	cout << "绑定listen :" << endl;
	// 封装bind 和 listen,同时可以传入监听过程中的回调函数 
	evconnlistener* ev = evconnlistener_new_bind(base,
		listen_cb,
		base,
		LEV_OPT_REUSEABLE | LEV_OPT_CLOSE_ON_FREE,
		10,
		(sockaddr*)&sin,
		sizeof(sin)
	);

	if (base) {
		cout << "dispatch base " << endl;
		event_base_dispatch(base);
	}
	if (ev) {
		cout << "free evconnlistener " << endl;
		evconnlistener_free(ev);
	}
	if (base) {
		cout << "bese free" << endl;
		event_base_free(base);
	}
	



#ifdef _WIN32
	WSACleanup();
#endif
	return 0;
}
