#pragma once

#include <mysql\mysql.h>
#include <list>
#include <vector>
#include "MGNetwork.h"
#include "safePool.h"
#include "GunmanServerEnum.h"
#include <WinDNS.h>

// 전방 선언
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
	MYSQL*			m_pSQL;					// 연결된 DB
	char			m_queryBuffer[1024];	// 쿼리문에 사용될 버퍼
	MYSQL_RES*		m_queryResult;			// 쿼리문 결과
	MYSQL_ROW		m_queryRow;				// 쿼리 row
	int				m_fieldNum;				// 필드 개수

	GameServer*		m_pGameServer;			// 현재 게임 서버 포인터
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
	std::vector<GameRoom*>		m_pGameRoomPool;	// 게임방 풀
	std::vector<User*>			m_LobbyUserList;	// 접속중인 유저 리스트
	std::queue<std::string>		m_LobbyMSGQueue;	// 로비에서 사용할 메세지 큐
	unsigned int				m_LobbyLock;
	bool						m_bLobby;
	std::thread*				m_LobbyThread;
};

