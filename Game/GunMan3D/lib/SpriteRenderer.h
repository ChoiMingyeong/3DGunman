#pragma once
class Renderer;


class SpriteRenderer : public Renderer
{
public :
	SpriteRenderer();
	virtual ~SpriteRenderer();

public:
	virtual void Init() override;
	virtual void Render(Camera* pCamera) override;

	void SetSpriteByName(std::wstring dataName);
	void SetSprite(ID3D11ShaderResourceView* data, bool bSizeSet = false);

private:
	ID3D11ShaderResourceView* mTexture;

	Matrix mTexTM;

	Vector2 mScreenSize;
	Vector2 mImageSize;
};

