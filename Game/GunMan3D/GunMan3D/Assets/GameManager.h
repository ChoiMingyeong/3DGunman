#pragma once
enum class EGameState
{
	Ready = 0,
	Start,
	Hold,
	OpenFire,
	End,
	ShowResult
};

enum class EWinPlayer
{
	Draw = 0,
	Player1,
	Player2
};

class GameManager : public BTBehaviour
{
public:
	GameManager();
	~GameManager();

public:
	virtual void Start() override;
	virtual void Update(float deltaTime) override;

	void CreateObject();
	void InitGame();

private:
	void ReadyFunc();
	void StartFunc();
	void HoldFunc(float deltaTime);
	void OpenFireFunc(float deltaTime);
	void EndFunc();
	void ShowResultFunc(float deltaTime);

	void RefreshReadyImage();
	void ResultImageOnOff(bool bAllOff = false);

	void TestInput(float deltaTime);


	// 온라인 용 함수

	// 플레이어 접속 
	void Player_1_On(bool bOn); //무조건 실행
	void Player_2_On(bool bOn);		 

public:
	void Recieve_OpponentExit();
	void Recieve_RoomUserState(string id1, string id2);
	void Recieve_ReadyState(string id1, string id2);
	void Recieve_AllPlayerReady(float waitTime); 
	void Recieve_GameResult(string id1);

public:
	Camera* mSceneCamera;
	GameObject* mControlPlayer;

	// 플레이어의 값
	GameObject* mPlayer1;
	GameObject* mPlayer2;
	
	wstring mP1_ID;
	wstring mP2_ID;

	bool mPlayerOn_1;
	bool mPlayerOn_2;

	bool mPlayerReady_1;
	SpriteRenderer* mReadyImage_1;
	bool mPlayerReady_2;
	SpriteRenderer* mReadyImage_2;

	// 게임 진행에 필요한 값

	GameObject* mHoldImage;
	GameObject* mFireImage;

	GameObject* mWinImage;
	GameObject* mDefeatedImage;
	GameObject* mDrawImage;


	// 현재 진행 상태
	EGameState mCurrentState;

	// 승리 플레이어
	EWinPlayer mWinPlayer;

	// 대기 시간
	float mWaitTime;
	
	// 결과 화면 대기 시간
	float mResultElapsedTime;

	// 경과 시간
	float mElapsedTime;

	// 발사 시간 & 발사 여부
	bool mbFired_P1;
	float mFireTime_P1;

	bool mbFired_P2;
	float mFireTime_P2;
	
	// 기준 시간
	const float LIMIT_TIME = 1.f;
	const float RESULT_TIME = 5.f;

	// 디버깅
	GameObject* mReadyStateTextUI;
	GameObject* mCurrentRoomNumTextUI;
};

