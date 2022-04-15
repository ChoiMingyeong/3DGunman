#include "pch.h"
#include "LobbySceneManager.h"

void LobbyClient::Receive(SSocket* psSocket, char* buffer)
{
	SHeader* packet = reinterpret_cast<SHeader*>(buffer);

	switch (packet->usType)
	{
	case PacketType::StringPakcet:
	{
		StringPacket* stringPacket = reinterpret_cast<StringPacket*>(packet);
		istringstream ss(stringPacket->buffer);
		string str;
		vector<string> strVec;
		while (getline(ss, str, ','))
		{
			strVec.push_back(str);
		}

		/// 0 =	입장 : "0,id1,id2" 
		/// ex) 1명 일 때 "0,test," / 2명 일 때 "0,test,test2"
		///	1 = 퇴장: "1"
		///	2 =	준비 : "2,id"
		///	3 =	게임시작 : "3,랜덤시간"
		///	4 =	게임결과 : "4,id1,id2"이긴 id만, 비기면 둘다

		/// 5 = 대기실 접속자 명단 업데이트
		/// 6 = 대기실 룸 업데이트

		// 메세지 구분
		switch (atoi(strVec[0].c_str()))
		{
		case 0:
		{
			//입장: "0,id1,id2"
			// strVec의 사이즈 -1 이 접속 인원수 
			if ((int)strVec.size()-1 == 1)
			{
				gameManager->Recieve_RoomUserState(strVec[1], "");
			}
			else if((int)strVec.size() -1 == 2)
			{
				gameManager->Recieve_RoomUserState(strVec[1], strVec[2]);
			}
		}
		break;
		case 1:
		{
			// 1 = 퇴장: "1"
			gameManager->Recieve_OpponentExit();
		}
		break;
		case 2:
		{
			//	2 =	준비 : "2,id1,id2" 
			if ((int)strVec.size() - 1 == 0)
			{
				gameManager->Recieve_RoomUserState("", "");
			}
			else if ((int)strVec.size() - 1 == 1)
			{
				gameManager->Recieve_RoomUserState(strVec[1], "");
			}
			else if ((int)strVec.size() - 1 == 2)
			{
				gameManager->Recieve_RoomUserState(strVec[1], strVec[2]);
			}
		}
		break;
		case 3:
		{
			//	3 =	게임시작 : "3,랜덤시간"
			gameManager->Recieve_AllPlayerReady(atof(strVec[1].c_str()));
		}
		break;
		case 4:
		{
			//	4 =	게임결과 : "4,id1,id2"이긴 id만, 비기면 둘다
			if ((int)strVec.size() - 1 == 0)
			{
				gameManager->Recieve_GameResult("");
			}
			else if ((int)strVec.size() - 1 == 1)
			{
				gameManager->Recieve_GameResult(strVec[1]);
			}
		}
		break;
		case 5:
		{
			int num = atoi(strVec[1].c_str());

			for (int i = 0; i < strVec.size() - 2; ++i)
			{
				string id = strVec[2 + i];
				wstring tempwString;
				tempwString.assign(id.begin(), id.end());
				lobbySceneManager->mAccessVec[(num * 2) + i] = tempwString;
			}

			if (strVec.size() - 2 < 2)
			{
				lobbySceneManager->mAccessVec[(num * 2) + 1] = L"";
			}
		}
		break;

		case 6:
		{
			const char* buf = strVec[1].c_str();

			for (int i = 0; i < 50; i++)
			{
				lobbySceneManager->mRoomHeadCountVec[i] = buf[i] - '0';
			}
		}
		break;

		}
	}
	break;
	}
}


LobbySceneManager::LobbySceneManager()
{
}

LobbySceneManager::~LobbySceneManager()
{
}

void LobbySceneManager::Start()
{
	if (nullptr != GameDataManager::mMGNetwork->m_pPacketSystem)
	{
		delete GameDataManager::mMGNetwork->m_pPacketSystem;
		GameDataManager::mMGNetwork->m_pPacketSystem = nullptr;
	}
	GameDataManager::mMGNetwork->m_pPacketSystem = new LobbyClient();

	// 씬매니저를 알고 있어야 로그를 띄울 수 있다.
	LobbyClient* lobbyClient = static_cast<LobbyClient*>(GameDataManager::mMGNetwork->m_pPacketSystem);
	lobbyClient->lobbySceneManager = this;

	mAccessVec.resize(100, wstring());
	mRoomHeadCountVec.resize(50, 0);

	StringPacket* stringPacket = new StringPacket();
	char buf[64] = { 0, };
	string sID;
	sID.assign(GameDataManager::ID.begin(), GameDataManager::ID.end());
	sprintf_s(buf, sizeof(buf), "0,0,%s", sID.c_str());
	memcpy_s(stringPacket->buffer, sizeof(stringPacket->buffer), buf, sizeof(buf));
	GameDataManager::mMGNetwork->SendPakcet(stringPacket, GameDataManager::mMGNetwork->m_ServerList.at("Game"));

	if (mBackground == nullptr)
	{
		mBackground = new GameObject();
		mBackground->AddComponent<SpriteRenderer>()->SetSpriteByName(L"LobbySceneBackground");
	}

	if (mRoomBackground == nullptr)
	{
		mRoomBackground = new GameObject();
		mRoomBackground->AddComponent<SpriteRenderer>()->SetSpriteByName(L"RoomBackground");
		mRoomBackground->mRectTransform->mRect = Vector4(0, 0.2771f, 0, 0.1408f);
	}

	if (mProfileBackground == nullptr)
	{
		mProfileBackground = new GameObject();
		mProfileBackground->AddComponent<SpriteRenderer>()->SetSpriteByName(L"ProfileBackground");
		mProfileBackground->mRectTransform->mRect = Vector4(0.7229f, 0, 0, 0.6593f);
	}

	if (mAccessListBackground == nullptr)
	{
		mAccessListBackground = new GameObject();
		mAccessListBackground->AddComponent<SpriteRenderer>()->SetSpriteByName(L"AccessListBackground");
		mAccessListBackground->mRectTransform->mRect = Vector4(0.7229f, 0, 0.3407f, 0.1408f);
	}

	if (RoomListBtnUp == nullptr)
	{
		RoomListBtnUp = new GameObject();
		BTButton* button = RoomListBtnUp->AddComponent<BTButton>();
		button->SetBasicImgByName(L"RoomList_Btn_UP");
		button->SetClickImgByName(L"RoomList_Btn_UP_PUSH");
		button->SetClickUpEvent([&]() { OnRoomListUp_Btn_Clicked(); });
		RoomListBtnUp->mRectTransform->mRect = Vector4(0, 1 - 0.3625f, 0.8592f, 0.0f);
	}

	if (RoomListBtnDown == nullptr)
	{
		RoomListBtnDown = new GameObject();
		BTButton* button = RoomListBtnDown->AddComponent<BTButton>();
		button->SetBasicImgByName(L"RoomList_Btn_DOWN");
		button->SetClickImgByName(L"RoomList_Btn_DOWN_PUSH");
		button->SetClickUpEvent([&]() { OnRoomListDown_Btn_Clicked(); });
		RoomListBtnDown->mRectTransform->mRect = Vector4(0.3625f, 0.275f, 0.8592f, 0.0f);
	}

	if (AccessListBtnUp == nullptr)
	{
		AccessListBtnUp = new GameObject();
		BTButton* button = AccessListBtnUp->AddComponent<BTButton>();
		button->SetBasicImgByName(L"AccessList_Btn_UP");
		button->SetClickImgByName(L"AccessList_Btn_UP_PUSH");
		button->SetClickUpEvent([&]() { OnAccessListUp_Btn_Clicked(); });
		AccessListBtnUp->mRectTransform->mRect = Vector4(0.7229f, 1 - (0.7229f + 0.1375f), 0.8592f, 0.0f);
	}
	if (AccessListBtnDown == nullptr)
	{
		AccessListBtnDown = new GameObject();
		BTButton* button = AccessListBtnDown->AddComponent<BTButton>();
		button->SetBasicImgByName(L"AccessList_Btn_DOWN");
		button->SetClickImgByName(L"AccessList_Btn_DOWN_PUSH");
		button->SetClickUpEvent([&]() { OnAccessListDown_Btn_Clicked(); });
		AccessListBtnDown->mRectTransform->mRect = Vector4(0.7229f + 0.1375f, 0, 0.8592f, 0.0f);
	}

	if (mRoomVec.empty() == true)
	{
		for (int index = 0; index < 20; ++index)
		{
			mRoomVec.push_back(new RoomCell());
			mRoomVec[index]->InitRoomCell(index);
			mRoomVec[index]->lobbyManager = this;
		}
	}

	if (mProfileLabelVec.empty() == true)
	{
		for (int index = 0; index < 5; ++index)
		{
			/*
				0 : 아이디
				1 : 전체 판수
				2 : 승리 판수
				3 : 패배 판수
				4 : 무승부 판수
			*/
			mProfileLabelVec.push_back(new LabelUI());
			mProfileLabelVec[index]->InitLabelUI();

			SpriteRenderer* labelBack = mProfileLabelVec[index]->GetComponent<SpriteRenderer>();
			labelBack->SetSpriteByName(L"ProfileLabelBackground");
			mProfileLabelVec[index]->mRectTransform->mRect
				= Vector4(0.873f, 0, 0, 0.963f) + Vector4(-0.127f, 0.127f, 0.05f * index, -0.05f * index) + Vector4(0, 0, 0.05f, -0.05f);

			mProfileLabelVec[index]->mOffset = Vector2(10.f, 8.f);
			mProfileLabelVec[index]->SetPositionLabel();
		}
	}

	if (mAccessLabelVec.empty() == true)
	{
		for (int index = 0; index < 7; ++index)
		{
			mAccessLabelVec.push_back(new LabelUI());
			mAccessLabelVec[index]->InitLabelUI();

			SpriteRenderer* labelBack = mAccessLabelVec[index]->GetComponent<SpriteRenderer>();
			labelBack->SetSpriteByName(L"AccessListLabelBackground");

			mAccessLabelVec[index]->mRectTransform->mRect
				= Vector4(0.873f, 0, 0, 0.95f) + Vector4(-0.127f, 0.027f, 0.065f * index, -0.065f * index) + Vector4(0, 0, 0.38f, -0.38f);

			mAccessLabelVec[index]->mOffset = Vector2(10.f, 8.f);
			mAccessLabelVec[index]->SetPositionLabel();
		}
	}
}

void LobbySceneManager::Update(float deltaTime)
{
	RefreshProfileLabel();

	if (BTInputEvent::InputKeyDown(VK_RETURN))
	{
		//BTScene::LoadScene((int)EGunManScene::GameScene);
	}

	for (int index = 0; index < (int)mRoomVec.size(); ++index)
	{
		// 여기에서 현재 보여지는 룸의 정보를 입력
		int cell_idx = (mLineNum * 5) + index;
		mRoomVec[index]->RefreshRoomCellByLine(cell_idx, mRoomHeadCountVec[cell_idx]);
	}
	for (int index = 0; index < (int)mProfileLabelVec.size(); ++index)
	{
		mProfileLabelVec[index]->SetPositionLabel();
	}
	for (int index = 0; index < (int)mAccessLabelVec.size(); ++index)
	{
		mAccessLabelVec[index]->SetPositionLabel();
		mAccessLabelVec[index]->mTextRender->SetText(L"%d. %s", mAccessLineNum + index + 1, mAccessVec[mAccessLineNum + index].c_str());
	}
}

void LobbySceneManager::OnRoomListUp_Btn_Clicked()
{
	mLineNum -= 1;
	if (mLineNum < 0)
	{
		mLineNum = 0;
	}
}

void LobbySceneManager::OnRoomListDown_Btn_Clicked()
{
	mLineNum += 1;

	// 최대 50개 방, 한줄에 5개, 최대 보여지는 개수 4
	int maxLine = (50 / 5) - 4;
	if (mLineNum > maxLine)
	{
		mLineNum = maxLine;
	}

}

void LobbySceneManager::OnAccessListUp_Btn_Clicked()
{
	mAccessLineNum -= 7;
	if (mAccessLineNum < 0)
	{
		mAccessLineNum = 0;
	}
}

void LobbySceneManager::OnAccessListDown_Btn_Clicked()
{
	mAccessLineNum += 7;

	int maxLine = 100 - 7;
	if (mAccessLineNum > maxLine)
	{
		mAccessLineNum = maxLine;
	}
}

void LobbySceneManager::RefreshProfileLabel()
{
	/*
		0 : 아이디
		1 : 전체 판수
		2 : 승리 판수
		3 : 패배 판수
		4 : 무승부 판수
	*/
	mProfileLabelVec[0]->mTextRender->SetText(L"아이디\t\t%s", GameDataManager::ID.c_str());
	mProfileLabelVec[1]->mTextRender->SetText(L"판수\t\t\t%3d", GameDataManager::WIN + GameDataManager::LOSE + GameDataManager::DRAW);
	mProfileLabelVec[2]->mTextRender->SetText(L"승리\t\t\t%3d", GameDataManager::WIN);
	mProfileLabelVec[3]->mTextRender->SetText(L"패배\t\t\t%3d", GameDataManager::LOSE);
	mProfileLabelVec[4]->mTextRender->SetText(L"무승부\t\t%3d", GameDataManager::DRAW);
}

void LobbySceneManager::RefreshAccessListLabel()
{

}
