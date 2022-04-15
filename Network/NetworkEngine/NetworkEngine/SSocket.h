#pragma once
#include <string>
#include <WS2tcpip.h>
#define BUFSIZE 512

class SOverlapped
{
public:
	enum class EIOType
	{
		Accept,
		Connect,
		Disconnect,
		Recv,
		Send,
	};

	SOverlapped()
	{
		socket = INVALID_SOCKET;
		ZeroMemory(&wsaOverlapped, sizeof(wsaOverlapped));
		ZeroMemory(&szBuffer, sizeof(szBuffer) * BUFSIZE);
	}

	WSAOVERLAPPED wsaOverlapped;			// Overlapped I/O ����ü
	EIOType eIOType;				// ó�� ��� ���п�
	SOCKET socket;					// �� ���������� ��� ����
	char szBuffer[BUFSIZE];			// �ۼ��� ����
	int iDataSize;					// �����ͷ�
};

class SSocket
{
public:
	SSocket()
	{
		socket = INVALID_SOCKET;
		usPort = 0;
		refCount = 0;
		bConnected = FALSE;
	}

	~SSocket()
	{
		// ���� ����
		if (INVALID_SOCKET != socket)
		{
			shutdown(socket, SD_BOTH);
			closesocket(socket);
		}

	}

	SOCKET socket;					// �� ���������� ��� ����
	std::string strIP;			// ������ ������
	unsigned short usPort;		// ������ ��Ʈ
	int refCount;					// �ɸ� ��� ����
	BOOL bConnected;			// ���� ����
};

