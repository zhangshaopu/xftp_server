#pragma once
class XTask // �ӿ���
{
public:
	struct event_base* base = 0;
	int sock = 0;
	int thread_id = 0;
	//��ʼ������ �麯��
	virtual bool Init() = 0;
};

