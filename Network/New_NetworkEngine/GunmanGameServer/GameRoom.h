#pragma once
#include <queue>
#include <vector>
#include <string>
#include <thread>
#include "MGNetwork.h"

class GameServer;

struct RoomUser
{
	std::string		userID;
	SOCKET			userSock;
	bool			readyState = false;
	float			time;
	bool			resultState = false;
};

class GameRoom
{
public:
	GameRoom(GameServer* gameserver, int roomNumber);
	~GameRoom();

public:
	void			AddGameMSG(std::string MSG);
	
	bool			IsUserIn(std::string user_id);
	bool			IsUserIn(SOCKET sock, std::string& user_id);
	std::string		FindUserID(SOCKET sock);

	int				UserCount();

private:
	void			RoomFunction();
	void			RoomMSGProcess(std::string MSG);
	void			BroadcastMSG(std::string MSG);

public:
	std::queue<std::string>		m_GameMSGQueue;
	std::vector<RoomUser*>		m_UserVector;
	std::thread*				m_myThread;

private:
	GameServer*		m_pGameServer;		// 게임 서버 포인터
	int				m_myNumber;			// 내 방 번호
	unsigned int	m_roomLock;			// 룸 스레드 락
	bool			m_bRoom;
};

