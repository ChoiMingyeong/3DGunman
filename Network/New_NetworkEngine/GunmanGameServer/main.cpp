#include "GameServer.h"

int main()
{
	GameServer* myGameServer = new GameServer("127.0.0.1", "root", "bg0823", "gunman3duserdb", 3306);
	while (1)
	{
		Sleep(0);
	}
}