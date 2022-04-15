#include "pch.h"
#include "GameManager.h"


GameManager::GameManager()
{
	srand(std::time(nullptr));
}

GameManager::~GameManager()
{
}

void GameManager::Start()
{
	CreateObject();

	mSceneCamera->mPosition = Vector3(12.59f, -4.918f, 14.5744f);
	mSceneCamera->LookAt(mSceneCamera->mPosition, mSceneCamera->mPosition + Vector3(-0.27f, 0.026f, 0.9597f), Vector3(0, 1.0f, 0));
	mSceneCamera->IsEditorCamera = false;
	mControlPlayer = mPlayer1;

	InitGame();
}

void GameManager::Update(float deltaTime)
{
	// >> 씬 컨트롤
	//TestInput(deltaTime);

	if (BTInputEvent::InputKeyDown(VK_BACK))
	{
		if (GameDataManager::IsOnlineMode == true)
		{
			StringPacket* strPacket = new StringPacket();
			sprintf_s(strPacket->buffer, sizeof(strPacket->buffer), "1,1,%d", GameDataManager::RoomNumber);
			GameDataManager::mMGNetwork->SendPakcet(strPacket, GameDataManager::mMGNetwork->m_ServerList.at("Game"));

			BTScene::LoadScene((int)EGunManScene::LobbyScene);
		}
		else
		{
			BTScene::LoadScene((int)EGunManScene::LoginScene);
		}
	}

	// >> 게임 진행
	switch (mCurrentState)
	{
	case EGameState::Ready:
	{
		ReadyFunc();
	}
	break;
	case EGameState::Start:
	{
		StartFunc();
	}
	break;
	case EGameState::Hold:
	{
		HoldFunc(deltaTime);
	}
	break;
	case EGameState::OpenFire:
	{
		OpenFireFunc(deltaTime);
	}
	break;
	case EGameState::End:
	{
		EndFunc();
	}
	break;
	case EGameState::ShowResult:
	{
		ShowResultFunc(deltaTime);
	}
	break;

	default:
		break;
	}

}

void GameManager::InitGame()
{
	if (mCurrentRoomNumTextUI != nullptr)
	{
		if (GameDataManager::IsOnlineMode == true)
		{
			// 해당 방번호로 룸 접속을 요청한다. > 피드백을 안받아서 확인 단계가 없음...
			// 따라서 씬을 변경 후에 옮겨야함
			// 성공한 경우 씬을 변경하고
			// 실패한 경우 아무것도 하지 않는다.
			StringPacket* strPacket = new StringPacket();
			sprintf_s(strPacket->buffer, sizeof(strPacket->buffer), "0,2,%d", GameDataManager::RoomNumber);
			bool bSuccess = GameDataManager::mMGNetwork->SendPakcet(strPacket, GameDataManager::mMGNetwork->m_ServerList.at("Game"));

			// 룸번호 출력
			mCurrentRoomNumTextUI->GetComponent<TextRenderer>()->SetText(L"Room : %d", GameDataManager::RoomNumber + 1);

			// 온라인 모드 나는 무조건 있음
			Player_1_On(true);
			Player_2_On(false);
		}
		else
		{
			// 로컬 모드 둘다 무조건 있음
			Player_1_On(true);
			Player_2_On(true);
		}

	}
}

void GameManager::CreateObject()
{
	LobbyClient* lobbyClient = static_cast<LobbyClient*>(GameDataManager::mMGNetwork->m_pPacketSystem);
	lobbyClient->gameManager = this;

	mPlayer1 = new GameObject();
	mPlayer2 = new GameObject();

	mPlayer1->AddComponent<MeshRenderer>()->SetMeshDatasByStr(L"03IK-Joe");
	mPlayer2->AddComponent<MeshRenderer>()->SetMeshDatasByStr(L"03IK-Joe");

	mPlayer1->mTransform->Position = Vector3(11.f, -7.133f, 16.6f);
	mPlayer1->mTransform->Rotation = Vector3(0, -177.492538f, 0);
	mPlayer1->mTransform->Scale = Vector3(0.0252845f, 0.0252845f, 0.0252845f);

	mPlayer2->mTransform->Position = Vector3(12.239f, -6.351f, 22.5529f);
	mPlayer2->mTransform->Rotation = Vector3(0, 5.84110498f, 0);
	mPlayer2->mTransform->Scale = Vector3(0.0178356f, 0.0178356f, 0.0178356f);

	GameObject* temp1 = new GameObject();
	mReadyImage_1 = temp1->AddComponent<SpriteRenderer>();
	mReadyImage_1->SetSpriteByName(L"Ready_Off");
	temp1->mRectTransform->mRect = Vector4(0.5f, 0.2f, 0.6f, 0.2f);
	temp1->IsActive = false;

	GameObject* temp2 = new GameObject();
	mReadyImage_2 = temp2->AddComponent<SpriteRenderer>();
	mReadyImage_2->SetSpriteByName(L"Ready_Off");
	temp2->mRectTransform->mRect = Vector4(0.73f, 0.15f, 0.25f, 0.65f);
	temp2->IsActive = false;

	mHoldImage = new GameObject();
	mHoldImage->AddComponent<SpriteRenderer>()->SetSpriteByName(L"Hold");
	mHoldImage->mRectTransform->mRect = Vector4(0.292f, 0.292f, 0.411f, 0.411f);
	mHoldImage->IsActive = false;

	mFireImage = new GameObject();
	mFireImage->AddComponent<SpriteRenderer>()->SetSpriteByName(L"Fire");
	mFireImage->mRectTransform->mRect = Vector4(0.292f, 0.292f, 0.411f, 0.411f);
	mFireImage->IsActive = false;

	mDefeatedImage = new GameObject();
	mDefeatedImage->AddComponent<SpriteRenderer>()->SetSpriteByName(L"Defeated");
	mDefeatedImage->mRectTransform->mRect = Vector4(0.2678f, 0.2678f, 0.441f, 0.441f);

	mDefeatedImage->IsActive = false;

	mWinImage = new GameObject();
	mWinImage->AddComponent<SpriteRenderer>()->SetSpriteByName(L"Win");
	mWinImage->mRectTransform->mRect = Vector4(0.284f, 0.284f, 0.441f, 0.441f);
	mWinImage->IsActive = false;

	mDrawImage = new GameObject();
	mDrawImage->AddComponent<SpriteRenderer>()->SetSpriteByName(L"Draw");
	mDrawImage->mRectTransform->mRect = Vector4(0.2678f, 0.2678f, 0.441f, 0.441f);
	mDrawImage->IsActive = false;

	mReadyStateTextUI = new GameObject();
	mReadyStateTextUI->AddComponent<TextRenderer>()->SetSize(3.f);
	mReadyStateTextUI->mTransform->Position = Vector3(10.f, 50.f, 0);

	mCurrentRoomNumTextUI = new GameObject();
	mCurrentRoomNumTextUI->AddComponent<TextRenderer>()->SetSize(2.f);
	mCurrentRoomNumTextUI->mTransform->Position = Vector3(10.f, 5.f, 0);
}

// 대기 상태 시 실행 될 함수
void GameManager::ReadyFunc()
{
	// 플레이어 캐릭터 상태 변경
	mPlayer1->IsActive = mPlayerOn_1;
	mPlayer2->IsActive = mPlayerOn_2;

	if (GameDataManager::IsOnlineMode == true)
	{
		if (BTInputEvent::InputKeyDown(VK_SPACE) && mPlayerOn_1)
		{
			mPlayerReady_1 = !mPlayerReady_1;

			//<Send> 플레이어 1 Ready상태  변경
		}
		RefreshReadyImage();
	}
	else
	{
		if (BTInputEvent::InputKeyDown(VK_SPACE) && mPlayerOn_1)
		{
			mPlayerReady_1 = !mPlayerReady_1;

			//<Send> 플레이어 1 Ready상태  변경
		}

		// 로컬 모드 2P 키 설정 필요
		if (BTInputEvent::InputKeyDown(VK_RETURN) && mPlayerOn_2)
		{
			// 플레이어 2 Ready상태  변경
			mPlayerReady_2 = !mPlayerReady_2;
		}

		RefreshReadyImage();

		//<Receive> 실행 되어야 함, 값이 변경 되고 둘다 레디면 시간을 받는다. > 함수 AllPlayerReady
		// 로컬 모드   이대로 진행
		if (mPlayerReady_1 && mPlayerReady_2)
		{
			mPlayerReady_1 = false;
			mPlayerReady_2 = false;
			mbFired_P1 = false;
			mbFired_P2 = false;
			mReadyImage_1->gameObject->IsActive = false;
			mReadyImage_2->gameObject->IsActive = false;

			//둘다 레디면 상태 변경
			mCurrentState = EGameState::Start;
		}
	}
}

void GameManager::StartFunc()
{
	if (GameDataManager::IsOnlineMode == true)
	{
		mElapsedTime = 0.0f;
		mFireTime_P1 = 0.0f;
		mFireTime_P2 = 0.0f;

		mHoldImage->IsActive = true;
		mCurrentState = EGameState::Hold;
	}
	else
	{
		// 로컬 모드 시작 상태 시 실행
		int rndNum = rand() % 50;
		mWaitTime = (float)rndNum / 10.f;
		mWaitTime += 1.0f; // 기다려야하는 시간 0.5f ~ 3.0f 사이

		mElapsedTime = 0.0f;
		mFireTime_P1 = 0.0f;
		mFireTime_P2 = 0.0f;

		mHoldImage->IsActive = true;
		mCurrentState = EGameState::Hold;
	}
}

// 발사 대기 시 실행 될 함수
void GameManager::HoldFunc(float deltaTime)
{
	mElapsedTime += deltaTime;

	// Test코드
	mReadyStateTextUI->GetComponent<TextRenderer>()->SetText(L"WaitTime: %f\nElapsedTime: %f", mWaitTime, mElapsedTime);

	mHoldImage->mTransform->Scale += Vector3::One * 0.1f * deltaTime;

	if (mElapsedTime >= mWaitTime)
	{
		mElapsedTime = 0.f;
		mbFired_P1 = false;
		mbFired_P2 = false;
		mHoldImage->mTransform->Scale = Vector3::One;
		mHoldImage->IsActive = false;
		mFireImage->mTransform->Scale = Vector3::One * 4.f;
		mFireImage->IsActive = true;
		mCurrentState = EGameState::OpenFire;
	}

	if (GameDataManager::IsOnlineMode == true)
	{
		if (BTInputEvent::InputKeyDown(VK_SPACE))
		{
			// <Send> 현재 플레이어가 미리 눌렀음
			mResultElapsedTime = 0;
			mHoldImage->IsActive = false;
			mHoldImage->mTransform->Scale = Vector3::One;

			// 로컬 모드
			//mWinPlayer = EWinPlayer::Player2;
			//mCurrentState = EGameState::ShowResult;
			return;
		}
	}
	else
	{
		// 로컬 모드
		if (BTInputEvent::InputKeyDown(VK_SPACE))
		{
			mHoldImage->IsActive = false;
			mHoldImage->mTransform->Scale = Vector3::One;

			mWinPlayer = EWinPlayer::Player2;
			mCurrentState = EGameState::ShowResult;

			mResultElapsedTime = 0;
			return;
		}
		// 로컬 모드
		if (BTInputEvent::InputKeyDown(VK_RETURN))
		{
			mHoldImage->IsActive = false;
			mHoldImage->mTransform->Scale = Vector3::One;

			mWinPlayer = EWinPlayer::Player1;
			mCurrentState = EGameState::ShowResult;

			mResultElapsedTime = 0;
			return;
		}
	}

}

// 발사 가능 상태 시 실행 될 함수
void GameManager::OpenFireFunc(float deltaTime)
{
	mElapsedTime += deltaTime;

	// Test용
	mReadyStateTextUI->GetComponent<TextRenderer>()->SetText(L"Player 1 : %f\nPlayer 2 : %f", mFireTime_P1, mFireTime_P2);

	if (mFireImage->mTransform->Scale.x > 1.f)
	{
		mFireImage->mTransform->Scale -= Vector3::One * 25.f * deltaTime;
	}

	if (mElapsedTime >= LIMIT_TIME)
	{
		mFireImage->IsActive = false;
		mCurrentState = EGameState::End;
		return;
	}

	if (GameDataManager::IsOnlineMode == true)
	{
		if (BTInputEvent::InputKeyDown(VK_SPACE) && !mbFired_P1)
		{
			mbFired_P1 = true;
			mFireTime_P1 = mElapsedTime;
		}
	}
	else
	{
		// 로컬 모드용
		if (BTInputEvent::InputKeyDown(VK_SPACE) && !mbFired_P1)
		{
			mbFired_P1 = true;
			mFireTime_P1 = mElapsedTime;
		}
		if (BTInputEvent::InputKeyDown(VK_RETURN) && !mbFired_P2)
		{
			mbFired_P2 = true;
			mFireTime_P2 = mElapsedTime;
		}
	}
}

// 결과 처리 시 실행 될 함수
void GameManager::EndFunc()
{
	if (GameDataManager::IsOnlineMode == true)
	{
		// 온라인 모드
		if (mbFired_P1 == false)
		{
			// <Send> 발사 못한 상태를 보내야함 (시간을 -100으로 담아서 보내기)
		}
		else
		{
			// <Send> 발사 했다고 시간 담아서 보내야함 (시간 담아서 보내기)
		}
	}
	else
	{
		// <Server> 서버에서 처리해 알려줘야한다.
		// 둘다 발사하지 않았거나, 둘의 시간이 완벽하게 같을 때
		if (mbFired_P1 == false && mbFired_P2 == false
			|| mFireTime_P1 == mFireTime_P2)
		{
			// 둘다 입력을 안한 경우 Draw
			mWinPlayer = EWinPlayer::Draw;
			mCurrentState = EGameState::ShowResult;
			mResultElapsedTime = 0;
			ResultImageOnOff();
			return;
		}
		// 둘 중하나는 입력 했고 시간이 적은 쪽이 이긴다.
		if (mFireTime_P1 < mFireTime_P2)
		{
			if (mbFired_P1 == false)
			{
				mWinPlayer = EWinPlayer::Player2;
			}
			else
			{
				mWinPlayer = EWinPlayer::Player1;
			}
		}
		else
		{
			if (mbFired_P2 == false)
			{
				mWinPlayer = EWinPlayer::Player1;
			}
			else
			{
				mWinPlayer = EWinPlayer::Player2;
			}
		}
		ResultImageOnOff();
		mCurrentState = EGameState::ShowResult;
		mResultElapsedTime = 0;
	}
}

void GameManager::ShowResultFunc(float deltaTime)
{
	// 애니메이션 재생하는 구간

	mResultElapsedTime += deltaTime;

	// 1P 기준으로 문구가 뜬다.
	if (mWinPlayer == EWinPlayer::Player1)
	{
		//Win
		if (mWinImage->mTransform->Scale.x > 1.f)
		{
			mWinImage->mTransform->Scale -= Vector3::One * deltaTime * 10.f;
		}
	}
	else if (mWinPlayer == EWinPlayer::Player2)
	{
		// Defeated
		mDefeatedImage->mTransform->Position.y -= 20.f * deltaTime;
	}
	else
	{
		//draw
		mDrawImage->mTransform->Scale = Vector3::One * 1.3f;
	}

	if (mResultElapsedTime >= RESULT_TIME)
	{
		// 접속 상태로 갱신
		mReadyImage_1->gameObject->IsActive = mPlayerOn_1;
		mReadyImage_2->gameObject->IsActive = mPlayerOn_2;

		ResultImageOnOff(true);

		mCurrentState = EGameState::Ready;
	}

	// Test 용
	mReadyStateTextUI->GetComponent<TextRenderer>()->SetText(
		L"Player 1 : %f\nPlayer 2 : %f\nWinPlayer : %d",
		mFireTime_P1,
		mFireTime_P2,
		(int)mWinPlayer
	);
}

// 레디 상태 이미지 교체
void GameManager::RefreshReadyImage()
{
	if (mPlayerReady_1)
	{
		mReadyImage_1->SetSpriteByName(L"Ready_On");
	}
	else
	{
		mReadyImage_1->SetSpriteByName(L"Ready_Off");
	}
	if (mPlayerReady_2)
	{
		mReadyImage_2->SetSpriteByName(L"Ready_On");
	}
	else
	{
		mReadyImage_2->SetSpriteByName(L"Ready_Off");
	}
}

void GameManager::ResultImageOnOff(bool bAllOff)
{
	if (bAllOff)
	{
		mDefeatedImage->IsActive = false;
		mWinImage->IsActive = false;
		mDrawImage->IsActive = false;
		return;
	}

	if (mWinPlayer == EWinPlayer::Player1)
	{
		mDefeatedImage->IsActive = false;
		mWinImage->IsActive = true;
		mDrawImage->IsActive = false;
		mWinImage->mTransform->Scale = Vector3::One * 5.f;
	}
	else if (mWinPlayer == EWinPlayer::Player2)
	{
		mDefeatedImage->IsActive = true;
		mWinImage->IsActive = false;
		mDrawImage->IsActive = false;
		mDefeatedImage->mTransform->Position.y = 50.f;
	}
	else
	{
		mDefeatedImage->IsActive = false;
		mWinImage->IsActive = false;
		mDrawImage->IsActive = true;
	}
}

//Test 용

void GameManager::TestInput(float deltaTime)
{
	if (BTInputEvent::InputKeyDown(VK_F9))
	{
		mSceneCamera->mPosition = Vector3(12.59f, -4.918f, 14.5744f);
		mSceneCamera->LookAt(mSceneCamera->mPosition, mSceneCamera->mPosition + Vector3(-0.27f, 0.026f, 0.9597f), Vector3(0, 1.0f, 0));
		mSceneCamera->IsEditorCamera = !mSceneCamera->IsEditorCamera;
	}

	if (BTInputEvent::InputKeyDown(VK_F5))
	{
		static bool bP1 = true;
		bP1 = !bP1;
		if (bP1)
		{
			mControlPlayer = mPlayer1;
		}
		else
		{
			mControlPlayer = mPlayer2;
		}
	}
	if (BTInputEvent::InputKey(VK_I))
	{
		mControlPlayer->mTransform->Position +=
			Vector3::Forward * 5.f * deltaTime;
	}
	if (BTInputEvent::InputKey(VK_K))
	{
		mControlPlayer->mTransform->Position +=
			Vector3::Backward * 5.f * deltaTime;
	}
	if (BTInputEvent::InputKey(VK_J))
	{
		mControlPlayer->mTransform->Position +=
			Vector3::Left * 5.f * deltaTime;
	}
	if (BTInputEvent::InputKey(VK_L))
	{
		mControlPlayer->mTransform->Position +=
			Vector3::Right * 5.f * deltaTime;
	}
	if (BTInputEvent::InputKey(VK_U))
	{
		mControlPlayer->mTransform->Position +=
			Vector3::Down * 5.f * deltaTime;
	}
	if (BTInputEvent::InputKey(VK_O))
	{
		mControlPlayer->mTransform->Position +=
			Vector3::Up * 5.f * deltaTime;
	}
	if (BTInputEvent::InputKey(VK_N))
	{
		mControlPlayer->mTransform->Rotation.y -= 20.f * deltaTime;

	}
	if (BTInputEvent::InputKey(VK_M))
	{
		mControlPlayer->mTransform->Rotation.y += 20.f * deltaTime;
	}
	if (BTInputEvent::InputKey(VK_V))
	{
		mControlPlayer->mTransform->Scale += Vector3::One * 0.1f * deltaTime;

	}
	if (BTInputEvent::InputKey(VK_B))
	{
		mControlPlayer->mTransform->Scale -= Vector3::One * 0.1f * deltaTime;
	}
}


// 온라인 모드용

void GameManager::Player_1_On(bool bOn)
{
	mPlayerOn_1 = bOn;
	mReadyImage_1->gameObject->IsActive = bOn;
	mPlayer1->IsActive = bOn;
}

void GameManager::Player_2_On(bool bOn)
{
	mPlayerOn_2 = bOn;
	mReadyImage_2->gameObject->IsActive = bOn;
	mPlayer2->IsActive = bOn;
}


// 리시브 함수

void GameManager::Recieve_RoomUserState(string id1, string id2)
{
	mP1_ID.clear();
	mP2_ID.clear();
	Player_1_On(false);
	Player_2_On(false);
	if (id1.empty() == false)
	{
		mP1_ID.append(id1.begin(), id1.end());
		Player_1_On(true);
	}
	if (id2.empty() == false)
	{
		mP2_ID.append(id2.begin(), id2.end());
		Player_2_On(true);
	}
}

void GameManager::Recieve_OpponentExit()
{
	Player_2_On(false);
}

void GameManager::Recieve_ReadyState(string id1, string id2)
{
	if (id1.empty() == true && id2.empty() == true)
	{
		mPlayerReady_1 = false; mPlayerReady_2 = false;
		return;
	}

	wstring wId1;
	wId1.append(id1.begin(), id1.end());
	wstring wId2;
	wId2.append(id2.begin(), id2.end());

	if (wId1.empty() == false && wId2.empty() == true)
	{
		if (wId1 == mP1_ID) { mPlayerReady_1 = true; }
		else { mPlayerReady_2 = true; }
	}
	else if (wId1.empty() == true && wId2.empty() == false)
	{
		if (wId2 == mP1_ID) { mPlayerReady_1 = true; }
		else { mPlayerReady_2 = true; }
	}
	else
	{
		mPlayerReady_1 = true; mPlayerReady_2 = true;
	}
}

void GameManager::Recieve_AllPlayerReady(float waitTime)
{
	mPlayerReady_1 = false;
	mPlayerReady_2 = false;
	mbFired_P1 = false;
	mbFired_P2 = false;
	mReadyImage_1->gameObject->IsActive = false;
	mReadyImage_2->gameObject->IsActive = false;

	// 이시점에 mWaitTime도 받아올 수 있나?
	mWaitTime = waitTime;

	mCurrentState = EGameState::Start;
}

void GameManager::Recieve_GameResult(string id1)
{
	if (id1.empty() == true)
	{
		mWinPlayer = EWinPlayer::Draw;
		return;
	}

	wstring wId1;
	wId1.append(id1.begin(), id1.end());
	if (wId1 == mP1_ID) { mWinPlayer = EWinPlayer::Player1; }
	if (wId1 == mP2_ID) { mWinPlayer = EWinPlayer::Player2; }

	// 전적 재개산 (클라에서 보여지는 용도, DB는 따로 서버에서 한다)
	switch (mWinPlayer)
	{
	case EWinPlayer::Draw:
		GameDataManager::DRAW += 1;
		break;
	case EWinPlayer::Player1:
		GameDataManager::WIN += 1;
		break;
	case EWinPlayer::Player2:
		GameDataManager::LOSE += 1;
		break;
	}

	mResultElapsedTime = 0;
	ResultImageOnOff();
	mCurrentState = EGameState::ShowResult;
}
