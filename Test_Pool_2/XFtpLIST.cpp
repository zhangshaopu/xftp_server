#include "XFtpLIST.h"
#include <iostream>
#include <event2/event.h>
#include <event2/bufferevent.h>
#ifdef _WIN32
#include <io.h>
#endif // _WIN32


using namespace std;
void XFtpLIST::Parse(std::string type, std::string msg)
{
	string resmsg = "";
	if (type == "PWD")
	{
		//257 "/" is current directory.
		resmsg = "257 \"";
		resmsg += cmdTask->curDir;
		resmsg += "\" is current dir.\r\n";

		ResCMD(resmsg);
	}
	else if (type == "LIST")
	{
		//1.��������ͨ��  2.150��Ӧ  3.����Ŀ¼����ͨ�� 4.�������226  5.�ر�����
		//����ͨ���ظ���Ϣ ʹ������ͨ������Ŀ¼
		//-rwxrwxrwx 1 root group 64463 Mar 14 09:53 101.jpg\r\n
		//1. ��������ͨ�� 
		ConnectPORT();

		//2. 1502 150��Ӧ
		ResCMD("150 Here comes the directory listing.\r\n");
		//string listdata = "-rwxrwxrwx 1 root group 64463 Mar 14 09:53 101.jpg\r\n";
		string listdata = GetListData(cmdTask->rootDir + cmdTask->curDir );
		//3. ����ͨ������
		Send(listdata);
	}
	else if (type == "CWD") { // �л�Ŀ¼
		//ȡ�������е�·��
		//CWD test\r\n
		int pos = msg.rfind(" ") + 1;
		//ȥ����β��\r\n
		string path = msg.substr(pos, msg.size() - pos - 2);
		if (path[0] == '/') //�ֶ�·��
		{
			cmdTask->curDir = path;
		}
		else
		{
			if (cmdTask->curDir[cmdTask->curDir.size() - 1] != '/')
				cmdTask->curDir += "/";
			cmdTask->curDir += path + "/";
		}
		//  /test/
		ResCMD("250 Directory succes chanaged.\r\n");

		//cmdTask->curDir += 
	}
	else if (type == "CDUP") {//�ص��ϲ�·��
		//  /Debug/test_ser.A3C61E95.tlog /Debug   /Debug/
		string path = cmdTask->curDir;
		//ͳһȥ����β�� /
		////  /Debug/test_ser.A3C61E95.tlog /Debug  
		if (path[path.size() - 1] == '/')
		{
			path = path.substr(0, path.size() - 1);
		}
		int pos = path.rfind("/"); // ����������
		path = path.substr(0, pos);
		cmdTask->curDir = path;
		ResCMD("250 Directory succes chanaged.\r\n");
	}
}

void XFtpLIST::Write(bufferevent* bev)
{
	//4 226 Transfer complete�������
	ResCMD("226 Transfer complete\r\n");
	//5 �ر�����
	Close();
}

void XFtpLIST::Event(bufferevent* bev, short what)
{
	//����Է�����ϵ������߻��������п����ղ���BEV_EVENT_EOF����
	if (what & (BEV_EVENT_EOF | BEV_EVENT_ERROR | BEV_EVENT_TIMEOUT))
	{
		cout << "BEV_EVENT_EOF | BEV_EVENT_ERROR |BEV_EVENT_TIMEOUT" << endl;
		//Close();
	}
	else if (what & BEV_EVENT_CONNECTED)
	{
		cout << "XFtpLIST BEV_EVENT_CONNECTED success" << endl;
	}
}

std::string XFtpLIST::GetListData(std::string path)
{
	//-rwxrwxrwx 1 root group 64463 Mar 14 09:53 101.jpg\r\n
	string data = "";
#ifdef _WIN32
	//�洢�ļ���Ϣ
	_finddata_t file;
	//Ŀ¼������
	path += "/*.*";
	intptr_t dir = _findfirst(path.c_str(), &file);
	if (dir < 0)
		return data;
	do
	{
		string tmp = "";
		//�Ƿ���Ŀ¼ȥ�� . ..
		if (file.attrib & _A_SUBDIR)
		{
			if (strcmp(file.name, ".") == 0 || strcmp(file.name, "..") == 0)
			{
				continue;
			}
			tmp = "drwxrwxrwx 1 root group ";
		}
		else
		{
			tmp = "-rwxrwxrwx 1 root group ";
		}
		//�ļ���С
		char buf[1024];
		//_CRT_SECURE_NO_WARNINGS
		sprintf(buf, "%u ", file.size);
		tmp += buf;

		//����ʱ��
		strftime(buf, sizeof(buf) - 1, "%b %d %H:%M ", localtime(&file.time_write));
		tmp += buf;
		tmp += file.name;
		tmp += "\r\n";
		data += tmp;

	} while (_findnext(dir, &file) == 0);
#else
	string cmd = "ls -l ";
	cmd += path;
	cout << "popen:" << cmd << endl;
	FILE* f = popen(cmd.c_str(), "r");
	if (!f)
		return data;
	char buffer[1024] = { 0 };
	for (;;)
	{
		int len = fread(buffer, 1, sizeof(buffer) - 1, f);
		if (len <= 0)break;
		buffer[len] = '\0';
		data += buffer;
	}
	pclose(f);
#endif // _WIN32

	


	return data;
}
