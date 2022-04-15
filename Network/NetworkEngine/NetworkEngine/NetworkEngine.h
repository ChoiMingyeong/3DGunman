#pragma once

#include <WS2tcpip.h>
#include <winnt.h>
#include <MSWSock.h>
#include <list>
#include <string>

#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "mswsock.lib")

// ���� ����
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
	HANDLE m_hIOCP;			// IOCP �ڵ�
	WSADATA m_wsaData;		// ����
	
	SOCKET m_ListenSocket;	// ���� ����
	int m_HeadCount;		// �ο� ����

	LPFN_CONNECTEX m_ConnectEx;
	LPFN_DISCONNECTEX m_DisconnectEx;
	LPFN_ACCEPTEX m_AcceptEx;

	std::list<Server*> m_ConnectServerList;			// ������ִ� ���� ����Ʈ

public:
	bool OpenServer(unsigned short port, int headCount = 0);						// ���ϴ� ��Ʈ��ȣ�� ���� �ο� �� �Է����� ���� ���� �Լ�
	bool ConnectServer(std::string name, unsigned short port, std::string ip);		// ������ ���ϴ� ������ ��Ī�� ��Ʈ��ȣ, ip�� ���� �����ϴ� �Լ�

	bool GetOpenServerExFunction();					// ������ ���µ��� �ʿ��� Ex �Լ��� �����ϴ� �Լ�( 1ȸ�� ���� )
	bool GetConnectServerExFunction();				// ������ �����ϴµ��� �ʿ��� Ex �Լ��� �����ϴ� �Լ�( 1ȸ�� ���� )

	bool BindListenSocket(unsigned short port);		// ���� ������ ���ε� �ϴ� �Լ�
	bool DoAccept();								// Accept �ɱ�

	void ServerworkThread();

	virtual void SendPacket(SOverlapped** psOverlapped, Packet* psPacket);
	
	virtual void ProcessRecv(DWORD dwNumberOfBytesTransferred, SOverlapped* psOverlapped, SSocket* psSocket);
	virtual void ProcessSend(DWORD dwNumberOfBytesTransferred, SOverlapped* psOverlapped, SSocket* psSocket);
	virtual void ProcessExit(DWORD dwNumberOfBytesTransferred, SOverlapped* psOverlapped, SSocket* psSocket);
};