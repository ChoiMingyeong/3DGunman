#pragma once

#include <mysql\mysql.h>
#include <string>
#include "MGNetwork.h"

class LogIn;

class LogInProcess : public ProcessPacket
{
public:
	LogInProcess(const char* host, const char* user, const char* passwd, const char* db, unsigned int port);
	~LogInProcess();

public:
	virtual void Receive(SSocket* psSocket, char* buffer);
	
	BOOL SignIn(const char* id, const char* passwd, SSocket* psSocket);				// ȸ������ �Ϸ��� �Լ�
	BOOL RequestLogIn(const char* id, const char* passwd, SSocket* psSocket);		// �α��� �õ��ϴ� �Լ�

public:
	MYSQL* m_pSQL;						// ����� DB
	char			m_queryBuffer[1024];		// �������� ���� ����
	MYSQL_RES* m_queryResult;				// ������ ���
	MYSQL_ROW		m_queryRow;					// ���� row
	int				m_fieldNum;					// �ʵ� ����

	LogIn* m_pLogInServer;
};

class LogIn : public MGNetwork
{
public:
	LogIn(const char* host, const char* user, const char* passwd, const char* db, unsigned int port);
	~LogIn();
};