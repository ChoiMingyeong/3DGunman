#pragma once

#include <WS2tcpip.h>
#include <Windows.h>
#include <MSWSock.h>
#include <unordered_map>
#include <list>
#include <thread>
#include <queue>
#include <string>
#include "safePool.h"

#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "mswsock.lib")

// 전방 선언
struct SOverlapped;
class Server;
class Client;
class SHeader;

struct SSocket
{
	SSocket()
	{
		socket = INVALID_SOCKET;
		usPort = 0;
	}

	SOCKET socket;
	std::string strIP;
	unsigned short usPort;
};

class ProcessPacket
{
public:
	virtual void Receive(SSocket*, char* buffer) {}
	virtual void AddClient(SOCKET) {}
	virtual void RemoveClient(SOCKET) {}
};

class MGNetwork
{
public:
	MGNetwork();
	~MGNetwork();

public:
	void OpenServer(unsigned short port, unsigned int headCount);					// port 번호와 최대 인원 수를 제한하여 서버를 여는 함수
	void ConnectServer(std::string name, unsigned short port, std::string ip);		// port 번호와 ip를 이용하여 서버에 연결하는 함수

protected:
	WSADATA m_wsaData;				// 윈속
	HANDLE m_hIOCP;					// IOCP 핸들

	SOCKET m_ListenSocket;			// 리슨 소켓
	unsigned int m_HeadCount;		// 오픈한 서버의 인원 제한 수

	LPFN_CONNECTEX				m_lpfnConnectEx;		// ConnectEx 확장 함수 포인터
	LPFN_DISCONNECTEX			m_lpfnDisconnectEx;		// DisconnectEx 확장 함수 포인터
	LPFN_ACCEPTEX				m_lpfnAcceptEx;			// AcceptEx 확장 함수 포인터
	LPFN_GETACCEPTEXSOCKADDRS	m_lpfnGetAcceptAddr;	// GetAcceptExSockAddrs 확장 함수 포인터

	std::thread*				m_workThreads;
	CRITICAL_SECTION			m_workCS;
	std::queue<std::string>		m_MSGQueue;
	char						m_MSGBuffer[512];
	std::thread*				m_MSGThread;
	bool						m_bMSGclose;

public:
	std::unordered_map<std::string, SOCKET>		m_ServerList;		// 연결된 서버 목록
	std::list<SOCKET>							m_ClientList;		// 연결된 클라이언트 목록
	
#pragma region Network 클래스 생성 준비부
protected:
	bool BindListenSocket(unsigned short port);		// 리슨 소켓 바인드하는 함수

	bool GetlpfnConnectEx();						// 확장 함수 ConnectEx 포인터를 가져오는 함수
	bool GetlpfnAcceptEx();							// 확장 함수 AcceptEx 포인터를 가져오는 함수
	bool GetlpfnDisconnectEx();						// 확장 함수 DisconnectEx 포인터를 가져오는 함수
	bool GetlpfnGetAcceptExAddr();					// 확장 함수 AcceptExAddr 포인터를 가져오는 함수
#pragma endregion

#pragma region 실제 네트워크 동작부
public:
	virtual bool SendPakcet(SHeader*, SOCKET sock);

public:
	ProcessPacket*				m_pPacketSystem;

protected:
	void CreateWorkThread();						// IOCP 큐에서 메세지를 가져와서 처리할 스레드를 생성하는 함수
	void WorkThread();								// IOCP 큐에서 메세지를 가져와서 처리하는 스레드 함수
	void MSGThread();

	void DoAccept();
	void DoDisconnect(SOverlapped*);

	bool DoReceive(SSocket*, SOverlapped*);
	bool DoSend(SSocket*, SOverlapped*);

	bool AddClientList(SOCKET);
	void RemoveClientList(SOCKET);

	virtual void ProcessConnect(SOverlapped*);
	virtual void ProcessAccept(SOverlapped*);
	virtual void ProcessDisconnect(SOverlapped*);

	virtual void ProcessRecv(DWORD dwNumberOfBytesTransferred, SOverlapped* psOverlapped, SSocket* psSocket);
	virtual void ProcessSend(DWORD dwNumberOfBytesTransferred, SOverlapped* psOverlapped, SSocket* psSocket);
#pragma endregion

};

