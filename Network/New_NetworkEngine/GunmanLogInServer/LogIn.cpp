#include <sstream>
#include <iostream>
#include "Packet.h"
#include "LogIn.h"

using namespace std;

LogIn::LogIn(const char* host, const char* user, const char* passwd, const char* db, unsigned int port)
{
	OpenServer(8110, 100);
	LogInProcess* tempProcess = new LogInProcess(host, user, passwd, db, port);
	tempProcess->m_pLogInServer = this;

	m_pPacketSystem = tempProcess;
	if (nullptr == m_pPacketSystem)
	{
		// �α��� ���� ó�� ���μ��� ���� ����
		return;
	}
}

LogIn::~LogIn()
{
	delete m_pPacketSystem;
	m_pPacketSystem = nullptr;
}

LogInProcess::LogInProcess(const char* host, const char* user, const char* passwd, const char* db, unsigned int port)
	: m_pSQL(nullptr), m_queryRow(nullptr), m_queryResult(nullptr)
{
	MYSQL* connectResult;
	unsigned int timeoutSec = 1;
	m_pSQL = mysql_init(NULL);
	mysql_options(m_pSQL, MYSQL_OPT_CONNECT_TIMEOUT, &timeoutSec);
	connectResult = mysql_real_connect(m_pSQL, host, user, passwd, db, port, NULL, 0);

	ZeroMemory(m_queryBuffer, sizeof(m_queryBuffer));

	if (NULL == connectResult)
	{
		// DB Connection Fail
		//cout << "DB Connection Fail" << endl;
		return;
	}
	else
	{
		// DB Connection Success
		//cout << "DB Connection Success" << endl;
	}
}

LogInProcess::~LogInProcess()
{
	if (nullptr != m_queryResult)
	{
		mysql_free_result(m_queryResult);
	}

	if (nullptr != m_pSQL)
	{
		mysql_close(m_pSQL);
	}
}

void LogInProcess::Receive(SSocket* psSocket, char* buffer)
{
	SHeader* packet = reinterpret_cast<SHeader*>(buffer);

	switch (packet->usType)
	{
	case PacketType::StringPakcet:
	{
		StringPacket* stringPacket = reinterpret_cast<StringPacket*>(packet);
		istringstream ss(stringPacket->buffer);
		int type = -1;
		string id, passwd;
		int i = 0;
		string str;
		while (getline(ss, str, ','))
		{
			if (i == 0)
			{
				type = atoi(str.c_str());
			}
			else if (i == 1)
			{
				id = str;
			}
			else if (i == 2)
			{
				passwd = str;
			}

			i++;
		}

		//ss >> type >> id >> passwd;

		switch (type)
		{
		case 0:
		{
			//printf("[ȸ������ �õ�] id = %s, password = %s\n", id.c_str(), passwd.c_str());

			BOOL resultSignIn = SignIn(id.c_str(), passwd.c_str(), psSocket);
			char buffer[64] = { 0, };
			Packet* packet = new Packet();
			packet->usType = PacketType::StringPakcet;

			/// [ resultSignIn �޼��� ó��]
			/// 1 = ȸ������ ����
			/// -1 = �̹� �����ϴ� ���̵�
			/// 0 = ȸ������ ����

			//if (TRUE == resultSignIn)
			//{
			//	sprintf_s(buffer, sizeof(buffer), "%d,id=%s %s", resultSignIn, id.c_str(), "ȸ�� ���Կ� �����߽��ϴ�.");
			//}
			//else if (-1 == resultSignIn)
			//{
			//	sprintf_s(buffer, sizeof(buffer), "%d,id=%s %s", resultSignIn, id.c_str(), "�̹� �����ϴ� ���̵� �Դϴ�.");

			//}
			//else
			//{
			//	sprintf_s(buffer, sizeof(buffer), "%d,id=%s %s", resultSignIn, id.c_str(), "ȸ�� ���Կ� �����Ͽ����ϴ�.");
			//}

			sprintf_s(buffer, sizeof(buffer), "0,%d", resultSignIn);
			memcpy_s(packet->buffer, sizeof(packet->buffer), buffer, sizeof(buffer));
			m_pLogInServer->SendPakcet(packet, psSocket->socket);

			//delete packet;
		}
		break;

		case 1:
		{
			//printf("[�α��� �õ�] id = %s, password = %s\n", id.c_str(), passwd.c_str());

			BOOL resultLogIn = RequestLogIn(id.c_str(), passwd.c_str(), psSocket);
			char buffer[64] = { 0, };
			StringPacket* packet = new StringPacket();
			packet->usType = PacketType::StringPakcet;

			/// [ resultLogIn �޼��� ó�� ]
			/// 1 = �α��� ����
			/// -1 = ���̵� �˻� ����(����x)
			/// -2 = ��й�ȣ Ʋ��
			/// 0 = �α��� ����(��ư ����)

			if (true == resultLogIn)
			{
				sprintf_s(buffer, sizeof(buffer), "1,%d", resultLogIn);
			}
			else if (-1 == resultLogIn)
			{
				sprintf_s(buffer, sizeof(buffer), "1,%d", resultLogIn);
			}
			else if (-2 == resultLogIn)
			{
				sprintf_s(buffer, sizeof(buffer), "1,%d", resultLogIn);
			}
			else
			{
				sprintf_s(buffer, sizeof(buffer), "1,%d", resultLogIn);
			}

			//sprintf_s(buffer, sizeof(buffer), "1,%d", resultLogIn);
			memcpy_s(packet->buffer, sizeof(packet->buffer), buffer, sizeof(buffer));
			m_pLogInServer->SendPakcet(packet, psSocket->socket);

			//delete packet;
		}
		break;

		// �α��� ��¥ ����
		case 2:
		{
			// ���� ���� ������ ���̵� ������ ������Ʈ
			char buffer[64] = { 0, };
			ZeroMemory(m_queryBuffer, sizeof(m_queryBuffer));
			sprintf_s(m_queryBuffer, sizeof(m_queryBuffer), "UPDATE userid_table SET ip = '%s', port = '%d' WHERE id = '%s'", psSocket->strIP.c_str(), psSocket->usPort, id.c_str());
			mysql_query(m_pSQL, m_queryBuffer);

			// ���� ������ ���̵� ���� ��� �����ֱ�
			ZeroMemory(m_queryBuffer, sizeof(m_queryBuffer));
			sprintf_s(m_queryBuffer, sizeof(m_queryBuffer), "SELECT * FROM userinfo_table WHERE id ='%s'", id.c_str());
			mysql_query(m_pSQL, m_queryBuffer);

			m_queryResult = mysql_store_result(m_pSQL);
			int fieldCount = mysql_num_fields(m_queryResult);
			m_queryRow = mysql_fetch_row(m_queryResult);

			StringPacket* strPacket = new StringPacket();
			sprintf_s(buffer, sizeof(buffer), "2,1,%s", m_queryRow[1]);
			memcpy_s(strPacket->buffer, sizeof(strPacket->buffer), buffer, sizeof(buffer));
			m_pLogInServer->SendPakcet(strPacket, psSocket->socket);
		}
		}
	}
	break;
	}

}

//void LogInProcess::Disconnect(SOCKET sock)
//{
//	sockaddr_in* sockAddr = nullptr;
//	int len = 0;
//	char buf[32] = { 0, };
//	getpeername(sock, (sockaddr*)sockAddr, &len);
//	inet_ntop(AF_INET, &sockAddr->sin_addr, buf, sizeof(buf));
//
//	ZeroMemory(m_queryBuffer, sizeof(m_queryBuffer));
//	sprintf_s(m_queryBuffer, sizeof(m_queryBuffer), "UPDATE userid_table SET ip = '%s' WHERE ip = '%s'", "", buf);
//	if (mysql_query(m_pSQL, m_queryBuffer))
//	{
//
//	}
//}

BOOL LogInProcess::SignIn(const char* id, const char* passwd, SSocket* psSocket)
{
	// ���̵� ������̸� ����
	if (id == "")
	{
		return FALSE;
	}

	ZeroMemory(m_queryBuffer, sizeof(m_queryBuffer));
	sprintf_s(m_queryBuffer, sizeof(m_queryBuffer), "INSERT INTO userid_table(id, password, ip, port) VALUES('%s','%s','%s','%d')", id, passwd, psSocket->strIP.c_str(), psSocket->usPort);
	if (mysql_query(m_pSQL, m_queryBuffer))
	{
		// insert�� ���������� �̹� �ִ� ���̵����� Ȯ��
		ZeroMemory(m_queryBuffer, sizeof(m_queryBuffer));
		sprintf_s(m_queryBuffer, sizeof(m_queryBuffer), "SELECT * FROM userid_table WHERE id ='%s'", id);

		if (mysql_query(m_pSQL, m_queryBuffer))
		{
			return FALSE;
		}
		else
		{
			// �̹� �ִ� ���̵�
			return -1;
		}
	}
	else
	{
		// ���� ����
		ZeroMemory(m_queryBuffer, sizeof(m_queryBuffer));
		sprintf_s(m_queryBuffer, sizeof(m_queryBuffer), "INSERT INTO userinfo_table(id, info) VALUES('%s','%s')", id, "0,0,0");

		if (mysql_query(m_pSQL, m_queryBuffer))
		{
			return FALSE;
		}
		return TRUE;
	}
}

BOOL LogInProcess::RequestLogIn(const char* id, const char* passwd, SSocket* psSocket)
{
	// �����ϴ� ���̵����� Ȯ��
	ZeroMemory(m_queryBuffer, sizeof(m_queryBuffer));
	sprintf_s(m_queryBuffer, sizeof(m_queryBuffer), "SELECT * FROM userid_table WHERE id ='%s'", id);
	if (mysql_query(m_pSQL, m_queryBuffer))
	{
		return -1;
	}
	// �̹� �����ϴ� ���̵�� ��й�ȣ Ȯ��
	else
	{
		m_queryResult = mysql_store_result(m_pSQL);
		int fieldCount = mysql_num_fields(m_queryResult);
		m_queryRow = mysql_fetch_row(m_queryResult);

		if (nullptr == m_queryRow)
		{
			return FALSE;
		}

		// ��й�ȣ���� ������ �α��� ����
		if (0 == strcmp(m_queryRow[1], passwd))
		{
			return TRUE;
		}
		else
		{
			return -2;
		}
	}
}
