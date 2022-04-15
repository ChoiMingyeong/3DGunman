#pragma once
class LobbySceneManager;

class RoomCell :public GameObject
{
public:
	RoomCell();
	virtual ~RoomCell();

public:
	void InitRoomCell(int index);

	void RefreshRoomCellByLine(int cellIndex, int userCount);

	void OnEnterButtonClickedUp();

public:
	LobbySceneManager* lobbyManager;

	GameObject* mRoomEnterButton;
	TextRenderer* mRoomNumText;
	TextRenderer* mRoomUserCountText;

	int mMaxUserCount = 2;
	int mCurrenCount;

private:
	int mIndex;		// 목록상 번호
	int mCellIndex; // 실제 방 번호

	// 페이지를 넘기다 보면
	// 0번째 룸에 5번째 방정보를 셋팅해야한다.
};

