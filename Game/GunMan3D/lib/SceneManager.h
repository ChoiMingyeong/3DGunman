#pragma once
enum class SceneLoadType
{
	Single		= 0,
	Additive	= 1
};

class SceneManager
{
public:
	static vector<EngineScene*> mBuildSceneVec;

public:
	static void AddEngineScene(EngineScene* engineScene);

	static void LoadScene(int	  sceneNum,  SceneLoadType type = (SceneLoadType)0);
	static void LoadScene(wstring sceneName, SceneLoadType type = (SceneLoadType)0);
};

