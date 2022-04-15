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

	WSAOVERLAPPED wsaOverlapped;			// Overlapped I/O 구조체
	EIOType eIOType;				// 처리 결과 구분용
	SOCKET socket;					// 이 오버랩드의 대상 소켓
	char szBuffer[BUFSIZE];			// 송수신 버퍼
	int iDataSize;					// 데이터량
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
		// 소켓 정리
		if (INVALID_SOCKET != socket)
		{
			shutdown(socket, SD_BOTH);
			closesocket(socket);
		}

	}

	SOCKET socket;					// 이 오버랩드의 대상 소켓
	std::string strIP;			// 원격지 아이피
	unsigned short usPort;		// 원격지 포트
	int refCount;					// 걸린 명령 갯수
	BOOL bConnected;			// 연결 여부
};

