#include "pch.h"
#include "LobbyScene.h"

LobbyScene::LobbyScene()
{
}

LobbyScene::~LobbyScene()
{
}

void LobbyScene::Start()
{
	if (mLobbySceneManager == nullptr)
	{
		mLobbySceneManager = new GameObject();
		mLobbySceneManager->AddComponent<LobbySceneManager>();
	}
}
