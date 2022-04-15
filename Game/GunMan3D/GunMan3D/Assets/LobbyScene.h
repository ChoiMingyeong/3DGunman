/*
	대기실 씬
	접속 유저의 명단과 참여 가능한 방을 보여준다.
	방을 선택하면 GameScene으로 넘어간다.
*/
#pragma once
class LobbyScene : public IScene
{
public:
	LobbyScene();
	~LobbyScene();

public:
	virtual void Start() override;

private:
	GameObject* mLobbySceneManager;
};

