#pragma once

#include <WS2tcpip.h>
#include <unordered_map>
#include <MSWSock.h>
#include <thread>
#include <string>

#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "mswsock.lib")

// 전방 선언
class IOCP_YourServer;
class IOCP_MyServer;

typedef std::unordered_map<std::string, IOCP_YourServer*> IOCP_Server_Dictionary;

class IOCP_Network
{
public:
	IOCP_Network();
	~IOCP_Network();

public:
	bool OpenServer(std::string name, unsigned short port, int head_count);			// 내 서버 여는 함수
	bool ConnectServer(std::string name, std::string ip, unsigned short port);		// 다른 서버와 연결하는 함수

protected:
	WSADATA				m_wsaData;				// 윈속
	HANDLE				m_hIOCP;				// IOCP 핸들

	IOCP_MyServer*				m_pMyServer;			// 내가 연 서버 관리
	IOCP_Server_Dictionary		m_ConnectDictionary;	// 현재 열린 네트워크 관리

};

