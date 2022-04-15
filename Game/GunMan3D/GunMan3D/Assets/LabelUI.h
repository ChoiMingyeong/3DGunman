#pragma once
class LabelUI : public GameObject
{
public:
	LabelUI();
	virtual ~LabelUI();

public:
	void InitLabelUI();

	void SetPositionLabel();
	
public:
	TextRenderer* mTextRender;

	Vector2 mOffset;
};

