/*
	게임 씬
	게임이 진행되는 씬, 나가기 버튼을 누르면 Lobby Scene으로 이동한다.
*/
#pragma once
class GameManager;

class GameScene : public IScene
{
public:
	GameScene();
	~GameScene();

public:
	virtual void Start() override;

private:
	GameObject* mGameManager;
	GameObject* mMapManager;

};

