#pragma once
class BTEditBox : public Renderer
{
public:
	BTEditBox();
	~BTEditBox();
public:
	// 재정의 함수
	virtual void Init() override;
	virtual void Start() override;
	virtual void Update(float deltaTime) override;
	virtual void Render(Camera* pCamera) override;

	static void SetFocusEditBox(BTEditBox* editBox);

public:
	static BTEditBox* FocusEditBox; // 계속 편집되어야하는 EditBox
	static BTEditBox* FirstFocus;

	SpriteRenderer* mEditBoxImageRender;
private:

	ID3D11ShaderResourceView* mBackImg;

	// 텍스트
	std::wstring	mText;

	// 텍스트 속성
	float			mSize;
	Vector4			mColor;
	Vector2			mPadding;

	// 지우기 입력 딜레이 값
	float			mInputDelay;

	// 입력 속성 값
	int				mInputLimit;	// 최대 입력 수 -1은 무제한
	bool			mbUseNewLine;	// Enter를 이요한 New Line 허용
	bool			mbPassword;		// *로 표시합니다.

public:
	void SetBackImageByName(std::wstring dataName);

	void SetColor(Vector4 color);
	void SetSize(float size);
	void SetPadding(Vector2 padding);

	void SetInputLimit(int count);
	void SetAcceptNewLine(bool bAccept);
	void SetPasswordMode(bool bPassword);

	void ClearText();

	std::wstring GetText();

private:
	void InputCheck(float deltaTime);
	int  GetInputChar(int ch);
};

