#pragma once
#include <SimpleMath.h>
using namespace DirectX::SimpleMath;

class Transform 
{
public:
	Transform();
	virtual ~Transform();

public:
	Vector3 Position;
	Vector3 Rotation;
	Vector3 Scale;

public:
	Vector3 GetForward();
	virtual Matrix  GetTransformMatrix();
};

