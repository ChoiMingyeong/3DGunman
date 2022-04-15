/*
	데이터를 로드하는 씬 로드가 끝나면 바로 LoginScene으로 넘어간다.
*/
#pragma once
class LoadScene : public IScene
{
public:
	LoadScene();
	~LoadScene();

public:
	virtual void Start() override;
};

