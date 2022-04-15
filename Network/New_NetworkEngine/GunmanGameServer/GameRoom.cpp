#include <sstream>
#include "GameServer.h"
#include "GameRoom.h"
#include "Packet.h"

GameRoom::GameRoom(GameServer* gameserver, int roomNumber)
	:m_pGameServer(gameserver), m_myNumber(roomNumber), m_roomLock(0), m_bRoom(true)
{
	m_myThread = new std::thread(&GameRoom::RoomFunction, this);
}

GameRoom::~GameRoom()
{
	m_bRoom = false;
	m_myThread->join();
}

void GameRoom::AddGameMSG(std::string MSG)
{
	while (0 != InterlockedExchange(&m_roomLock, 1))
	{
		Sleep(0);
	}

	m_GameMSGQueue.push(std::string(MSG));

	InterlockedExchange(&m_roomLock, 0);
}

bool GameRoom::IsUserIn(std::string user_id)
{
	bool bResult = false;

	for (int i = 0; i < m_UserVector.size(); ++i)
	{
		if (user_id == m_UserVector[i]->userID)
		{
			bResult = true;
			break;
		}
	}

	return bResult;
}

bool GameRoom::IsUserIn(SOCKET sock, std::string& user_id)
{
	bool bResult = false;

	for (int i = 0; i < m_UserVector.size(); ++i)
	{
		if (sock == m_UserVector[i]->userSock)
		{
			bResult = true;
			user_id = m_UserVector[i]->userID;
			break;
		}
	}

	return bResult;
}

std::string GameRoom::FindUserID(SOCKET sock)
{
	std::string id;

	for (int i = 0; i < m_UserVector.size(); ++i)
	{
		if (sock == m_UserVector[i]->userSock)
		{
			id = m_UserVector[i]->userID;
			break;
		}
	}
	return id;
}

int GameRoom::UserCount()
{
	return m_UserVector.size();
}

void GameRoom::RoomFunction()
{
	while (m_bRoom)
	{
		while (!m_GameMSGQueue.empty())
		{
			while (0 != InterlockedExchange(&m_roomLock, 1))
			{
				Sleep(0);
			}
			std::string msg = m_GameMSGQueue.front();
			m_GameMSGQueue.pop();
			RoomMSGProcess(msg);
			InterlockedExchange(&m_roomLock, 0);
		}
		Sleep(1);
	}
}

void GameRoom::RoomMSGProcess(std::string MSG)
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
	case (int)RoomMSGType::EnterRoom:
	{
		RoomUser* user = new RoomUser();
		user->userSock = atoi(msgVector[1].c_str());
		user->userID = msgVector[2].c_str();

		m_UserVector.push_back(user);

		char buf[64] = { 0, };
		std::string id[2];

		for (int i = 0; i < m_UserVector.size(); i++)
		{
			id[i] = m_UserVector[i]->userID;
		}

		sprintf_s(buf, sizeof(buf), "%d,%s,%s", Type, id[0].c_str(), id[1].c_str());
		BroadcastMSG(std::string(buf));

		m_pGameServer->SendAccessUserList();
		m_pGameServer->SendRoomState();
	}
	break;

	case (int)RoomMSGType::LeaveRoom:
	{
		SOCKET socket;
		std::string id = msgVector[1];
		for (auto iter = m_UserVector.begin(); iter != m_UserVector.end(); iter++)
		{
			if ((*iter)->userID == id)
			{
				RoomUser* pUser = (*iter);
				socket = (*iter)->userSock;
				m_UserVector.erase(iter);
				delete pUser;
				break;
			}
		}

		char buf[64] = { 0, };
		sprintf_s(buf, sizeof(buf), "0,%u,%s", socket, id.c_str());		// 0 = 대기실 입장, 소켓주소, ID
		m_pGameServer->SendMessageToLobby(buf);

		BroadcastMSG(std::string("1"));
	}
	break;

	case (int)RoomMSGType::Shutdown:
	{
		SOCKET socket;
		std::string id = msgVector[1];
		for (auto iter = m_UserVector.begin(); iter != m_UserVector.end(); iter++)
		{
			if ((*iter)->userID == id)
			{
				RoomUser* pUser = (*iter);
				socket = (*iter)->userSock;
				m_UserVector.erase(iter);
				delete pUser;
				m_pGameServer->SendAccessUserList();
				m_pGameServer->SendRoomState();
				break;
			}
		}

		BroadcastMSG(std::string("1"));
	}
	break;

	case (int)RoomMSGType::Ready:
	{
		std::string nowid = msgVector[1];
		for (auto iter = m_UserVector.begin(); iter != m_UserVector.end(); iter++)
		{
			if ((*iter)->userID == nowid)
			{
				(*iter)->readyState = !(*iter)->readyState;
				break;
			}
		}

		char buf[64] = { 0, };

		bool start = true;
		for (int i = 0; i < m_UserVector.size(); i++)
		{
			if (!m_UserVector[i]->readyState)
			{
				start = false;
			}

			ZeroMemory(buf, sizeof(buf));
			sprintf_s(buf, sizeof(buf), "2,%s,%d", m_UserVector[i]->userID.c_str(), m_UserVector[i]->readyState);
			BroadcastMSG(std::string(buf));
		}

		if (start && m_UserVector.size() >= 2)
		{
			srand(GetTickCount64());

			int rndNum = rand() % 50;
			float mWaitTime = (float)rndNum / 10.f;
			mWaitTime += 1.0f; // 기다려야하는 시간 0.5f ~ 3.0f 사이

			ZeroMemory(buf, sizeof(buf));
			sprintf_s(buf, sizeof(buf), "3,%f", mWaitTime);
			BroadcastMSG(std::string(buf));
		}

	}
	break;

	case (int)RoomMSGType::End:
	{
		std::string nowId = msgVector[1];
		float nowTime = atof(msgVector[2].c_str());
		bool isEnd = true;

		for (int i = 0; i < m_UserVector.size(); i++)
		{
			if (m_UserVector[i]->userID == nowId)
			{
				m_UserVector[i]->resultState = true;
				m_UserVector[i]->time = nowTime;
			}

			if (!m_UserVector[i]->resultState)
			{
				isEnd = false;
			}
		}

		if (isEnd)
		{
			char buf[64] = { 0, };
			std::string winUser;

			// 비김
			if (m_UserVector[0]->time == m_UserVector[1]->time)
			{
				winUser = "x";
			}

			// 유저 2 이김
			else if (m_UserVector[0]->time > m_UserVector[1]->time)
			{
				if (m_UserVector[1]->time < 0)
				{
					winUser = m_UserVector[0]->userID;
				}
				else
				{
					winUser = m_UserVector[1]->userID;
				}
			}

			// 유저 1 이김
			else
			{
				if (m_UserVector[0]->time < 0)
				{
					winUser = m_UserVector[1]->userID;
				}
				else
				{
					winUser = m_UserVector[0]->userID;
				}
			}

			sprintf_s(buf, sizeof(buf), "4,%s,%s,%f,%s,%f",
				winUser.c_str(),
				m_UserVector[0]->userID.c_str(), m_UserVector[0]->time,
				m_UserVector[1]->userID.c_str(), m_UserVector[1]->time);
			BroadcastMSG(std::string(buf));

			//전적 업데이트
			for (int i = 0; i < m_UserVector.size(); i++)
			{
				std::string nowID = m_UserVector[i]->userID;

				GameServerProcess* GSP = reinterpret_cast<GameServerProcess*>(m_pGameServer->m_pPacketSystem);

				ZeroMemory(GSP->m_queryBuffer, sizeof(GSP->m_queryBuffer));
				sprintf_s(GSP->m_queryBuffer, sizeof(GSP->m_queryBuffer), "SELECT * FROM userinfo_table WHERE id ='%s'", nowID.c_str());
				mysql_query(GSP->m_pSQL, GSP->m_queryBuffer);

				GSP->m_queryResult = mysql_store_result(GSP->m_pSQL);
				int fieldCount = mysql_num_fields(GSP->m_queryResult);
				GSP->m_queryRow = mysql_fetch_row(GSP->m_queryResult);

				std::istringstream nowInfo(std::string(GSP->m_queryRow[1]));
				std::string Info[3];
				std::string str;
				int index = 0;
				while (std::getline(nowInfo, str, ','))
				{
					Info[index++] = str;
				}

				int win = atoi(Info[0].c_str());
				int lose = atoi(Info[1].c_str());
				int draw = atoi(Info[2].c_str());

				if (winUser == "x")
				{
					draw++;
				}
				else if (winUser == nowID)
				{
					win++;
				}
				else
				{
					lose++;
				}

				char infoBuf[10];
				sprintf_s(infoBuf, sizeof(infoBuf), "%d,%d,%d", win, lose, draw);

				ZeroMemory(GSP->m_queryBuffer, sizeof(GSP->m_queryBuffer));
				sprintf_s(GSP->m_queryBuffer, sizeof(GSP->m_queryBuffer), "UPDATE userinfo_table SET info = '%s' WHERE id = '%s'", infoBuf, nowID.c_str());
				mysql_query(GSP->m_pSQL, GSP->m_queryBuffer);
			}

			for (int i = 0; i < m_UserVector.size(); i++)
			{
				m_UserVector[i]->readyState = false;
				m_UserVector[i]->resultState = false;
				m_UserVector[i]->time = 0;
			}
		}
	}
	break;
	}
}

void GameRoom::BroadcastMSG(std::string MSG)
{
	StringPacket* strPacket = new StringPacket();
	memcpy_s(strPacket->buffer, sizeof(strPacket->buffer), MSG.c_str(), MSG.length());

	for (int i = 0; i < m_UserVector.size(); i++)
	{
		m_pGameServer->SendPakcet(strPacket, m_UserVector[i]->userSock);
	}
}
