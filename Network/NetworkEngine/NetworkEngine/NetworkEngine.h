#pragma once

#include <WS2tcpip.h>
#include <winnt.h>
#include <MSWSock.h>
#include <list>
#include <string>

#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "mswsock.lib")

// 전방 선언
class Server;
class Client;
class Packet;
class SOverlapped;
class SSocket;

class NetworkEngine
{
public:
	NetworkEngine();
	~NetworkEngine();

private:
	HANDLE m_hIOCP;			// IOCP 핸들
	WSADATA m_wsaData;		// 윈속
	
	SOCKET m_ListenSocket;	// 리슨 소켓
	int m_HeadCount;		// 인원 제한

	LPFN_CONNECTEX m_ConnectEx;
	LPFN_DISCONNECTEX m_DisconnectEx;
	LPFN_ACCEPTEX m_AcceptEx;

	std::list<Server*> m_ConnectServerList;			// 연결되있는 서버 리스트

public:
	bool OpenServer(unsigned short port, int headCount = 0);						// 원하는 포트번호와 접속 인원 수 입력으로 서버 여는 함수
	bool ConnectServer(std::string name, unsigned short port, std::string ip);		// 연결을 원하는 서버의 별칭과 포트번호, ip로 서버 연결하는 함수

	bool GetOpenServerExFunction();					// 서버를 여는데에 필요한 Ex 함수를 생성하는 함수( 1회만 실행 )
	bool GetConnectServerExFunction();				// 서버와 연결하는데에 필요한 Ex 함수를 생성하는 함수( 1회만 실행 )

	bool BindListenSocket(unsigned short port);		// 리슨 소켓을 바인딩 하는 함수
	bool DoAccept();								// Accept 걸기

	void ServerworkThread();

	virtual void SendPacket(SOverlapped** psOverlapped, Packet* psPacket);
	
	virtual void ProcessRecv(DWORD dwNumberOfBytesTransferred, SOverlapped* psOverlapped, SSocket* psSocket);
	virtual void ProcessSend(DWORD dwNumberOfBytesTransferred, SOverlapped* psOverlapped, SSocket* psSocket);
	virtual void ProcessExit(DWORD dwNumberOfBytesTransferred, SOverlapped* psOverlapped, SSocket* psSocket);
};