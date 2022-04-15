#include <thread>
#include "safe_pool.h"
#include "SSocket.h"
#include "NetworkEngine.h"

NetworkEngine::NetworkEngine()
	:m_ConnectEx(nullptr), m_DisconnectEx(nullptr), m_AcceptEx(nullptr), m_hIOCP(nullptr), m_ListenSocket(INVALID_SOCKET)
{
	// ���� ����
	if (WSAStartup(MAKEWORD(2, 2), &m_wsaData) != 0)
	{
		// ���� �߻� WSAStartup
		return;
	}

	// IOCP �ڵ��� �����Ѵ�.
	m_hIOCP = CreateIoCompletionPort(INVALID_HANDLE_VALUE, nullptr, 0, 0);

#pragma region �۾� ������ ������
	// �ý��� ���� ȹ��
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

#pragma region Accept �������� �ɱ�
	safe_pool<SOverlapped*> sOverlappedPool = safe_pool<SOverlapped*>(m_HeadCount);
	for (int i = 0; i < sOverlappedPool.count(); i++)
	{
		auto tempElement = sOverlappedPool[i].value();
		if (INVALID_SOCKET == tempElement->socket)
		{
			tempElement->socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
		}
		tempElement->eIOType = SOverlapped::EIOType::Accept;

		// AcceptEx �Լ� ����
		if (!m_AcceptEx(
			m_ListenSocket, tempElement->socket,
			&tempElement->szBuffer, 0,
			sizeof(SOCKADDR_IN) + 16, sizeof(SOCKADDR_IN) + 16,
			&dwByte, &tempElement->wsaOverlapped)
			 && WSAGetLastError() != ERROR_IO_PENDING)
		{
			// m_AcceptEx �Լ� ����
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
	// ���ε�
	if (!BindListenSocket(port))
	{
		return false;
	}

	// ������ ������ �� �ʿ��� ex �Լ��� �����´�.
	if (!GetOpenServerExFunction())
	{
		closesocket(m_ListenSocket);
		m_ListenSocket = INVALID_SOCKET;
		return false;
	}

	// �ο� ���� �� ��ŭ Accept �ɱ�
	if (!DoAccept())
	{
		closesocket(m_ListenSocket);
		m_ListenSocket = INVALID_SOCKET;
		return false;
	}
}

bool NetworkEngine::ConnectServer(std::string name, unsigned short port, std::string ip)
{
	// ������ ������ �� �ʿ��� Ex �Լ��� �����´�.
	if (!GetConnectServerExFunction())
	{
		return false;
	}

	// ���� ����
	SOCKET sock = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, nullptr, 0, WSA_FLAG_OVERLAPPED);

	// ������ Connect
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
	// �̹� Ex �Լ��� �ҷ����� �ִٸ� ����
	if (nullptr != m_DisconnectEx && nullptr != m_AcceptEx)
	{
		return true;
	}

	DWORD dwByte;
	GUID GuidDisconnectEx = WSAID_DISCONNECTEX;
	GUID GuidAcceptEx = WSAID_ACCEPTEX;

	// DisconnectEx �Լ� ������ ȹ�� �� �޸𸮿� ���
	if (SOCKET_ERROR == WSAIoctl(
		m_ListenSocket, SIO_GET_EXTENSION_FUNCTION_POINTER,
		&GuidDisconnectEx, sizeof(GuidDisconnectEx),
		&m_DisconnectEx, sizeof(m_DisconnectEx),
		&dwByte, NULL, NULL))
	{
		// DisconnectEx ���� ����
		return false;
	}

	// AcceptEx �Լ� ������ ȹ��
	if (SOCKET_ERROR == WSAIoctl(
		m_ListenSocket, SIO_GET_EXTENSION_FUNCTION_POINTER,
		&GuidAcceptEx, sizeof(GuidAcceptEx),
		&m_AcceptEx, sizeof(m_AcceptEx),
		&dwByte, NULL, NULL))
	{
		// AcceptEx ���� ����
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

	// DisconnectEx �Լ� ������ ȹ�� �� �޸𸮿� ���
	if (SOCKET_ERROR == WSAIoctl(
		m_ListenSocket, SIO_GET_EXTENSION_FUNCTION_POINTER,
		&GuidDisconnectEx, sizeof(GuidDisconnectEx),
		&m_DisconnectEx, sizeof(m_DisconnectEx),
		&dwByte, NULL, NULL))
	{
		// DisconnectEx ���� ����
		closesocket(m_ListenSocket);
		WSACleanup();
	}

	// AcceptEx �Լ� ������ ȹ��
	if (SOCKET_ERROR == WSAIoctl(
		m_ListenSocket, SIO_GET_EXTENSION_FUNCTION_POINTER,
		&GuidAcceptEx, sizeof(GuidAcceptEx),
		&m_AcceptEx, sizeof(m_AcceptEx),
		&dwByte, NULL, NULL))
	{
		// AcceptEx ���� ����
		closesocket(m_ListenSocket);
		WSACleanup();
	}

	// AcceptEx �Լ� ������ ȹ��
	if (SOCKET_ERROR == WSAIoctl(
		m_ListenSocket, SIO_GET_EXTENSION_FUNCTION_POINTER,
		&GuidConnectEx, sizeof(GuidConnectEx),
		&m_ConnectEx, sizeof(m_ConnectEx),
		&dwByte, NULL, NULL))
	{
		// AcceptEx ���� ����
		closesocket(m_ListenSocket);
		WSACleanup();
	}

	return false;
}

bool NetworkEngine::BindListenSocket(unsigned short port)
{
	// �̹� ���� ������ �����ϴ� ���( ������ �̹� ���� ��Ȳ ) ����
	if (INVALID_SOCKET != m_ListenSocket)
	{
		return false;
	}

	// ���� ���� ����
	m_ListenSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (m_ListenSocket == INVALID_SOCKET)
	{
		// ���� �߻� WSASocket
		return false;
	}

	// IOCP �ڵ��� ���ٸ� �����Ѵ�.
	if (nullptr == m_hIOCP)
	{
		m_hIOCP = CreateIoCompletionPort(INVALID_HANDLE_VALUE, nullptr, 0, 0);
	}

	// ���� ������ IOCP�� ����
	CreateIoCompletionPort(reinterpret_cast<HANDLE>(m_ListenSocket), m_hIOCP, 0, 0);

	// ���� �ּҿ� ���� ������ ����
	SOCKADDR_IN serverAddr;
	ZeroMemory(&serverAddr, sizeof(serverAddr));
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);
	serverAddr.sin_port = htons(port);

	// ���ε�
	if (SOCKET_ERROR == bind(m_ListenSocket, reinterpret_cast<SOCKADDR*>(&serverAddr), sizeof(serverAddr)))
	{
		// ���� �߻� bind()
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

		// �ӽ� SOverlapped, SSocket
		SOverlapped* psOverlapped = nullptr;
		SSocket* psSocket = nullptr;

		OVERLAPPED_ENTRY entries[32];
		ULONG numEnries = 0;			// ������ Entries�� ��

		if (FALSE == GetQueuedCompletionStatusEx(m_hIOCP, entries, 32, &numEnries, INFINITE, FALSE))
		{
			// �������� ����
			continue;
		}

		// ������ �� ��ŭ �ݺ�
		for (ULONG i = 0; i < numEnries; ++i)
		{
			dwNumberOfBytesTransferred = entries[i].dwNumberOfBytesTransferred;

			psOverlapped = reinterpret_cast<SOverlapped*>(entries[i].lpOverlapped);
			if (psOverlapped == nullptr)
			{
				continue;
			}

			// Accept�� ���
			if (psOverlapped->eIOType == SOverlapped::EIOType::Accept)
			{
				/**/
				continue;
			}

			// Disconnect�� ���
			if (psOverlapped->eIOType == SOverlapped::EIOType::Disconnect)
			{
				/**/
				continue;
			}

			if (dwNumberOfBytesTransferred == 0)
			{
				// Ŭ���̾�Ʈ�� �������� �� Disconnect
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
