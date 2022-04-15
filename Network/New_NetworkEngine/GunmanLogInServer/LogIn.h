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
	
	BOOL SignIn(const char* id, const char* passwd, SSocket* psSocket);				// 회원가입 하려는 함수
	BOOL RequestLogIn(const char* id, const char* passwd, SSocket* psSocket);		// 로그인 시도하는 함수

public:
	MYSQL* m_pSQL;						// 연결된 DB
	char			m_queryBuffer[1024];		// 쿼리문에 사용될 버퍼
	MYSQL_RES* m_queryResult;				// 쿼리문 결과
	MYSQL_ROW		m_queryRow;					// 쿼리 row
	int				m_fieldNum;					// 필드 개수

	LogIn* m_pLogInServer;
};

class LogIn : public MGNetwork
{
public:
	LogIn(const char* host, const char* user, const char* passwd, const char* db, unsigned int port);
	~LogIn();
};