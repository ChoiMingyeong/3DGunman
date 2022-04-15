#include <thread>
#include "safe_pool.h"
#include "SSocket.h"
#include "NetworkEngine.h"

NetworkEngine::NetworkEngine()
	:m_ConnectEx(nullptr), m_DisconnectEx(nullptr), m_AcceptEx(nullptr), m_hIOCP(nullptr), m_ListenSocket(INVALID_SOCKET)
{
	// 윈속 시작
	if (WSAStartup(MAKEWORD(2, 2), &m_wsaData) != 0)
	{
		// 에러 발생 WSAStartup
		return;
	}

	// IOCP 핸들을 생성한다.
	m_hIOCP = CreateIoCompletionPort(INVALID_HANDLE_VALUE, nullptr, 0, 0);

#pragma region 작업 스레드 생성부
	// 시스템 정보 획득
	SYSTEM_INFO systemInfo;
	GetSystemInfo(&systemInfo);
	int threadCount = systemInfo.dwNumberOfProcessors * 2;

	std::thread** workThread;
	workThread = new std::thread * [threadCount];
	for (int i = 0; i < threadCount; i++)
	{
		workThread[i] = new std::thread(&ServerworkThread);
	}
#pragma endregion

#pragma region Accept 오버랩드 걸기
	safe_pool<SOverlapped*> sOverlappedPool = safe_pool<SOverlapped*>(m_HeadCount);
	for (int i = 0; i < sOverlappedPool.count(); i++)
	{
		auto tempElement = sOverlappedPool[i].value();
		if (INVALID_SOCKET == tempElement->socket)
		{
			tempElement->socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
		}
		tempElement->eIOType = SOverlapped::EIOType::Accept;

		// AcceptEx 함수 실행
		if (!m_AcceptEx(
			m_ListenSocket, tempElement->socket,
			&tempElement->szBuffer, 0,
			sizeof(SOCKADDR_IN) + 16, sizeof(SOCKADDR_IN) + 16,
			&dwByte, &tempElement->wsaOverlapped)
			 && WSAGetLastError() != ERROR_IO_PENDING)
		{
			// m_AcceptEx 함수 에러
		}
	}
#pragma endregion

}

NetworkEngine::~NetworkEngine()
{


	WSACleanup();
}

bool NetworkEngine::OpenServer(unsigned short port, int headCount)
{
	// 바인딩
	if (!BindListenSocket(port))
	{
		return false;
	}

	// 서버를 오픈할 때 필요한 ex 함수를 가져온다.
	if (!GetOpenServerExFunction())
	{
		closesocket(m_ListenSocket);
		m_ListenSocket = INVALID_SOCKET;
		return false;
	}

	// 인원 제한 수 만큼 Accept 걸기
	if (!DoAccept())
	{
		closesocket(m_ListenSocket);
		m_ListenSocket = INVALID_SOCKET;
		return false;
	}
}

bool NetworkEngine::ConnectServer(std::string name, unsigned short port, std::string ip)
{
	// 서버와 연결할 때 필요한 Ex 함수를 가져온다.
	if (!GetConnectServerExFunction())
	{
		return false;
	}

	// 소켓 생성
	SOCKET sock = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, nullptr, 0, WSA_FLAG_OVERLAPPED);

	// 서버에 Connect
	SOCKADDR_IN serverAddr;
	ZeroMemory(&serverAddr, sizeof(serverAddr));
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(port);
	inet_pton(AF_INET, ip.c_str(), &serverAddr.sin_addr.s_addr);

	if (SOCKET_ERROR == connect(sock, reinterpret_cast<SOCKADDR*>(&serverAddr), sizeof(serverAddr)))
	{
		closesocket(sock);
		return false;
	}
}

bool NetworkEngine::GetOpenServerExFunction()
{
	// 이미 Ex 함수가 불러와져 있다면 리턴
	if (nullptr != m_DisconnectEx && nullptr != m_AcceptEx)
	{
		return true;
	}

	DWORD dwByte;
	GUID GuidDisconnectEx = WSAID_DISCONNECTEX;
	GUID GuidAcceptEx = WSAID_ACCEPTEX;

	// DisconnectEx 함수 포인터 획득 및 메모리에 등록
	if (SOCKET_ERROR == WSAIoctl(
		m_ListenSocket, SIO_GET_EXTENSION_FUNCTION_POINTER,
		&GuidDisconnectEx, sizeof(GuidDisconnectEx),
		&m_DisconnectEx, sizeof(m_DisconnectEx),
		&dwByte, NULL, NULL))
	{
		// DisconnectEx 생성 오류
		return false;
	}

	// AcceptEx 함수 포인터 획득
	if (SOCKET_ERROR == WSAIoctl(
		m_ListenSocket, SIO_GET_EXTENSION_FUNCTION_POINTER,
		&GuidAcceptEx, sizeof(GuidAcceptEx),
		&m_AcceptEx, sizeof(m_AcceptEx),
		&dwByte, NULL, NULL))
	{
		// AcceptEx 생성 오류
		return false;
	}

	return false;
}

bool NetworkEngine::GetConnectServerExFunction()
{
	DWORD dwByte;
	GUID GuidDisconnectEx = WSAID_DISCONNECTEX;
	GUID GuidAcceptEx = WSAID_ACCEPTEX;
	GUID GuidConnectEx = WSAID_CONNECTEX;

	// DisconnectEx 함수 포인터 획득 및 메모리에 등록
	if (SOCKET_ERROR == WSAIoctl(
		m_ListenSocket, SIO_GET_EXTENSION_FUNCTION_POINTER,
		&GuidDisconnectEx, sizeof(GuidDisconnectEx),
		&m_DisconnectEx, sizeof(m_DisconnectEx),
		&dwByte, NULL, NULL))
	{
		// DisconnectEx 생성 오류
		closesocket(m_ListenSocket);
		WSACleanup();
	}

	// AcceptEx 함수 포인터 획득
	if (SOCKET_ERROR == WSAIoctl(
		m_ListenSocket, SIO_GET_EXTENSION_FUNCTION_POINTER,
		&GuidAcceptEx, sizeof(GuidAcceptEx),
		&m_AcceptEx, sizeof(m_AcceptEx),
		&dwByte, NULL, NULL))
	{
		// AcceptEx 생성 오류
		closesocket(m_ListenSocket);
		WSACleanup();
	}

	// AcceptEx 함수 포인터 획득
	if (SOCKET_ERROR == WSAIoctl(
		m_ListenSocket, SIO_GET_EXTENSION_FUNCTION_POINTER,
		&GuidConnectEx, sizeof(GuidConnectEx),
		&m_ConnectEx, sizeof(m_ConnectEx),
		&dwByte, NULL, NULL))
	{
		// AcceptEx 생성 오류
		closesocket(m_ListenSocket);
		WSACleanup();
	}

	return false;
}

bool NetworkEngine::BindListenSocket(unsigned short port)
{
	// 이미 리슨 소켓이 존재하는 경우( 서버가 이미 열린 상황 ) 리턴
	if (INVALID_SOCKET != m_ListenSocket)
	{
		return false;
	}

	// 리슨 소켓 생성
	m_ListenSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (m_ListenSocket == INVALID_SOCKET)
	{
		// 에러 발생 WSASocket
		return false;
	}

	// IOCP 핸들이 없다면 생성한다.
	if (nullptr == m_hIOCP)
	{
		m_hIOCP = CreateIoCompletionPort(INVALID_HANDLE_VALUE, nullptr, 0, 0);
	}

	// 리슨 소켓을 IOCP에 연결
	CreateIoCompletionPort(reinterpret_cast<HANDLE>(m_ListenSocket), m_hIOCP, 0, 0);

	// 전송 주소에 대한 설정값 지정
	SOCKADDR_IN serverAddr;
	ZeroMemory(&serverAddr, sizeof(serverAddr));
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);
	serverAddr.sin_port = htons(port);

	// 바인딩
	if (SOCKET_ERROR == bind(m_ListenSocket, reinterpret_cast<SOCKADDR*>(&serverAddr), sizeof(serverAddr)))
	{
		// 에러 발생 bind()
		closesocket(m_ListenSocket);
		m_ListenSocket = INVALID_SOCKET;

		return false;
	}

	return true;
}

bool NetworkEngine::DoAccept()
{
	return false;
}

void NetworkEngine::ServerworkThread()
{
	while (true)
	{
		DWORD dwNumberOfBytesTransferred = 0;

		// 임시 SOverlapped, SSocket
		SOverlapped* psOverlapped = nullptr;
		SSocket* psSocket = nullptr;

		OVERLAPPED_ENTRY entries[32];
		ULONG numEnries = 0;			// 가져온 Entries의 수

		if (FALSE == GetQueuedCompletionStatusEx(m_hIOCP, entries, 32, &numEnries, INFINITE, FALSE))
		{
			// 가져오기 실패
			continue;
		}

		// 가져온 수 만큼 반복
		for (ULONG i = 0; i < numEnries; ++i)
		{
			dwNumberOfBytesTransferred = entries[i].dwNumberOfBytesTransferred;

			psOverlapped = reinterpret_cast<SOverlapped*>(entries[i].lpOverlapped);
			if (psOverlapped == nullptr)
			{
				continue;
			}

			// Accept인 경우
			if (psOverlapped->eIOType == SOverlapped::EIOType::Accept)
			{
				/**/
				continue;
			}

			// Disconnect인 경우
			if (psOverlapped->eIOType == SOverlapped::EIOType::Disconnect)
			{
				/**/
				continue;
			}

			if (dwNumberOfBytesTransferred == 0)
			{
				// 클라이언트가 종료했을 때 Disconnect
				/**/
				continue;
			}

			if(dwNumberOfBytesTransferred == -1)
			{
				continue;
			}


		}
	}
}
