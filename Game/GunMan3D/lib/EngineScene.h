/*
	EngineScene : 엔진에서 Render와 Game을 이어주는 역할, Object, Light, Camera등을 모두 알고 있는다.
	IScene : 외부에서 재정의 해야하는 Scene 구조
	Render는 EngineScene의 Render()만 실행
*/
#pragma once

class EngineScene
{
public:
	EngineScene(IScene* scene);
	~EngineScene();

public:
	// 업데이트 문
	void SceneStart();
	void SceneUpdate(float deltaTime);
	void SceneRender();

private:
	bool mbChangeScene;

private:
	// Update가 재정의된 Scene 객체
	IScene* mScene; 

	// 에디터 카메라
	Camera* mEditorCamera;

public:
	// 오브젝트 List
	std::vector<GameObject*> mGameObjectVec;

	// 렌더링 List ( 따로 관리 )
	std::vector<Renderer*> mRendererVec;

	// 카메라	List
	std::vector<Camera*> mCameraVec;	

	// 라이트	List
	std::vector<DirectionalLight> mDirLightVec;

	// 글씨 출력 정보 List

};

