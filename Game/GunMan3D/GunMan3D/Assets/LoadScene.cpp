#include "pch.h"
#include "LoadScene.h"

LoadScene::LoadScene()
{
}

LoadScene::~LoadScene()
{
}

void LoadScene::Start()
{
	//BTLoader::LoadTextureFromDDS(L"GunmanSkyBox",		L"../Data/Skybox/GunmanSkyBox.dds");
	BTLoader::LoadTextureFromDDS(L"GunmanSkyBox2",		L"../Data/Skybox/GunmanSkyBox2.dds");

	// 로그인
	BTLoader::LoadTexturePng(L"Login_Popup_Back",		L"../Data/UI_IMG/Login_Popup_Back.png");
	BTLoader::LoadTexturePng(L"ID_Text_Img",			L"../Data/UI_IMG/ID_Text_Img.png");
	BTLoader::LoadTexturePng(L"Password_Text_Img",		L"../Data/UI_IMG/Password_Text_Img.png");
	BTLoader::LoadTexturePng(L"Confirm_Text_Img",		L"../Data/UI_IMG/Confirm_Text_Img.png");
	BTLoader::LoadTexturePng(L"Login_EditBox",			L"../Data/UI_IMG/Login_EditBox.png");
	BTLoader::LoadTexturePng(L"Login_Btn_Up",			L"../Data/UI_IMG/Login_Btn_Up.png");
	BTLoader::LoadTexturePng(L"Login_Btn_Down",			L"../Data/UI_IMG/Login_Btn_Down.png");
	BTLoader::LoadTexturePng(L"LoginSceneBackground",	L"../Data/UI_IMG/LoginSceneBackground.png");
	BTLoader::LoadTexturePng(L"SignIn_Btn_up",			L"../Data/UI_IMG/SignIn_Btn_up.png");
	BTLoader::LoadTexturePng(L"SignIn_Btn_OnMouse",		L"../Data/UI_IMG/SignIn_Btn_OnMouse.png");
	BTLoader::LoadTexturePng(L"LocalPlay_Btn_Down",		L"../Data/UI_IMG/LocalPlay_Btn_Down.png");
	BTLoader::LoadTexturePng(L"LocalPlay_Btn_Up",		L"../Data/UI_IMG/LocalPlay_Btn_Up.png");
	BTLoader::LoadTexturePng(L"Signup_Btn_Down",		L"../Data/UI_IMG/Signup_Btn_Down.png");
	BTLoader::LoadTexturePng(L"Signup_Btn_Up",			L"../Data/UI_IMG/Signup_Btn_Up.png");
	BTLoader::LoadTexturePng(L"Signup_Cancel_Btn_Down", L"../Data/UI_IMG/Signup_Cancel_Btn_Down.png");
	BTLoader::LoadTexturePng(L"Signup_Cancel_Btn_Up",   L"../Data/UI_IMG/Signup_Cancel_Btn_Up.png");
	BTLoader::LoadTexturePng(L"Dimmed",					L"../Data/UI_IMG/Dimmed.png");

	// 로비
	BTLoader::LoadTexturePng(L"LobbySceneBackground",	L"../Data/UI_IMG/LobbySceneBackground.png");
	BTLoader::LoadTexturePng(L"AccessListBackground",	L"../Data/UI_IMG/AccessListBackground.png");
	BTLoader::LoadTexturePng(L"ProfileBackground",		L"../Data/UI_IMG/ProfileBackground.png");
	BTLoader::LoadTexturePng(L"RoomBackground",			L"../Data/UI_IMG/RoomBackground.png");

	BTLoader::LoadTexturePng(L"RoomList_Btn_UP",		L"../Data/UI_IMG/RoomList_Btn_UP.png");
	BTLoader::LoadTexturePng(L"RoomList_Btn_DOWN",		L"../Data/UI_IMG/RoomList_Btn_DOWN.png");
	BTLoader::LoadTexturePng(L"AccessList_Btn_UP",		L"../Data/UI_IMG/AccessList_Btn_UP.png");
	BTLoader::LoadTexturePng(L"AccessList_Btn_DOWN",	L"../Data/UI_IMG/AccessList_Btn_DOWN.png");

	BTLoader::LoadTexturePng(L"RoomList_Btn_UP_PUSH",		L"../Data/UI_IMG/RoomList_Btn_UP_PUSH.png");
	BTLoader::LoadTexturePng(L"RoomList_Btn_DOWN_PUSH",		L"../Data/UI_IMG/RoomList_Btn_DOWN_PUSH.png");
	BTLoader::LoadTexturePng(L"AccessList_Btn_UP_PUSH",		L"../Data/UI_IMG/AccessList_Btn_UP_PUSH.png");
	BTLoader::LoadTexturePng(L"AccessList_Btn_DOWN_PUSH",	L"../Data/UI_IMG/AccessList_Btn_DOWN_PUSH.png");

	BTLoader::LoadTexturePng(L"RoomEnterButton",		L"../Data/UI_IMG/RoomEnterButton.png");
	BTLoader::LoadTexturePng(L"RoomEnterButton_PUSH",	L"../Data/UI_IMG/RoomEnterButton_PUSH.png");
	BTLoader::LoadTexturePng(L"SmallRoomBackground",	L"../Data/UI_IMG/SmallRoomBackground.png");

	BTLoader::LoadTexturePng(L"AccessListLabelBackground",	L"../Data/UI_IMG/AccessListLabelBackground.png");
	BTLoader::LoadTexturePng(L"ProfileLabelBackground",		L"../Data/UI_IMG/ProfileLabelBackground.png");

	//게임
	BTLoader::LoadTexturePng(L"Ready_Off",		L"../Data/UI_IMG/Ready_Off.png");
	BTLoader::LoadTexturePng(L"Ready_On",		L"../Data/UI_IMG/Ready_On.png");
	BTLoader::LoadTexturePng(L"Fire",			L"../Data/UI_IMG/Fire.png");
	BTLoader::LoadTexturePng(L"Hold",			L"../Data/UI_IMG/Hold.png");
	BTLoader::LoadTexturePng(L"Defeated",		L"../Data/UI_IMG/Defeated.png");
	BTLoader::LoadTexturePng(L"Win",			L"../Data/UI_IMG/Win.png");
	BTLoader::LoadTexturePng(L"Draw",			L"../Data/UI_IMG/Draw.png");


	// 3D 배경
	BTLoader::LoadModelDataSet(L"03IK-Joe",					L"../Data/ASE/03IK-Joe.ASE");
	BTLoader::LoadModelDataSet(L"Enviroment_1",				L"../Data/ASE/UseData/Enviroment_1.ASE");
	BTLoader::LoadModelDataSet(L"Mounting_1",				L"../Data/ASE/UseData/Mounting_1.ASE");
	BTLoader::LoadModelDataSet(L"Mounting_2",				L"../Data/ASE/UseData/Mounting_2.ASE");

	BTLoader::LoadModelDataSet(L"Rock_1", L"../Data/ASE/UseData/Rock_1.ASE");
	BTLoader::LoadModelDataSet(L"Rock_2", L"../Data/ASE/UseData/Rock_2.ASE");
	BTLoader::LoadModelDataSet(L"Rock_3", L"../Data/ASE/UseData/Rock_3.ASE");
	BTLoader::LoadModelDataSet(L"Rock_4", L"../Data/ASE/UseData/Rock_4.ASE");
	BTLoader::LoadModelDataSet(L"Rock_5", L"../Data/ASE/UseData/Rock_5.ASE");
	BTLoader::LoadModelDataSet(L"Rock_6", L"../Data/ASE/UseData/Rock_6.ASE");

	// 에러나면 안넘어가야하는데.. 일단 미루고
	// Start이후 LoginScene으로 이동
	BTScene::LoadScene((int)EGunManScene::LoginScene);
}
