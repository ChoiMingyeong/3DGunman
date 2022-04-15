#pragma once

#include <WS2tcpip.h>
#include <unordered_map>
#include <MSWSock.h>
#include <thread>
#include <string>

#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "mswsock.lib")

// ���� ����
class IOCP_YourServer;
class IOCP_MyServer;

typedef std::unordered_map<std::string, IOCP_YourServer*> IOCP_Server_Dictionary;

class IOCP_Network
{
public:
	IOCP_Network();
	~IOCP_Network();

public:
	bool OpenServer(std::string name, unsigned short port, int head_count);			// �� ���� ���� �Լ�
	bool ConnectServer(std::string name, std::string ip, unsigned short port);		// �ٸ� ������ �����ϴ� �Լ�

protected:
	WSADATA				m_wsaData;				// ����
	HANDLE				m_hIOCP;				// IOCP �ڵ�

	IOCP_MyServer*				m_pMyServer;			// ���� �� ���� ����
	IOCP_Server_Dictionary		m_ConnectDictionary;	// ���� ���� ��Ʈ��ũ ����

};

