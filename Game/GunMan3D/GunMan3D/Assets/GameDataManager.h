#pragma once

class GameDataManager
{
public:
	static int RoomNumber;

	static MGNetwork* mMGNetwork;

	static wstring ID;
	static int WIN;
	static int LOSE;
	static int DRAW;

	static bool IsOnlineMode;
};

