#include "pch.h"
#include "RoomCell.h"

RoomCell::RoomCell()
{
	mCurrenCount = 0;
}

RoomCell::~RoomCell()
{
	delete mRoomEnterButton;
	delete mRoomNumText;
}

// 목록 상 번호를 받아와 위치를 설정한다.
void RoomCell::InitRoomCell(int index)
{
	mIndex = index;

	this->AddComponent<SpriteRenderer>();
	this->GetComponent<SpriteRenderer>()->SetSpriteByName(L"SmallRoomBackground");

	// 번호를 좌표로 바꿔야한다.
	POINT coordi;
	coordi.x = mIndex % 5;
	coordi.y = mIndex / 5;

	float posX = 0.025f;
	float posY = 0.025f;
	float minX = coordi.x * 0.1354f + posX;
	float maxX = 1 - (coordi.x + 1) * 0.1354f - posX;

	float minY = 1 - (coordi.y + 1) * 0.2037f - posY; 
	float maxY = coordi.y * 0.2037f + posY;

	mRectTransform->mRect = Vector4(minX, maxX, maxY, minY);

	GameObject* temp1 = new GameObject();
	mRoomNumText= temp1->AddComponent<TextRenderer>();

	mRoomEnterButton = new GameObject();
	BTButton* button = mRoomEnterButton->AddComponent<BTButton>();
	button->SetBasicImgByName(L"RoomEnterButton");
	button->SetClickImgByName(L"RoomEnterButton_PUSH");
	button->SetClickUpEvent( [&](){ OnEnterButtonClickedUp(); } );

	mRoomEnterButton->mRectTransform->mRect = mRectTransform->mRect;
	mRoomEnterButton->mRectTransform->mRect += Vector4(0.03f, 0.03f, 0.13f, 0.03f);
	mRoomEnterButton->GetComponent<SpriteRenderer>()->SetSortOrder(5);

	GameObject* temp2 = new GameObject();
	mRoomUserCountText = temp2->AddComponent<TextRenderer>();

}

void RoomCell::RefreshRoomCellByLine(int cellIndex, int userCount)
{
	// 실제 표시될 Cell정보 표시
	mCellIndex = cellIndex;

	float rPosX = (mRectTransform->mRect.x + 0.0125f) * mRectTransform->mScreenSize.x;
	float rPosY = (mRectTransform->mRect.z + 0.025f) * mRectTransform->mScreenSize.y;

	mRoomNumText->gameObject->mTransform->Position = Vector3(rPosX, rPosY, 0);
	mRoomNumText->SetText(L"%d", mCellIndex+1);
	mRoomNumText->SetSize(1.5f);
	mRoomNumText->SetColor(Vector4(0.f, 0.f,0.f, 1.f));

	mCurrenCount = userCount;
	mRoomUserCountText->gameObject->mTransform->Position = Vector3(rPosX + 30, rPosY + 30, 0);
	mRoomUserCountText->SetText(L"( %d / %d )", mCurrenCount, mMaxUserCount);
	mRoomUserCountText->SetSize(1.75f);
	mRoomUserCountText->SetColor(Vector4(0.f, 0.f, 0.f, 1.f));
}

void RoomCell::OnEnterButtonClickedUp()
{
	int roomUserCount = lobbyManager->mRoomHeadCountVec[mCellIndex];
	if (roomUserCount >= 2)
	{
		//인원수가 꽉차서 접속 불가
		return;
	}

	GameDataManager::RoomNumber = mCellIndex;

	// GameScene이 모두 실행 되고 나서야 메세지를 받아야한다.
	// 즉 모두 로드 된다음에 메세지를 보내야한다.
	BTScene::LoadScene((int)EGunManScene::GameScene);
}

