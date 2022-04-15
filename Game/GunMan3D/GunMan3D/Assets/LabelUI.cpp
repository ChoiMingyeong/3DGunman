#include "pch.h"
#include "LabelUI.h"

LabelUI::LabelUI()
{
}

LabelUI::~LabelUI()
{
	delete mTextRender;
}

void LabelUI::InitLabelUI()
{

	if (mTextRender == nullptr)
	{
		GameObject* textObj = new GameObject();
		mTextRender = textObj->AddComponent<TextRenderer>();
	}

	this->AddComponent<SpriteRenderer>();

	mTextRender->SetColor(Vector4(0.9f, 0.9f, 0.9f, 1));
	mTextRender->SetSize(1.3f);

}

void LabelUI::SetPositionLabel()
{
	RECT rect = mRectTransform->GetRectSize();
	float posX = rect.left + mOffset.x;
	float posY = rect.top + mOffset.y;
	mTextRender->gameObject->mTransform->Position = Vector3(posX, posY, 0);
}
