#include "pch.h"
#include "LoginScene.h"

LoginScene::LoginScene()
{
}

LoginScene::~LoginScene()
{
}

void LoginScene::Start()
{
	if (mLoginSceneManager == nullptr)
	{
		mLoginSceneManager = new GameObject();
		mLoginSceneManager->AddComponent<LoginSceneManager>();
	}
}
