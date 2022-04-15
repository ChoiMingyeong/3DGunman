#pragma once
class GameManager;
class LobbyClient : public ProcessPacket
{
public:
	LobbyClient()
	{
		hLobbyEvent = CreateEvent(nullptr, false, false, nullptr);
	}
	~LobbyClient()
	{
		CloseHandle(hLobbyEvent);
	}

public:
	LobbySceneManager* lobbySceneManager;
	GameManager* gameManager;

public:
	virtual void Receive(SSocket* psSocket, char* buffer);
	

private:
	HANDLE hLobbyEvent;
};

class LobbySceneManager : public BTBehaviour
{
public:
	LobbySceneManager();
	virtual ~LobbySceneManager();

public:
	virtual void Start() override;
	virtual void Update(float deltaTime) override;

public:
	void OnRoomListUp_Btn_Clicked();
	void OnRoomListDown_Btn_Clicked();

	void OnAccessListUp_Btn_Clicked();
	void OnAccessListDown_Btn_Clicked();

public:
	void RefreshProfileLabel();
	void RefreshAccessListLabel();

private:
	GameObject* mBackground;

	GameObject* mRoomBackground;
	GameObject* mProfileBackground;
	GameObject* mAccessListBackground;

	GameObject* RoomListBtnUp;
	GameObject* RoomListBtnDown;
	GameObject* AccessListBtnUp;
	GameObject* AccessListBtnDown;

	int mLineNum;
	vector<RoomCell*> mRoomVec;

	vector<LabelUI*>  mProfileLabelVec;

	int mAccessLineNum;
	vector<LabelUI*>  mAccessLabelVec;

public:
	vector<wstring>	  mAccessVec;
	vector<int>		  mRoomHeadCountVec;
};

