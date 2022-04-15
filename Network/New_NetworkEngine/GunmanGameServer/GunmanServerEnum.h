#pragma once

enum class GameMSGType
{
	Lobby = 0,
	Room
};

enum class LobbyMSGType
{
	EnterLobby = 0,
	LeaveLobby,
	EnterRoom,
	LeaveRoom,
	Test,
};

enum class RoomMSGType
{
	EnterRoom = 0,
	LeaveRoom,
	Shutdown,
	Ready,
	Start,
	End,
	KeyInput,
};