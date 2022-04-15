#pragma once
class TextRenderer : public Renderer
{
public:
	TextRenderer();
	virtual ~TextRenderer();

public:
	virtual void Render(Camera* pCamera) override;

	void SetText(std::wstring text, ...);
	void SetColor(Vector4 color);
	void SetSize(float size);

private:
	float mSize;
	Vector4 mColor;
	TCHAR mText[1024];

};