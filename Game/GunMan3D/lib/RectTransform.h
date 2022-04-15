#pragma once
#include "Transform.h"

class RectTransform : public Transform
{
public:
	RectTransform();
	virtual ~RectTransform();

public:
	// Rect ¸ðµå
	float	mNearHeight;
	float	mFarHeight;

	Vector4 mRect;
	Vector2 mScreenSize;
	Vector2 mImageSize;

public:
	virtual Matrix  GetTransformMatrix() override;
	
	RECT GetRectSize();

	void SetRectByOriginal();
};

