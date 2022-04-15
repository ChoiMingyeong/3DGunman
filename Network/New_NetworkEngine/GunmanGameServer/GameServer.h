#pragma once

#include <mysql\mysql.h>
#include <list>
#include <vector>
#include "MGNetwork.h"
#include "safePool.h"
#include "GunmanServerEnum.h"
#include <WinDNS.h>

// ���� ����
class GameRoom;
class GameServer;

struct User
{
	std::string		userID;
	SOCKET			userSocket;
};

class GameServerProcess : public ProcessPacket
{
public:
	GameServerProcess(const char* host, const char* user, const char* passwd, const char* db, unsigned int port);
	~GameServerProcess();

public:
	virtual void Receive(SSocket* psSocket, char* buffer);
	virtual void RemoveClient(SOCKET socket);

public:
	MYSQL*			m_pSQL;					// ����� DB
	char			m_queryBuffer[1024];	// �������� ���� ����
	MYSQL_RES*		m_queryResult;			// ������ ���
	MYSQL_ROW		m_queryRow;				// ���� row
	int				m_fieldNum;				// �ʵ� ����

	GameServer*		m_pGameServer;			// ���� ���� ���� ������
};

class GameServer : public MGNetwork
{
public:
	GameServer(const char* host, const char* user, const char* passwd, const char* db, unsigned int port);
	~GameServer();

public:
	void SendMessageToGameRoom(int room_number, const char* MSG);
	void SendMessageToLobby(const char* MSG);

public:
	std::string		FindLobbyUserID(SOCKET sock);
	int				FindRoomNumber(std::string user_id);
	int				FindRoomNumber(SOCKET sock, std::string& user_id);

	void			SendAccessUserList();
	void			SendRoomState();
private:
	void			LobbyFunction();

	void			LobbyMSGProcess(std::string MSG);

	void			BroadcastLobby(std::string MSG);

public:
	std::vector<GameRoom*>		m_pGameRoomPool;	// ���ӹ� Ǯ
	std::vector<User*>			m_LobbyUserList;	// �������� ���� ����Ʈ
	std::queue<std::string>		m_LobbyMSGQueue;	// �κ񿡼� ����� �޼��� ť
	unsigned int				m_LobbyLock;
	bool						m_bLobby;
	std::thread*				m_LobbyThread;
};

