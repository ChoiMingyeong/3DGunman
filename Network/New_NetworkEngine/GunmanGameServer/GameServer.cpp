#include <sstream>
#include <iostream>

#include "Packet.h"
#include "GameServer.h"
#include "GameRoom.h"

GameServerProcess::GameServerProcess(const char* host, const char* user, const char* passwd, const char* db, unsigned int port)
{
	MYSQL* connectResult;
	unsigned int timeoutSec = 1;
	m_pSQL = mysql_init(NULL);
	mysql_options(m_pSQL, MYSQL_OPT_CONNECT_TIMEOUT, &timeoutSec);
	connectResult = mysql_real_connect(m_pSQL, host, user, passwd, db, port, NULL, 0);

	ZeroMemory(m_queryBuffer, sizeof(m_queryBuffer));
}

GameServerProcess::~GameServerProcess()
{
}

void GameServerProcess::Receive(SSocket* psSocket, char* buffer)
{
	SHeader* packet = reinterpret_cast<SHeader*>(buffer);
	char buf[64] = { 0, };

	switch (packet->usType)
	{
	case PacketType::StringPakcet:
	{
		StringPacket* stringPacket = reinterpret_cast<StringPacket*>(packet);
		std::istringstream ss(stringPacket->buffer);
		std::string str;
		int i = 0;
		int bigType, smallType;		// 메세지 분류용
		std::string msg;
		while (std::getline(ss, str, ','))
		{
			if (i == 0)
			{
				bigType = atoi(str.c_str());
			}
			else if (i == 1)
			{
				smallType = atoi(str.c_str());
			}
			else if (i == 2)
			{
				msg = str;			// 추가 메세지
			}

			i++;
		}

		switch (bigType)
		{
		case (int)GameMSGType::Lobby:
		{
			switch (smallType)
			{
			case (int)LobbyMSGType::EnterLobby:
			{
				// 로비 입장할 때는 str은 ID로 들어온다.
				sprintf_s(buf, sizeof(buf), "0,%u,%s", psSocket->socket, str.c_str());		// 0 = 대기실 입장, 소켓주소, ID
				m_pGameServer->SendMessageToLobby(buf);
			}
			break;

			case (int)LobbyMSGType::LeaveLobby:
			{
				// 로비 퇴장할 때 str은 들어오지 않는다.
				sprintf_s(buf, sizeof(buf), "1,%s", m_pGameServer->FindLobbyUserID(psSocket->socket).c_str());		// 1 = 대기실 퇴장, 소켓주소, ID
				m_pGameServer->SendMessageToLobby(buf);
			}
			break;

			case (int)LobbyMSGType::EnterRoom:
			{
				// 방에 입장할 때는 str은 방번호로 들어온다.
				int roomNumber = atoi(str.c_str());

				std::string id = m_pGameServer->FindLobbyUserID(psSocket->socket);
				sprintf_s(buf, sizeof(buf), "1,%s", id.c_str());		// 1 = 대기실 퇴장, ID
				m_pGameServer->SendMessageToLobby(buf);

				sprintf_s(buf, sizeof(buf), "0,%u,%s", psSocket->socket, id.c_str());		// 1 = 게임방 입장, 소켓주소, ID
				m_pGameServer->SendMessageToGameRoom(roomNumber, buf);				//
			}
			break;

			//case (int)LobbyMSGType::LeaveRoom:
			//{
			//	// 방에서 퇴장할때는 str은 ID로 들어온다.
			//	sprintf_s(buf, sizeof(buf), "1,%s", str.c_str());					// 1 = 게임방 퇴장, ID
			//	m_pGameServer->SendMessageToGameRoom(m_pGameServer->FindRoomNumber(str), buf);

			//	sprintf_s(buf, sizeof(buf), "0,%u,%s", psSocket->socket, str.c_str());		// 0 = 대기실 입장, 소켓주소, ID
			//	m_pGameServer->SendMessageToLobby(buf);
			//}
			//break;

			// 중복 로그인 테스트
			case (int)LobbyMSGType::Test:
			{
				bool bLoginCheck = true;

				// 이미 로그인 되어있는 아이디인지 확인
				for (auto user : m_pGameServer->m_LobbyUserList)
				{
					if (user->userID == str)
					{
						bLoginCheck = false;
						break;
					}
				}

				// 방에 있는지 확인
				for (auto room : m_pGameServer->m_pGameRoomPool)
				{
					if (room->IsUserIn(str))
					{
						bLoginCheck = false;
						break;
					}
				}

				StringPacket* loginCheckPacket = new StringPacket();
				sprintf_s(buf, sizeof(buf), "1,%d", (bLoginCheck) ? 1 : -3);
				memcpy_s(loginCheckPacket->buffer, sizeof(loginCheckPacket->buffer), buf, sizeof(buf));

				m_pGameServer->SendPakcet(loginCheckPacket, psSocket->socket);
			}
			break;
			}
		}
		break;

		case (int)GameMSGType::Room:
		{
			switch (smallType)
			{
				/*case (int)RoomMSGType::EnterRoom:
				{

				}
				break;*/

			case (int)RoomMSGType::LeaveRoom:
			{
				int roomNumber = atoi(str.c_str());
				std::string id = m_pGameServer->m_pGameRoomPool[roomNumber]->FindUserID(psSocket->socket);
				sprintf_s(buf, sizeof(buf), "%d,%s", smallType, id.c_str());					// 1 = 게임방 퇴장, ID
				m_pGameServer->SendMessageToGameRoom(roomNumber, buf);
			}
			break;

			case (int)RoomMSGType::Ready:
			{
				std::string id;
				int roomNumber = m_pGameServer->FindRoomNumber(psSocket->socket, id);
				sprintf_s(buf, sizeof(buf), "%d,%s", smallType, id.c_str());					// 1 = 게임방 퇴장, ID
				m_pGameServer->SendMessageToGameRoom(roomNumber, buf);
			}
			break;

			case (int)RoomMSGType::End:
			{
				float time = atof(str.c_str());
				std::string id;
				int roomNumber = m_pGameServer->FindRoomNumber(psSocket->socket, id);
				sprintf_s(buf, sizeof(buf), "%d,%s,%f", smallType, id.c_str(), time);					// 1 = 게임방 퇴장, ID
				m_pGameServer->SendMessageToGameRoom(roomNumber, buf);
			}
			break;
			}
		}
		break;
		}
	}
	break;
	}
}

void GameServerProcess::RemoveClient(SOCKET socket)
{
	for (int i = 0; i < m_pGameServer->m_pGameRoomPool.size(); i++)
	{
		std::string id;
		if (m_pGameServer->m_pGameRoomPool[i]->IsUserIn(socket, id))
		{
			char buf[64] = { 0, };
			sprintf_s(buf, sizeof(buf), "%d,%s", (int)RoomMSGType::Shutdown, id.c_str());		// 1 = 대기실 퇴장, 소켓주소, ID
			m_pGameServer->SendMessageToGameRoom(i, buf);

			return;
		}
	}

	for (auto iter = m_pGameServer->m_LobbyUserList.begin(); iter != m_pGameServer->m_LobbyUserList.end(); iter++)
	{
		if (socket == (*iter)->userSocket)
		{
			char buf[64] = { 0, };
			sprintf_s(buf, sizeof(buf), "1,%s", (*iter)->userID.c_str());		// 1 = 대기실 퇴장, 소켓주소, ID
			m_pGameServer->SendMessageToLobby(buf);

			return;
		}
	}
}

GameServer::GameServer(const char* host, const char* user, const char* passwd, const char* db, unsigned int port)
	:m_LobbyLock(0), m_bLobby(true)
{
	OpenServer(9000, 100);
	GameServerProcess* tempProcess = new GameServerProcess(host, user, passwd, db, port);
	tempProcess->m_pGameServer = this;

	m_pPacketSystem = tempProcess;

	// 방 50개 만들기
	for (int i = 0; i < 50; ++i)
	{
		m_pGameRoomPool.push_back(new GameRoom(this, i));
	}

	m_LobbyThread = new std::thread(&GameServer::LobbyFunction, this);
}

GameServer::~GameServer()
{
	for (int i = 0; i < 50; ++i)
	{
		delete m_pGameRoomPool[i];
		m_pGameRoomPool[i] = nullptr;
	}
	m_pGameRoomPool.clear();

	m_bLobby = false;
	m_LobbyThread->join();

	delete m_pPacketSystem;
	m_pPacketSystem = nullptr;
}

void GameServer::SendMessageToGameRoom(int room_number, const char* MSG)
{
	if (room_number < 0 || room_number >= m_pGameRoomPool.size())
	{
		return;
	}

	m_pGameRoomPool[room_number]->AddGameMSG(std::string(MSG));
}

void GameServer::SendMessageToLobby(const char* MSG)
{
	while (0 != InterlockedExchange(&m_LobbyLock, 1))
	{
		Sleep(0);
	}

	m_LobbyMSGQueue.push(std::string(MSG));

	InterlockedExchange(&m_LobbyLock, 0);
}

std::string GameServer::FindLobbyUserID(SOCKET sock)
{
	for (auto user : m_LobbyUserList)
	{
		if (sock == user->userSocket)
		{
			return user->userID;
		}
	}

	return std::string();
}

int GameServer::FindRoomNumber(std::string user_id)
{
	int roomNumber = 0;
	for (; roomNumber < m_pGameRoomPool.size(); roomNumber++)
	{
		// 찾았다!
		if (m_pGameRoomPool[roomNumber]->IsUserIn(user_id))
		{
			break;
		}
	}

	return roomNumber;
}

int GameServer::FindRoomNumber(SOCKET sock, std::string& user_id)
{
	int roomNumber = 0;
	for (; roomNumber < m_pGameRoomPool.size(); roomNumber++)
	{
		// 찾았다!
		if (m_pGameRoomPool[roomNumber]->IsUserIn(sock,user_id))
		{
			break;
		}
	}

	return roomNumber;
}

void GameServer::LobbyFunction()
{
	while (m_bLobby)
	{
		while (!m_LobbyMSGQueue.empty())
		{
			while (0 != InterlockedExchange(&m_LobbyLock, 1))
			{
				Sleep(0);
			}
			std::string msg = m_LobbyMSGQueue.front();
			m_LobbyMSGQueue.pop();
			LobbyMSGProcess(msg);
			InterlockedExchange(&m_LobbyLock, 0);
		}
		Sleep(0);
	}
}

void GameServer::LobbyMSGProcess(std::string MSG)
{
	std::istringstream ss(MSG);
	std::string str;
	std::vector<std::string> msgVector;
	while (std::getline(ss, str, ','))
	{
		msgVector.push_back(str);
	}

	int Type = atoi(msgVector[0].c_str());
	switch (Type)
	{
	case (int)LobbyMSGType::EnterLobby:
	{
		User* tempUser = new User();
		tempUser->userSocket = atol(msgVector[1].c_str());
		tempUser->userID = msgVector[2];
		m_LobbyUserList.push_back(tempUser);

		SendAccessUserList();
		SendRoomState();
	}
	break;

	case (int)LobbyMSGType::LeaveLobby:
	{
		std::string id = msgVector[1];
		for (auto iter = m_LobbyUserList.begin(); iter != m_LobbyUserList.end(); iter++)
		{
			if (id == (*iter)->userID)
			{
				m_LobbyUserList.erase(iter);
				break;
			}
		}
		
		SendAccessUserList();
		SendRoomState();

		//SendAccessUserList();
	}
	break;

	case (int)LobbyMSGType::EnterRoom:
	{
		SendAccessUserList();
		SendRoomState();
	}
	break;

	/*case (int)LobbyMSGType::LeaveRoom:
	{
		SendAccessUserList();
		SendRoomState();
	}
	break;*/
	}
}

void GameServer::BroadcastLobby(std::string MSG)
{
	StringPacket* strPacket = new StringPacket();
	sprintf_s(strPacket->buffer, sizeof(strPacket->buffer), MSG.c_str());
	for (int i = 0; i < m_LobbyUserList.size(); i++)
	{
		SendPakcet(strPacket, m_LobbyUserList[i]->userSocket);
	}
}

void GameServer::SendAccessUserList()
{
	if (!m_LobbyUserList.empty())
	{
		int num = 0;
		std::string userData[2];

		for (int i = 0; i < 100; i++)
		{
			if (m_LobbyUserList.size() - 1 < i)
			{
				userData[(num++) % 2] = std::string();
			}
			else
			{
				userData[(num++) % 2] = m_LobbyUserList[i]->userID;
			}

			if (num % 2 == 0)
			{
				char buf[64] = { 0, };
				sprintf_s(buf, sizeof(buf), "5,%d,%s,%s", (num / 2) - 1, userData[0].c_str(), userData[1].c_str());
				BroadcastLobby(std::string(buf));
			}
		}
	}
}

void GameServer::SendRoomState()
{
	char buf[64] = { 0, };
	sprintf_s(buf, sizeof(buf), "6,");
	int bufPoint = strlen(buf);

	for (int i = 0; i < m_pGameRoomPool.size(); i++)
	{
		buf[bufPoint++] = m_pGameRoomPool[i]->UserCount() + '0';
	}

	BroadcastLobby(std::string(buf));
}
