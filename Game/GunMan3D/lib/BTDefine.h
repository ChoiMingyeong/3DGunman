#pragma once

class IScene
{
public:
	IScene() {};

public:
	// 추상 메서드
	virtual void Start() = 0;

public:
	Camera* mActiveCamera;
};