#include "pch.h"
#include "MapManager.h"

MapManager::MapManager()
{
}

MapManager::~MapManager()
{
}

void MapManager::Start()
{
}

void MapManager::Update(float deltaTime)
{
}

void MapManager::CreateMap()
{
	CreateEnviroment();
	CreateRock_1();
	CreateRock_2();
	CreateRock_3();
	CreateRock_4();
	CreateRock_5();
	CreateRock_6();
}

void MapManager::CreateEnviroment()
{
	// 바닥
	mEnviroment = new GameObject();
	mEnviroment->AddComponent<MeshRenderer>()->SetMeshDatasByStr(L"Enviroment_1");
	mEnviroment->mTransform->Position = Vector3(7.8638, -4.028f, 74.842f);
	mEnviroment->mTransform->Scale = Vector3(7, 4.728f, 7);
	mEnviroment->mTransform->Rotation.y = 235.84f;

	BTMaterial* mat = BTResource::GetMaterial(L"Enviroment_1", 0);
	mat->mProperties->Ambient = Vector4(0.8f, 0.401f, 0.115f, 1.f);

	// 뒤쪽 산
	mMountin_1 = new GameObject();
	mMountin_1->AddComponent<MeshRenderer>()->SetMeshDatasByStr(L"Mounting_1");
	mMountin_1->mTransform->Position = Vector3(1.263844f, -2.92866f, 139.2421f);
	mMountin_1->mTransform->Scale = Vector3(6.191341f, 5, 4.059378f);
	mMountin_1->mTransform->Rotation.y = 300.f;

	mat = BTResource::GetMaterial(L"Mounting_1", 0);
	mat->mProperties->Ambient = Vector4(0.8f, 0.401f, 0.115f, 1.f);

	// 뒤쪽 산
	mMountin_1_1 = new GameObject();
	mMountin_1_1->AddComponent<MeshRenderer>()->SetMeshDatasByStr(L"Mounting_1");
	mMountin_1_1->mTransform->Position = Vector3(-33.23616f, -5.228665f, 47.14209f);
	mMountin_1_1->mTransform->Scale = Vector3(2.052611f, 4.130066f, 4.718091f);
	mMountin_1_1->mTransform->Rotation.y = 180 - 77.869f;

	mat = BTResource::GetMaterial(L"Mounting_1", 0);
	mat->mProperties->Ambient = Vector4(0.8f, 0.401f, 0.115f, 1.f);

	// 앞쪽 오른쪽 산
	mMountin_2 = new GameObject();
	mMountin_2->AddComponent<MeshRenderer>()->SetMeshDatasByStr(L"Mounting_2");
	mMountin_2->mTransform->Position = Vector3(52.96384f, -6.728665f, 69.44209f);
	mMountin_2->mTransform->Scale = Vector3(3.800004f, 2.645041f, 2.645042f);
	mMountin_2->mTransform->Rotation = Vector3(0.984f, 335.014f, -0.216f);

	mat = BTResource::GetMaterial(L"Mounting_2", 0);
	mat->mProperties->Ambient = Vector4(0.8f, 0.401f, 0.115f, 1.f);
}
void MapManager::CreateRock_1()
{
	BTMaterial* mat;

	// Rock_1
	for (int index = 0; index < 3; ++index)
	{
		mRock_1_Vec.push_back(new GameObject());
		mRock_1_Vec[index]->AddComponent<MeshRenderer>()->SetMeshDatasByStr(L"Rock_1");

	}
	mat = BTResource::GetMaterial(L"Rock_1", 0);
	mat->mProperties->Ambient = Vector4(0.65f, 0.333f, 0.0705f, 1.f);
	mRock_1_Vec[0]->mTransform->Position = Vector3(-1.126156f, -8.188665f, 13.04209f);
	mRock_1_Vec[0]->mTransform->Rotation = Vector3(4.528f, -1.079f, -0.554f);
	mRock_1_Vec[0]->mTransform->Scale = Vector3(2.511697f, 1.899952f, 4.142993f);

	mRock_1_Vec[1]->mTransform->Position = Vector3(17.73384f, -5.888665f, 12.80209f);
	mRock_1_Vec[1]->mTransform->Rotation = Vector3(-11.671f, -0.102f, 0.503f);
	mRock_1_Vec[1]->mTransform->Scale = Vector3(1.8595f, 1.368851f, 1.974687f);

	mRock_1_Vec[2]->mTransform->Position = Vector3(18.50384f, -5.548666f, 12.95209f);
	mRock_1_Vec[2]->mTransform->Rotation = Vector3(-11.971f, -1.129f, 0.148f);
	mRock_1_Vec[2]->mTransform->Scale = Vector3(1.428053f, 1.132061f, 1.249568f);
}

void MapManager::CreateRock_2()
{
	BTMaterial* mat;

	for (int index = 0; index < 7; ++index)
	{
		mRock_2_Vec.push_back(new GameObject());
		mRock_2_Vec[index]->AddComponent<MeshRenderer>()->SetMeshDatasByStr(L"Rock_2");

	}

	mat = BTResource::GetMaterial(L"Rock_2", 0);
	mat->mProperties->Ambient = Vector4(0.65f, 0.333f, 0.0705f, 1.f);
	mRock_2_Vec[0]->mTransform->Position = Vector3(-6.586156f, -5.618665f, 28.79209f);
	mRock_2_Vec[0]->mTransform->Rotation = Vector3(0, 0, 0);
	mRock_2_Vec[0]->mTransform->Scale = Vector3(1, 1, 1);

	mRock_2_Vec[1]->mTransform->Position = Vector3(18.66384f, -5.128665f, 57.84209f);
	mRock_2_Vec[1]->mTransform->Rotation = Vector3(0, 0, 0);
	mRock_2_Vec[1]->mTransform->Scale = Vector3(1.342681f, 1.34268f, 1.34268f);

	mRock_2_Vec[2]->mTransform->Position = Vector3(-12.67616f, -5.748665f, 27.85209f);
	mRock_2_Vec[2]->mTransform->Rotation = Vector3(-22.165f, 7.329f, -7.452f);
	mRock_2_Vec[2]->mTransform->Scale = Vector3(1.501484f, 1.f, 1.770232f);

	mRock_2_Vec[3]->mTransform->Position = Vector3(30.99163f, -5.449634f, 43.32868f);
	mRock_2_Vec[3]->mTransform->Rotation = Vector3(0, 0, 0);
	mRock_2_Vec[3]->mTransform->Scale = Vector3(1, 1, 1);

	mRock_2_Vec[4]->mTransform->Position = Vector3(31.87384f, -2.948665f, 45.71209f);
	mRock_2_Vec[4]->mTransform->Rotation = Vector3(8.569f, 45.248f, 21.168f);
	mRock_2_Vec[4]->mTransform->Scale = Vector3(1.f, 3.999666f, 1.f);

	mRock_2_Vec[5]->mTransform->Position = Vector3(16.49384f, -6.288665f, 61.77209f);
	mRock_2_Vec[5]->mTransform->Rotation = Vector3(23.934f, 78.687f, -162.78f);
	mRock_2_Vec[5]->mTransform->Scale = Vector3(1.492568f, 1.55731f, 1.558767f);

	mRock_2_Vec[6]->mTransform->Position = Vector3(-16.79f, -5.71f, 28.58f);
	mRock_2_Vec[6]->mTransform->Rotation = Vector3(-30.365f, -3.604f, 7.102f);
	mRock_2_Vec[6]->mTransform->Scale = Vector3(1, 1, 1);

}

void MapManager::CreateRock_3()
{
	BTMaterial* mat;

	for (int index = 0; index < 3; ++index)
	{
		mRock_3_Vec.push_back(new GameObject());
		mRock_3_Vec[index]->AddComponent<MeshRenderer>()->SetMeshDatasByStr(L"Rock_3");

	}

	mat = BTResource::GetMaterial(L"Rock_3", 0);
	mat->mProperties->Ambient = Vector4(0.65f, 0.333f, 0.0705f, 1.f);
	mRock_3_Vec[0]->mTransform->Position = Vector3(-9.796156f, -4.598665f, 29.42209f);
	mRock_3_Vec[0]->mTransform->Rotation = Vector3(-13.564f, 62.751f, -18.866f);
	mRock_3_Vec[0]->mTransform->Scale = Vector3(1.383537f, 1, 1.24f);

	mRock_3_Vec[1]->mTransform->Position = Vector3(19.73384f, -4.448665f, 56.84209f);
	mRock_3_Vec[1]->mTransform->Rotation = Vector3(1.128f, -40.762f, 0.597f);
	mRock_3_Vec[1]->mTransform->Scale = Vector3(1, 1, 1.822774f);

	mRock_3_Vec[2]->mTransform->Position = Vector3(5.383844f, -6.453523f, 1.372093f);
	mRock_3_Vec[2]->mTransform->Rotation = Vector3(12.246f, -22.904f, -175.241f);
	mRock_3_Vec[2]->mTransform->Scale = Vector3(0.7f, 0.7f, 0.7f);
}

void MapManager::CreateRock_4()
{
	BTMaterial* mat;

	for (int index = 0; index < 9; ++index)
	{
		mRock_4_Vec.push_back(new GameObject());
		mRock_4_Vec[index]->AddComponent<MeshRenderer>()->SetMeshDatasByStr(L"Rock_4");

	}

	mat = BTResource::GetMaterial(L"Rock_4", 0);
	mat->mProperties->Ambient = Vector4(0.65f, 0.333f, 0.0705f, 1.f);
	mRock_4_Vec[0]->mTransform->Position = Vector3(21.75385f, -5.428665f, 51.9921f);
	mRock_4_Vec[0]->mTransform->Rotation = Vector3(-0.039f, 160.437f, 11.058f);
	mRock_4_Vec[0]->mTransform->Scale = Vector3(3.045255f, 2.307534f, 5.332306f);

	mRock_4_Vec[1]->mTransform->Position = Vector3(7.913844f, -6.538665f, 2.872093f);
	mRock_4_Vec[1]->mTransform->Rotation = Vector3(-0.9210001f, -50.665f, 0.987f);
	mRock_4_Vec[1]->mTransform->Scale = Vector3(1.424142f, 1.424142f, 2.250016f);

	mRock_4_Vec[2]->mTransform->Position = Vector3(0.1738441f, -6.298665f, 29.41209f);
	mRock_4_Vec[2]->mTransform->Rotation = Vector3(0, -39.574f, 0);
	mRock_4_Vec[2]->mTransform->Scale = Vector3(2.733029f, 1.531004f, 1.553303f);

	mRock_4_Vec[3]->mTransform->Position = Vector3(46.01384f, -4.238666f, 41.5621f);
	mRock_4_Vec[3]->mTransform->Rotation = Vector3(-3.518f, -4.106f, 10.317f);
	mRock_4_Vec[3]->mTransform->Scale = Vector3(2.3434f, 1.532508f, 1.532507f);

	mRock_4_Vec[4]->mTransform->Position = Vector3(14.43598f, -5.833415f, 41.04706f);
	mRock_4_Vec[4]->mTransform->Rotation = Vector3(0, 0, 0);
	mRock_4_Vec[4]->mTransform->Scale = Vector3(1.230283f, 1.230283f, 1.230283f);

	mRock_4_Vec[5]->mTransform->Position = Vector3(22.46384f, -3.588665f, 53.5621f);
	mRock_4_Vec[5]->mTransform->Rotation = Vector3(-14.04f, 266.245f, -2.848f);
	mRock_4_Vec[5]->mTransform->Scale = Vector3(3.193985f, 0.9478975f, 1.568694f);

	mRock_4_Vec[6]->mTransform->Position = Vector3(16.0517f, -5.061715f, 58.94869f);
	mRock_4_Vec[6]->mTransform->Rotation = Vector3(0, 0, 0);
	mRock_4_Vec[6]->mTransform->Scale = Vector3(1, 1, 1);

	mRock_4_Vec[7]->mTransform->Position = Vector3(27.52384f, -3.728665f, 45.71209f);
	mRock_4_Vec[7]->mTransform->Rotation = Vector3(6.221f, 14.171f, -3.663f);
	mRock_4_Vec[7]->mTransform->Scale = Vector3(3.035687f, 2.159647f, 0.9695483f);

	mRock_4_Vec[8]->mTransform->Position = Vector3(13.4f, -3.8f, 80.8f);
	mRock_4_Vec[8]->mTransform->Rotation = Vector3(6.483f, 70.899f, 3.174f);
	mRock_4_Vec[8]->mTransform->Scale = Vector3(3.03569f, 2.15965f, 0.9695498f);
}

void MapManager::CreateRock_5()
{
	BTMaterial* mat;

	for (int index = 0; index < 8; ++index)
	{
		mRock_5_Vec.push_back(new GameObject());
		mRock_5_Vec[index]->AddComponent<MeshRenderer>()->SetMeshDatasByStr(L"Rock_5");

	}

	mat = BTResource::GetMaterial(L"Rock_5", 0);
	mat->mProperties->Ambient = Vector4(0.65f, 0.333f, 0.0705f, 1.f);
	mRock_5_Vec[0]->mTransform->Position = Vector3(40.25385f, -3.548665f, 47.87209f);
	mRock_5_Vec[0]->mTransform->Rotation = Vector3(-12.209f, 85.829f, 169.463f);
	mRock_5_Vec[0]->mTransform->Scale = Vector3(2.621224f, 2.621227f, 2.621225f);

	mRock_5_Vec[1]->mTransform->Position = Vector3(16.5115f, -6.007264f, 18.83619f);
	mRock_5_Vec[1]->mTransform->Rotation = Vector3(0, 0, 0);
	mRock_5_Vec[1]->mTransform->Scale = Vector3(1, 1, 1);

	mRock_5_Vec[2]->mTransform->Position = Vector3(2.083844f, -6.098665f, 30.61209f);
	mRock_5_Vec[2]->mTransform->Rotation = Vector3(3.445f, -89.64101f, -178.148f);
	mRock_5_Vec[2]->mTransform->Scale = Vector3(1, 1, 1);

	mRock_5_Vec[3]->mTransform->Position = Vector3(33.61366f, -5.184267f, 43.13914f);
	mRock_5_Vec[3]->mTransform->Rotation = Vector3(0, 0, 0);
	mRock_5_Vec[3]->mTransform->Scale = Vector3(1, 1, 1);

	mRock_5_Vec[4]->mTransform->Position = Vector3(1.788646f, -5.002096f, 49.734f);
	mRock_5_Vec[4]->mTransform->Rotation = Vector3(0, 0, 0);
	mRock_5_Vec[4]->mTransform->Scale = Vector3(1.790366f, 1.790366f, 1.790366f);

	mRock_5_Vec[5]->mTransform->Position = Vector3(27.91385f, -5.178665f, 44.78209f);
	mRock_5_Vec[5]->mTransform->Rotation = Vector3(-192.491f, -175.758f, 358.638f);
	mRock_5_Vec[5]->mTransform->Scale = Vector3(2.914691f, 2.239363f, 1.367028f);

	mRock_5_Vec[6]->mTransform->Position = Vector3(-21.75616f, -5.098665f, 29.39209f);
	mRock_5_Vec[6]->mTransform->Rotation = Vector3(11.906f, 129.166f, -172.734f);
	mRock_5_Vec[6]->mTransform->Scale = Vector3(1.767423f, 1.767422f, 1.767422f);

	mRock_5_Vec[7]->mTransform->Position = Vector3(4.3f, -4.8f, 71.3f);
	mRock_5_Vec[7]->mTransform->Rotation = Vector3(-190.936f, -139.6f, 366.222f);
	mRock_5_Vec[7]->mTransform->Scale = Vector3(1.361996f, 3.136373f, 1.817344f);
}

void MapManager::CreateRock_6()
{
	BTMaterial* mat;

	for (int index = 0; index < 7; ++index)
	{
		mRock_6_Vec.push_back(new GameObject());
		mRock_6_Vec[index]->AddComponent<MeshRenderer>()->SetMeshDatasByStr(L"Rock_6");

	}

	mat = BTResource::GetMaterial(L"Rock_6", 0);
	mat->mProperties->Ambient = Vector4(0.65f, 0.333f, 0.0705f, 1.f);

	mRock_6_Vec[0]->mTransform->Position = Vector3(-24.01616f, -5.33925f, 32.27209f);
	mRock_6_Vec[0]->mTransform->Rotation = Vector3(-9.586f, 32.662f, 18.055f);
	mRock_6_Vec[0]->mTransform->Scale = Vector3(1.351725f, 1.351725f, 1.351725f);

	mRock_6_Vec[1]->mTransform->Position = Vector3(5.953844f, -6.928665f, 5.132092f);
	mRock_6_Vec[1]->mTransform->Rotation = Vector3(0, 26.533f, 0);
	mRock_6_Vec[1]->mTransform->Scale = Vector3(0.4037803f, 0.2034726f, 0.4710101f);

	mRock_6_Vec[2]->mTransform->Position = Vector3(3.033844f, -6.618666f, 33.6721f);
	mRock_6_Vec[2]->mTransform->Rotation = Vector3(0, 0, 0);
	mRock_6_Vec[2]->mTransform->Scale = Vector3(1, 1, 1.307424f);

	mRock_6_Vec[3]->mTransform->Position = Vector3(49.39384f, -4.418665f, 43.80209f);
	mRock_6_Vec[3]->mTransform->Rotation = Vector3(7.688f, -28.122f, -72.67001f);
	mRock_6_Vec[3]->mTransform->Scale = Vector3(1.303776f, 2.389374f, 1.303776f);

	mRock_6_Vec[4]->mTransform->Position = Vector3(-29.41616f, -5.018665f, 29.11209f);
	mRock_6_Vec[4]->mTransform->Rotation = Vector3(0, 0, 0);
	mRock_6_Vec[4]->mTransform->Scale = Vector3(2.909237f, 2.909237f, 2.909237f);

	mRock_6_Vec[5]->mTransform->Position = Vector3(35.78384f, -5.409546f, 45.87209f);
	mRock_6_Vec[5]->mTransform->Rotation = Vector3(15.344f, 55.863f, 47.923f);
	mRock_6_Vec[5]->mTransform->Scale = Vector3(1.337052f, 1.337052f, 1.337053f);

	mRock_6_Vec[6]->mTransform->Position = Vector3(-4.326156f, -7.938665f, 14.84209f);
	mRock_6_Vec[6]->mTransform->Rotation = Vector3(2.968f, -35.221f, 2.234f);
	mRock_6_Vec[6]->mTransform->Scale = Vector3(1.65015f, 1, 1);
}
