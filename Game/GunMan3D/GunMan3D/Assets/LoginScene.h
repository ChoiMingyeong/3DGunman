/*
	로그인 씬
	입력한 유저 데이터가 문제가 없다면 LobbyScene으로 넘어간다.
*/
#pragma once
class LoginScene : public IScene
{
public:
	LoginScene();
	~LoginScene();

public:
	virtual void Start() override;

private:
	GameObject* mLoginSceneManager;

};

