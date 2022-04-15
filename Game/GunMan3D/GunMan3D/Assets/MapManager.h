#pragma once
class MapManager : public BTBehaviour
{
public:
	MapManager();
	~MapManager();

public:
	virtual void Start() override;
	virtual void Update(float deltaTime) override;

public:
	void CreateMap();

private:
	void CreateEnviroment();
	void CreateRock_1();
	void CreateRock_2();
	void CreateRock_3();
	void CreateRock_4();
	void CreateRock_5();
	void CreateRock_6();

private:
	// 환경 오브젝트
	GameObject* mEnviroment;
	GameObject* mMountin_1;
	GameObject* mMountin_2;
	GameObject* mMountin_1_1;

	// 1번돌 리스트
	vector<GameObject*> mRock_1_Vec;
	vector<GameObject*> mRock_2_Vec;
	vector<GameObject*> mRock_3_Vec;
	vector<GameObject*> mRock_4_Vec;
	vector<GameObject*> mRock_5_Vec;
	vector<GameObject*> mRock_6_Vec;
};

