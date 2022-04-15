#include <iostream>
#include "Packet.h"
#include "MGNetwork.h"

struct SOverlapped
{
	enum class EIOType
	{
		IO_TYPE_RECV,
		IO_TYPE_SEND,
		IO_TYPE_ACCEPT,
		IO_TYPE_CONNECT,
		IO_TYPE_DISCONNECT,
		IO_TYPE_NONE,
	};

	SOverlapped()
	{
		ZeroMemory(&wsaOverlapped, sizeof(wsaOverlapped));
		socket = INVALID_SOCKET;
		ZeroMemory(szBuffer, sizeof(szBuffer));
		iDataSize = 0;
	}

	WSAOVERLAPPED wsaOverlapped;		// Overlapped I/O 에 사용될 구조체
	EIOType	eIOType;		// 처리 종류 확인

	SOCKET	socket;			// Overlapped 대상 소켓
	char	szBuffer[512];		// 버퍼
	int		iDataSize;			// 데이터의 양
};

class Server
{
public:
	Server(unsigned short port, std::string ip, HANDLE hIOCP)
	{
		psOverlapped = new SOverlapped();
		psOverlapped->eIOType = SOverlapped::EIOType::IO_TYPE_CONNECT;

		// 소켓 생성
		psOverlapped->socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
		//psOverlapped->socket = WSASocketW(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);

		// 소켓 생성 실패
		if (INVALID_SOCKET == psOverlapped->socket)
		{
			// 서버용 소켓 생성 실패
			return;
		}

		// 서버 정보 입력
		ZeroMemory(&addr, sizeof(addr));
		addr.sin_family = AF_INET;
		addr.sin_addr.s_addr = htonl(INADDR_ANY);
		addr.sin_port = htons(port);

		//// 바인딩
		//if (!::bind(psOverlapped->socket, reinterpret_cast<SOCKADDR*>(&addr), sizeof(addr)))
		//{
		//	std::cout << "서버용 소켓 바인드 실패 : " << std::endl;
		//	closesocket(psOverlapped->socket);
		//}

		inet_pton(AF_INET, ip.c_str(), &addr.sin_addr);
	}

public:
	SOverlapped* psOverlapped;
	SOCKADDR_IN		addr;
};

//class Client
//{
//public:
//	Client(unsigned short port)
//	{
//		psOverlapped = new SOverlapped();
//		psOverlapped->eIOType = SOverlapped::EIOType::IO_TYPE_ACCEPT;
//
//		// 소켓 생성
//		psOverlapped->socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
//
//		// 소켓 생성 실패
//		if (INVALID_SOCKET == psOverlapped->socket)
//		{
//			std::cout << "클라이언트용 소켓 생성 실패" << std::endl;
//			return;
//		}
//	}
//
//public:
//	SOverlapped* psOverlapped;
//};

MGNetwork::MGNetwork()
	: m_lpfnAcceptEx(nullptr), m_lpfnConnectEx(nullptr), m_lpfnDisconnectEx(nullptr), m_lpfnGetAcceptAddr(nullptr),
	m_ListenSocket(INVALID_SOCKET), m_hIOCP(nullptr), m_HeadCount(0),
	m_workThreads(nullptr), m_pPacketSystem(nullptr), m_bMSGclose(false)
{
	ZeroMemory(m_MSGBuffer, sizeof(m_MSGBuffer));

	// 윈속 시작
	if (WSAStartup(MAKEWORD(2, 2), &m_wsaData) != 0)
	{
		// 에러 발생 WSAStartup
		ZeroMemory(m_MSGBuffer, sizeof(m_MSGBuffer));
		sprintf_s(m_MSGBuffer, sizeof(m_MSGBuffer), "WSAStartup 에러");
		m_MSGQueue.push(std::string(m_MSGBuffer));
		return;
	}

	m_MSGThread = new std::thread(&MGNetwork::MSGThread, this);
	InitializeCriticalSection(&m_workCS);

	// IOCP 핸들을 생성한다.
	m_hIOCP = CreateIoCompletionPort(INVALID_HANDLE_VALUE, nullptr, 0, 0);
}

MGNetwork::~MGNetwork()
{
	// 스레드 종료 대기
	SYSTEM_INFO systemInfo;
	GetSystemInfo(&systemInfo);
	int threadCount = systemInfo.dwNumberOfProcessors * 2;

	if (nullptr != m_workThreads)
	{
		for (int i = 0; i < threadCount; ++i)
		{
			m_workThreads[i].join();
		}
	}

	DeleteCriticalSection(&m_workCS);

	if (INVALID_SOCKET != m_ListenSocket)
	{
		closesocket(m_ListenSocket);
		m_ListenSocket = INVALID_SOCKET;
	}

	if (nullptr != m_hIOCP)
	{
		CloseHandle(m_hIOCP);
	}

	WSACleanup();

	m_MSGThread = new std::thread(&MGNetwork::MSGThread, this);

	ZeroMemory(m_MSGBuffer, sizeof(m_MSGBuffer));
	sprintf_s(m_MSGBuffer, sizeof(m_MSGBuffer), "네트워크 종료\n");
	m_MSGQueue.push(std::string(m_MSGBuffer));

	m_bMSGclose = true;
	m_MSGThread->join();
}

void MGNetwork::OpenServer(unsigned short port, unsigned int headCount)
{
	// 리슨 소켓 바인딩 확인
	if (!BindListenSocket(port))
	{
		// 에러 발생 bind() 실패

		ZeroMemory(m_MSGBuffer, sizeof(m_MSGBuffer));
		sprintf_s(m_MSGBuffer, sizeof(m_MSGBuffer), "리슨 소켓 바인드 실패\n");
		m_MSGQueue.push(std::string(m_MSGBuffer));
		return;
	}

	// AcceptEx 확장 함수 포인터를 얻어오는데에 실패하면 리턴
	if (!GetlpfnAcceptEx())
	{
		// 확장 함수 포인터 획득 실패
		ZeroMemory(m_MSGBuffer, sizeof(m_MSGBuffer));
		sprintf_s(m_MSGBuffer, sizeof(m_MSGBuffer), "확장 함수 포인터 획득 실패 : AcceptEx\n");
		m_MSGQueue.push(std::string(m_MSGBuffer));
		return;
	}

	// GetAcceptExAddr 확장 함수 포인터를 얻어오는데에 실패하면 리턴
	if (!GetlpfnGetAcceptExAddr())
	{
		// 확장 함수 포인터 획득 실패
		ZeroMemory(m_MSGBuffer, sizeof(m_MSGBuffer));
		sprintf_s(m_MSGBuffer, sizeof(m_MSGBuffer), "확장 함수 포인터 획득 실패 : GetAcceptExAddr\n");
		m_MSGQueue.push(std::string(m_MSGBuffer));
		return;
	}

	// DisconnectEx 확장 함수 포인터를 얻어오는데에 실패하면 리턴
	if (!GetlpfnDisconnectEx())
	{
		// 확장 함수 포인터 획득 실패
		ZeroMemory(m_MSGBuffer, sizeof(m_MSGBuffer));
		sprintf_s(m_MSGBuffer, sizeof(m_MSGBuffer), "확장 함수 포인터 획득 실패 : DisconnectEx\n");
		m_MSGQueue.push(std::string(m_MSGBuffer));
		return;
	}

	// 인원 제한 수 만큼 접속 소켓 걸어두기
	m_HeadCount = headCount;

	if (SOCKET_ERROR == listen(m_ListenSocket, SOMAXCONN))
	{
		// 에러 발생 listen
		ZeroMemory(m_MSGBuffer, sizeof(m_MSGBuffer));
		sprintf_s(m_MSGBuffer, sizeof(m_MSGBuffer), "listen() 실패\n");
		m_MSGQueue.push(std::string(m_MSGBuffer));
		return;
	}

	// 최대 Accept 수 + 오류 처리용 여분 수
	int acceptCount = headCount + (headCount / 2);
	ZeroMemory(m_MSGBuffer, sizeof(m_MSGBuffer));
	sprintf_s(m_MSGBuffer, sizeof(m_MSGBuffer), "포트번호 : %d, 최대 접속 수 : %d\n", port, headCount);
	m_MSGQueue.push(std::string(m_MSGBuffer));

	for (int i = 0; i < acceptCount; ++i)
	{
		// Overlapped 생성 및 AcceptEx 실행
		SOverlapped* psOverlapped = new SOverlapped();
		psOverlapped->eIOType = SOverlapped::EIOType::IO_TYPE_ACCEPT;

		// 소켓 생성
		psOverlapped->socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

		// 소켓 생성 실패
		if (INVALID_SOCKET == psOverlapped->socket)
		{
			ZeroMemory(m_MSGBuffer, sizeof(m_MSGBuffer));
			sprintf_s(m_MSGBuffer, sizeof(m_MSGBuffer), "클라이언트용 소켓 생성 실패\n");
			m_MSGQueue.push(std::string(m_MSGBuffer));
			return;
		}
		DWORD dwByte;

		if (m_lpfnAcceptEx(
			m_ListenSocket, psOverlapped->socket, &psOverlapped->szBuffer,
			0, sizeof(SOCKADDR_IN) + 16, sizeof(SOCKADDR_IN) + 16,
			&dwByte, &psOverlapped->wsaOverlapped))
		{
			//std::cout << "AcceptEx succeeded immediately" << std::endl;
		}
		else if (WSAGetLastError() == ERROR_IO_PENDING)
		{
			//std::cout << "AcceptEx pending" << std::endl;

			DWORD numBytes;
			if (GetOverlappedResult((HANDLE)psOverlapped->socket, &psOverlapped->wsaOverlapped, &numBytes, TRUE))
			{
				//std::cout << "AcceptEx succeeded" << std::endl;
			}
			else
			{
				ZeroMemory(m_MSGBuffer, sizeof(m_MSGBuffer));
				sprintf_s(m_MSGBuffer, sizeof(m_MSGBuffer), "AcceptEx failed\n");
				m_MSGQueue.push(std::string(m_MSGBuffer));
			}
		}
		else
		{
			ZeroMemory(m_MSGBuffer, sizeof(m_MSGBuffer));
			sprintf_s(m_MSGBuffer, sizeof(m_MSGBuffer), "AcceptEx failed\n");
			m_MSGQueue.push(std::string(m_MSGBuffer));
		}
	}
	ZeroMemory(m_MSGBuffer, sizeof(m_MSGBuffer));
	sprintf_s(m_MSGBuffer, sizeof(m_MSGBuffer), "서버 오픈\n");
	m_MSGQueue.push(std::string(m_MSGBuffer));

	if (nullptr == m_workThreads)
	{
		// 작업 스레드 생성
		CreateWorkThread();
	}
}

void MGNetwork::ConnectServer(std::string name, unsigned short port, std::string ip)
{
	// ConnectEx 확장 함수 포인터를 얻어오는데에 실패하면 리턴
	if (!GetlpfnConnectEx())
	{
		// 확장 함수 포인터 획득 실패
		return;
	}

	// name 으로 m_ServerList에 키값으로 등록될 이름을 지정하고,
	// 포트번호와 ip 주소를 이용하여 다른 서버에 연결한다.
	DWORD numBytes;
	Server* tempServer = new Server(port, ip, m_hIOCP);

	ZeroMemory(m_MSGBuffer, sizeof(m_MSGBuffer));
	sprintf_s(m_MSGBuffer, sizeof(m_MSGBuffer), "[ConnectServer] %s : %d\n", ip.c_str(), port);
	m_MSGQueue.push(std::string(m_MSGBuffer));

	// ConnectEx 에러때문에 일단 연결 확인용으로 동기 함수 사용..
	if (SOCKET_ERROR == connect(tempServer->psOverlapped->socket, reinterpret_cast<SOCKADDR*>(&tempServer->addr), sizeof(tempServer->addr)))
	{
		if (WSAGetLastError() == WSAECONNREFUSED)
		{
			return;
		}
	}

	// Overlapped 생성 및 AcceptEx 실행
	SOverlapped* psOverlapped = new SOverlapped();
	SSocket* psSocket = new SSocket();

	// 소켓 생성
	psOverlapped->eIOType = SOverlapped::EIOType::IO_TYPE_RECV;
	psOverlapped->socket = tempServer->psOverlapped->socket;

	psSocket->socket = psOverlapped->socket;
	psSocket->strIP = ip;
	psSocket->usPort = ntohs(port);

	// IOCP 생성 및 소켓 등록
	CreateIoCompletionPort(reinterpret_cast<HANDLE>(psOverlapped->socket), m_hIOCP, reinterpret_cast<ULONG_PTR>(psSocket), 0);

	m_ServerList.insert(std::make_pair(name, psOverlapped->socket));
	if (false == DoReceive(psSocket, psOverlapped))
	{
		delete psSocket;
		delete psOverlapped;
	}

	// 왜 안되는데?????????????????????
	//if (m_lpfnConnectEx(
	//	tempServer->psOverlapped->socket,
	//	reinterpret_cast<SOCKADDR*>(&tempServer->addr), sizeof(&tempServer->addr),
	//	NULL, NULL, NULL,
	//	reinterpret_cast<LPOVERLAPPED>(&tempServer->psOverlapped->wsaOverlapped)))
	//{
	//	// ConnectEx succeeded immediately
	//	std::cout << "ConnectEx succeeded immediately" << std::endl;
	//	m_ServerList.insert(std::make_pair(name, tempServer));
	//}
	//else if (WSAGetLastError() != ERROR_IO_PENDING)
	//{
	//	std::cout << "ConnectEx failed : " << WSAGetLastError() << std::endl;
	//}

	if (nullptr == m_workThreads)
	{
		// 작업 스레드 생성
		CreateWorkThread();
	}
}

bool MGNetwork::BindListenSocket(unsigned short port)
{
	// 이미 리슨 소켓이 존재하는 경우 리턴
	if (INVALID_SOCKET != m_ListenSocket)
	{
		return true;
	}

	// 리슨 소켓 생성
	m_ListenSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (m_ListenSocket == INVALID_SOCKET)
	{
		// 에러 발생 WSASocket
		ZeroMemory(m_MSGBuffer, sizeof(m_MSGBuffer));
		sprintf_s(m_MSGBuffer, sizeof(m_MSGBuffer), "WSASocket 생성 오류\n");
		m_MSGQueue.push(std::string(m_MSGBuffer));
		return false;
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
	if (SOCKET_ERROR == ::bind(m_ListenSocket, reinterpret_cast<SOCKADDR*>(&serverAddr), sizeof(serverAddr)))
	{
		// 에러 발생 bind()
		ZeroMemory(m_MSGBuffer, sizeof(m_MSGBuffer));
		sprintf_s(m_MSGBuffer, sizeof(m_MSGBuffer), "리슨 소켓 바인딩 실패\n");
		m_MSGQueue.push(std::string(m_MSGBuffer));

		closesocket(m_ListenSocket);
		m_ListenSocket = INVALID_SOCKET;

		return false;
	}

	return true;
}

bool MGNetwork::GetlpfnConnectEx()
{
	// 이미 해당 함수 포인터를 얻었다면 리턴
	if (nullptr != m_lpfnConnectEx)
	{
		return true;
	}

	SOCKET tempSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (INVALID_SOCKET == tempSocket)
	{
		return false;
	}

	// 전송 주소에 대한 설정값 지정
	SOCKADDR_IN serverAddr;
	ZeroMemory(&serverAddr, sizeof(serverAddr));
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);
	serverAddr.sin_port = htons(0);

	// 소켓을 IOCP에 연결
	CreateIoCompletionPort(reinterpret_cast<HANDLE>(tempSocket), m_hIOCP, 0, 0);

	// 바인딩
	if (SOCKET_ERROR == ::bind(tempSocket, reinterpret_cast<SOCKADDR*>(&serverAddr), sizeof(serverAddr)))
	{
		// 에러 발생 bind()
		closesocket(tempSocket);
		tempSocket = INVALID_SOCKET;

		return false;
	}

	DWORD dwByte;
	GUID GuidConnectEx = WSAID_CONNECTEX;

	// ConnectEx 함수 포인터 획득 및 메모리에 등록
	if (SOCKET_ERROR == WSAIoctl(
		tempSocket, SIO_GET_EXTENSION_FUNCTION_POINTER,
		&GuidConnectEx, sizeof(GuidConnectEx),
		&m_lpfnConnectEx, sizeof(m_lpfnConnectEx),
		&dwByte, NULL, NULL))
	{
		// ConnectEx 생성 오류
		return false;
	}

	return true;
}

bool MGNetwork::GetlpfnAcceptEx()
{
	// 이미 해당 함수 포인터를 얻었다면 리턴
	if (nullptr != m_lpfnAcceptEx)
	{
		return true;
	}

	// 리슨 소켓 바인딩 확인
	if (INVALID_SOCKET == m_ListenSocket)
	{
		return false;
	}

	DWORD dwByte;
	GUID GuidAcceptEx = WSAID_ACCEPTEX;

	// AcceptEx 함수 포인터 획득
	if (SOCKET_ERROR == WSAIoctl(
		m_ListenSocket, SIO_GET_EXTENSION_FUNCTION_POINTER,
		&GuidAcceptEx, sizeof(GuidAcceptEx),
		&m_lpfnAcceptEx, sizeof(m_lpfnAcceptEx),
		&dwByte, NULL, NULL))
	{
		// ConnectEx 생성 오류
		return false;
	}

	return true;
}

bool MGNetwork::GetlpfnDisconnectEx()
{
	// 이미 해당 함수 포인터를 얻었다면 리턴
	if (nullptr != m_lpfnDisconnectEx)
	{
		return true;
	}

	// 리슨 소켓 바인딩 확인
	if (INVALID_SOCKET == m_ListenSocket)
	{
		return false;
	}

	DWORD dwByte;
	GUID GuidDisconnectEx = WSAID_DISCONNECTEX;

	// DisconnectEx 함수 포인터 획득
	if (SOCKET_ERROR == WSAIoctl(
		m_ListenSocket, SIO_GET_EXTENSION_FUNCTION_POINTER,
		&GuidDisconnectEx, sizeof(GuidDisconnectEx),
		&m_lpfnDisconnectEx, sizeof(m_lpfnDisconnectEx),
		&dwByte, NULL, NULL))
	{
		// DisconnectEx 생성 오류
		return false;
	}

	return true;
}

bool MGNetwork::GetlpfnGetAcceptExAddr()
{
	// 이미 해당 함수 포인터를 얻었다면 리턴
	if (nullptr != m_lpfnGetAcceptAddr)
	{
		return true;
	}

	// 리슨 소켓 바인딩 확인
	if (INVALID_SOCKET == m_ListenSocket)
	{
		return false;
	}

	DWORD dwByte;
	GUID GuidGetAcceptExAddrEx = WSAID_GETACCEPTEXSOCKADDRS;

	// GetAcceptExAddr 함수 포인터 획득
	if (SOCKET_ERROR == WSAIoctl(
		m_ListenSocket, SIO_GET_EXTENSION_FUNCTION_POINTER,
		&GuidGetAcceptExAddrEx, sizeof(GuidGetAcceptExAddrEx),
		&m_lpfnGetAcceptAddr, sizeof(m_lpfnGetAcceptAddr),
		&dwByte, NULL, NULL))
	{
		// DisconnectEx 생성 오류
		return false;
	}

	return true;
}

bool MGNetwork::SendPakcet(SHeader* packet, SOCKET sock)
{
	if (PacketTypeMax <= packet->usType)
	{
		return false;
	}

	SOverlapped* psOverlapped = new SOverlapped();
	psOverlapped->eIOType = SOverlapped::EIOType::IO_TYPE_SEND;
	psOverlapped->socket = sock;

	// 패킷 복사
	psOverlapped->iDataSize = 2 + packet->usSize;

	if (sizeof(psOverlapped->szBuffer) < psOverlapped->iDataSize)
	{
		delete psOverlapped;
		return false;
	}

	memcpy_s(psOverlapped->szBuffer, sizeof(psOverlapped->szBuffer), packet, psOverlapped->iDataSize);

	// WSABUF 셋팅
	WSABUF wsaBuffer;
	wsaBuffer.buf = psOverlapped->szBuffer;
	wsaBuffer.len = psOverlapped->iDataSize;

	// WSASend() 오버랩드 걸기
	DWORD dwNumberOfBytesSent = 0;

	int iResult = WSASend(
		psOverlapped->socket,
		&wsaBuffer,
		1,
		&dwNumberOfBytesSent,
		0,
		&psOverlapped->wsaOverlapped,
		nullptr);

	if ((SOCKET_ERROR == iResult) && (WSAGetLastError() != WSA_IO_PENDING))
	{
		ZeroMemory(m_MSGBuffer, sizeof(m_MSGBuffer));
		sprintf_s(m_MSGBuffer, sizeof(m_MSGBuffer), "WSASend() 에러\n");
		m_MSGQueue.push(std::string(m_MSGBuffer));
		delete psOverlapped;
	}

	return true;
}

void MGNetwork::CreateWorkThread()
{
	// 시스템 정보 획득
	SYSTEM_INFO systemInfo;
	GetSystemInfo(&systemInfo);
	int threadCount = systemInfo.dwNumberOfProcessors * 2;

	m_workThreads = new std::thread[threadCount];
	for (int i = 0; i < threadCount; i++)
	{
		m_workThreads[i] = std::thread(&MGNetwork::WorkThread, this);
	}
}

void MGNetwork::WorkThread()
{
	while (true)
	{
		EnterCriticalSection(&m_workCS);

		DWORD dwNumberOfBytesTransferred = 0;

		// 임시 SOverlapped, SSocket
		SOverlapped* psOverlapped = nullptr;
		SSocket* psSocket = nullptr;

		OVERLAPPED_ENTRY entries[32];
		ULONG numEnries = 0;			// 가져온 Entries의 수


		if (FALSE == GetQueuedCompletionStatusEx(m_hIOCP, entries, 32, &numEnries, INFINITE, FALSE))
		{
			// 가져오기 실패
			LeaveCriticalSection(&m_workCS);
			Sleep(0);
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

			// Connect인 경우
			if (psOverlapped->eIOType == SOverlapped::EIOType::IO_TYPE_CONNECT)
			{
				ProcessConnect(psOverlapped);
				continue;
			}

			// Accept인 경우
			if (psOverlapped->eIOType == SOverlapped::EIOType::IO_TYPE_ACCEPT)
			{
				ProcessAccept(psOverlapped);
				continue;
			}

			// Disconnect인 경우
			if (psOverlapped->eIOType == SOverlapped::EIOType::IO_TYPE_DISCONNECT)
			{
				ProcessDisconnect(psOverlapped);
				continue;
			}

			if (dwNumberOfBytesTransferred == 0)
			{
				// 클라이언트가 종료했을 때 Disconnect
				DoDisconnect(psOverlapped);
				continue;
			}

			if (dwNumberOfBytesTransferred == -1)
			{
				continue;
			}

			// SSocket 확인 (Accept가 아니면 SSocket이 담긴다)
			psSocket = reinterpret_cast<SSocket*>(entries[i].lpCompletionKey);
			if (psSocket == nullptr)
			{
				if (psOverlapped->eIOType != SOverlapped::EIOType::IO_TYPE_ACCEPT)
				{
					// PQCS : GQCS를 리턴시킬 수 있는 함수, 주로 종료에 쓰인다.
					PostQueuedCompletionStatus(m_hIOCP, 0, entries[i].lpCompletionKey, entries[i].lpOverlapped);
					continue;
				}
			}

			// IO_Type에 따라 처리 선택
			if (psOverlapped->eIOType == SOverlapped::EIOType::IO_TYPE_RECV)
			{
				// Overlapped 결과가 제대로 나온 경우 소켓의 참조 카운트 감소
				ProcessRecv(dwNumberOfBytesTransferred, psOverlapped, psSocket);
			}
			else if (psOverlapped->eIOType == SOverlapped::EIOType::IO_TYPE_SEND)
			{
				// Overlapped 결과가 제대로 나온 경우 소켓의 참조 카운트 감소
				ProcessSend(dwNumberOfBytesTransferred, psOverlapped, psSocket);
			}
		}

		LeaveCriticalSection(&m_workCS);
	}
}

void MGNetwork::MSGThread()
{
	while (!m_bMSGclose)
	{
		while (!m_MSGQueue.empty())
		{
			std::string str = m_MSGQueue.front();
			m_MSGQueue.pop();
			std::cout << str.c_str() << std::endl;
		}

		Sleep(0);
	}
}

void MGNetwork::DoAccept()
{
}

void MGNetwork::DoDisconnect(SOverlapped* psOverlapped)
{
	shutdown(psOverlapped->socket, SD_SEND);

	SOverlapped* tempOverlapped = new SOverlapped();
	tempOverlapped->eIOType = SOverlapped::EIOType::IO_TYPE_DISCONNECT;
	tempOverlapped->socket = psOverlapped->socket;

	bool result = m_lpfnDisconnectEx(tempOverlapped->socket, &tempOverlapped->wsaOverlapped, TF_REUSE_SOCKET, 0);
	if (result == false && WSAGetLastError() != ERROR_IO_PENDING)
	{
		ZeroMemory(m_MSGBuffer, sizeof(m_MSGBuffer));
		sprintf_s(m_MSGBuffer, sizeof(m_MSGBuffer), "Disconnect Fail\n");
		m_MSGQueue.push(std::string(m_MSGBuffer));
	}

	ZeroMemory(m_MSGBuffer, sizeof(m_MSGBuffer));
	sprintf_s(m_MSGBuffer, sizeof(m_MSGBuffer), "Disconnect Success\n");
	//m_MSGQueue.push(std::string(m_MSGBuffer));

	delete psOverlapped;
}

bool MGNetwork::DoReceive(SSocket* psSocket, SOverlapped* psOverlapped)
{
	if (INVALID_SOCKET == psSocket->socket)
	{
		return false;
	}

	psOverlapped->eIOType = SOverlapped::EIOType::IO_TYPE_RECV;
	psOverlapped->socket = psSocket->socket;

	WSABUF wsaBuffer;
	wsaBuffer.buf = psOverlapped->szBuffer + psOverlapped->iDataSize;
	wsaBuffer.len = sizeof(psOverlapped->szBuffer) - psOverlapped->iDataSize;
	DWORD dwNumberOfBytesRecvd = 0, dwFlag = 0;

	// WSARecv
	int result = WSARecv(
		psOverlapped->socket,
		&wsaBuffer,
		1,
		&dwNumberOfBytesRecvd,
		&dwFlag,
		&psOverlapped->wsaOverlapped,
		nullptr);

	if ((result == SOCKET_ERROR) && (WSAGetLastError() != WSA_IO_PENDING))
	{
		ZeroMemory(m_MSGBuffer, sizeof(m_MSGBuffer));
		sprintf_s(m_MSGBuffer, sizeof(m_MSGBuffer), "WSARecv 에러 발생 : %d\n", WSAGetLastError());
		m_MSGQueue.push(std::string(m_MSGBuffer));
		return false;
	}

	ZeroMemory(m_MSGBuffer, sizeof(m_MSGBuffer));
	sprintf_s(m_MSGBuffer, sizeof(m_MSGBuffer), "DoReceive Success\n");
	//m_MSGQueue.push(std::string(m_MSGBuffer));
	return true;
}

bool MGNetwork::DoSend(SSocket* psSocket, SOverlapped* psOverlapped)
{
	if (INVALID_SOCKET == psSocket->socket)
	{
		return false;
	}

	psOverlapped->eIOType = SOverlapped::EIOType::IO_TYPE_RECV;
	psOverlapped->socket = psSocket->socket;

	WSABUF wsaBuffer;
	wsaBuffer.buf = psOverlapped->szBuffer + psOverlapped->iDataSize;
	wsaBuffer.len = psOverlapped->iDataSize;

	// WSASend() 오버랩드 걸기
	DWORD dwNumberOfBytesSent = 0;

	int iResult = WSASend(psOverlapped->socket,
		&wsaBuffer,
		1,
		&dwNumberOfBytesSent,
		0,
		&psOverlapped->wsaOverlapped,
		nullptr);

	if ((SOCKET_ERROR == iResult) && (WSAGetLastError() != WSA_IO_PENDING))
	{
		ZeroMemory(m_MSGBuffer, sizeof(m_MSGBuffer));
		sprintf_s(m_MSGBuffer, sizeof(m_MSGBuffer), "WSASend 오류\n");
		m_MSGQueue.push(std::string(m_MSGBuffer));
		delete psOverlapped;

		return false;
	}

	delete psOverlapped;

	return true;
}

bool MGNetwork::AddClientList(SOCKET sock)
{
	if (m_HeadCount <= m_ClientList.size())
	{
		return false;
	}

	m_ClientList.push_back(sock);
	if (nullptr != m_pPacketSystem)
	{
		m_pPacketSystem->AddClient(sock);
	}

	return true;
}

void MGNetwork::RemoveClientList(SOCKET sock)
{
	for (auto iter = m_ClientList.begin(); iter != m_ClientList.end(); iter++)
	{
		if (sock == *iter)
		{
			m_ClientList.erase(iter);
			if (nullptr != m_pPacketSystem)
			{
				m_pPacketSystem->RemoveClient(sock);
			}
			return;
		}
	}
}

void MGNetwork::ProcessConnect(SOverlapped* psOverlapped)
{
	ZeroMemory(m_MSGBuffer, sizeof(m_MSGBuffer));
	sprintf_s(m_MSGBuffer, sizeof(m_MSGBuffer), "Connect Message\n");
	//m_MSGQueue.push(std::string(m_MSGBuffer));
}

void MGNetwork::ProcessAccept(SOverlapped* psOverlapped)
{
	ZeroMemory(m_MSGBuffer, sizeof(m_MSGBuffer));
	sprintf_s(m_MSGBuffer, sizeof(m_MSGBuffer), "Accept Message\n");
	//m_MSGQueue.push(std::string(m_MSGBuffer));

	// 클라이언트 정보 확인
	int addrLen = sizeof(sockaddr_in) + 16;

	sockaddr_in* pLocal = nullptr;
	sockaddr_in* pRemote = nullptr;

	m_lpfnGetAcceptAddr(
		psOverlapped->szBuffer,
		0,
		sizeof(sockaddr_in) + 16,
		sizeof(sockaddr_in) + 16,
		(sockaddr**)&pLocal,
		&addrLen,
		(sockaddr**)&pRemote,
		&addrLen);

	char buf[32] = { 0, };
	inet_ntop(AF_INET, &pRemote->sin_addr, buf, sizeof(buf));

	// 클라이언트 생성 및 등록
	SSocket* psSocket = new SSocket();
	psSocket->socket = psOverlapped->socket;
	psSocket->strIP = buf;
	psSocket->usPort = ntohs(pRemote->sin_port);

	// IOCP 생성 및 소켓 등록
	CreateIoCompletionPort(reinterpret_cast<HANDLE>(psSocket->socket), m_hIOCP, reinterpret_cast<ULONG_PTR>(psSocket), 0);

	// 수용 인원 초과인지 확인
	if (!AddClientList(psOverlapped->socket))
	{
		ZeroMemory(m_MSGBuffer, sizeof(m_MSGBuffer));
		sprintf_s(m_MSGBuffer, sizeof(m_MSGBuffer), "서버 최대 접속 수 초과\n");
		m_MSGQueue.push(std::string(m_MSGBuffer));

		DoDisconnect(psOverlapped);
		return;
	}

	if (false == DoReceive(psSocket, psOverlapped))
	{
		return;
	}

	ZeroMemory(m_MSGBuffer, sizeof(m_MSGBuffer));
	sprintf_s(m_MSGBuffer, sizeof(m_MSGBuffer), "[%s : %d] 클라이언트 접속\n", psSocket->strIP.c_str(), psSocket->usPort);
	m_MSGQueue.push(std::string(m_MSGBuffer));
}

void MGNetwork::ProcessDisconnect(SOverlapped* psOverlapped)
{
	m_MSGQueue.push(std::string("Disconnect Message\n"));

	RemoveClientList(psOverlapped->socket);
}

void MGNetwork::ProcessRecv(DWORD dwNumberOfBytesTransferred, SOverlapped* psOverlapped, SSocket* psSocket)
{
	//ZeroMemory(m_MSGBuffer, sizeof(m_MSGBuffer));
	//sprintf_s(m_MSGBuffer, sizeof(m_MSGBuffer), "[%s] 패킷 수신 완료 : %d Byte\n", psSocket->strIP.c_str(), dwNumberOfBytesTransferred);
	//m_MSGQueue.push(std::string(m_MSGBuffer));

	psOverlapped->iDataSize += dwNumberOfBytesTransferred;

	while (psOverlapped->iDataSize > 0)
	{
		static const unsigned short cusHeaderSize = 2;

		// HeaderSize 만큼 패킷이 안들어옴
		if (psOverlapped->iDataSize < cusHeaderSize)
		{
			break;
		}

		unsigned short usBodySize = *reinterpret_cast<unsigned short*>(psOverlapped->szBuffer);
		unsigned short usPacketSize = cusHeaderSize + usBodySize;

		// 완전한 패킷이 안들어옴
		if (usPacketSize > psOverlapped->iDataSize)
		{
			break;
		}

		// 버퍼를 복사해서 인터페이스 객체에 넘겨야한다.
		char tempBuffer[1024];
		ZeroMemory(tempBuffer, sizeof(tempBuffer));

		memcpy_s(tempBuffer, psOverlapped->iDataSize, psOverlapped->szBuffer, psOverlapped->iDataSize);

		// 데이터들을 이번에 처리한만큼 당긴다.
		memcpy_s(psOverlapped->szBuffer, psOverlapped->iDataSize,
			psOverlapped->szBuffer + usPacketSize, psOverlapped->iDataSize - usPacketSize);

		SHeader* packet = reinterpret_cast<SHeader*>(psOverlapped->szBuffer);
		if (packet->usType == PacketType::StringPakcet)
		{
			ZeroMemory(m_MSGBuffer, sizeof(m_MSGBuffer));
			sprintf_s(m_MSGBuffer, sizeof(m_MSGBuffer), "[Recv] %s\n", reinterpret_cast<StringPacket*>(packet)->buffer);
			m_MSGQueue.push(std::string(m_MSGBuffer));
		}

		if (nullptr != m_pPacketSystem)
		{
			m_pPacketSystem->Receive(psSocket, tempBuffer);
		}

		// 처리한 패킷 크기만큼 처리할량 감소
		psOverlapped->iDataSize -= usPacketSize;
	}

	// 다시 수신 걸어 두기
	if (DoReceive(psSocket, psOverlapped) == false)
	{
		return;
	}
}

void MGNetwork::ProcessSend(DWORD dwNumberOfBytesTransferred, SOverlapped* psOverlapped, SSocket* psSocket)
{
	//ZeroMemory(m_MSGBuffer, sizeof(m_MSGBuffer));
	//sprintf_s(m_MSGBuffer, sizeof(m_MSGBuffer), "[%s] 패킷 송신 완료 : %d Byte\n", psSocket->strIP.c_str(), dwNumberOfBytesTransferred);
	//m_MSGQueue.push(std::string(m_MSGBuffer));

	SHeader* packet = reinterpret_cast<SHeader*>(psOverlapped->szBuffer);

	if (packet->usType == PacketType::StringPakcet)
	{
		ZeroMemory(m_MSGBuffer, sizeof(m_MSGBuffer));
		sprintf_s(m_MSGBuffer, sizeof(m_MSGBuffer), "[Send] %s\n", reinterpret_cast<StringPacket*>(packet)->buffer);
		m_MSGQueue.push(std::string(m_MSGBuffer));
	}

	//DoSend(psSocket, psOverlapped);

	delete psOverlapped;
}
