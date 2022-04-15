#include <sstream>
#include <fstream>
#include "pch.h"
#include "LoginSceneManager.h"

class LoginSceneManager;

class LoginClient : public ProcessPacket
{
public:
	LoginClient() : type(0), result(0)
	{
		resultEvent = CreateEvent(nullptr, false, false, nullptr);
	}
	~LoginClient()
	{
		CloseHandle(resultEvent);
	}
public:
	LoginSceneManager* loginSceneManager;

public:
	virtual void Receive(SSocket* psSocket, char* buffer)
	{
		SHeader* packet = reinterpret_cast<SHeader*>(buffer);

		switch (packet->usType)
		{
		case PacketType::StringPakcet:
		{
			StringPacket* stringPacket = reinterpret_cast<StringPacket*>(packet);
			istringstream ss(stringPacket->buffer);
			string str;
			string info[3];
			int i = 0;
			int j = 0;
			while (getline(ss, str, ','))
			{
				if (i == 0)
				{
					type = atoi(str.c_str());
				}
				else if (i == 1)
				{
					result = atoi(str.c_str());
				}
				else
				{
					info[j++] = str;
				}

				i++;
			}

			switch (type)
			{
			case 0:		// 회원가입 결과처리
			{
				TextRenderer* signUpLog = loginSceneManager->GetSignupLog();
				switch (result)
				{
					// 회원가입 성공
				case 1:
				{
					signUpLog->SetText(L"회원가입 성공");
					signUpLog->SetColor(Vector4(0, 1, 0, 1));
				}
				break;

				// 이미 존재하는 아이디
				case -1:
				{
					signUpLog->SetText(L"이미 존재하는 아이디입니다.");
					signUpLog->SetColor(Vector4(0, 1, 0, 1));
				}
				break;

				// 회원가입 실패
				case 0:
				{
					signUpLog->SetText(L"회원가입 실패");
					signUpLog->SetColor(Vector4(1, 0, 0, 1));
				}
				break;
				}
			}
			break;

			case 1:		// 로그인 결과처리
			{
				TextRenderer* loginLog = loginSceneManager->GetLoginLog();
				switch (result)
				{
					// 로그인 성공
				case 1:
				{
					loginLog->SetText(L"로그인 성공");
					loginLog->SetColor(Vector4(0, 1, 0, 1));
				}
				break;

				// 아이디 검색 실패
				case -1:
				{
					loginLog->SetText(L"존재하지 않는 아이디입니다.");
					loginLog->SetColor(Vector4(1, 0, 0, 1));
				}
				break;

				// 비밀번호 틀림
				case -2:
				{
					loginLog->SetText(L"비밀번호가 일치하지 않습니다.");
					loginLog->SetColor(Vector4(1, 0, 0, 1));
				}
				break;

				// 로그인 실패
				case 0:
				{
					loginLog->SetText(L"로그인 실패");
					loginLog->SetColor(Vector4(1, 0, 0, 1));
				}
				break;

				// 로그인 실패
				case -3:
				{
					loginLog->SetText(L"이미 로그인 된 아이디입니다.");
					loginLog->SetColor(Vector4(1, 0, 0, 1));
				}
				break;
				}
			}
			break;

			case 2:
			{
				GameDataManager::WIN = atoi(info[0].c_str());
				GameDataManager::LOSE = atoi(info[1].c_str());
				GameDataManager::DRAW = atoi(info[2].c_str());
			}
			break;
			}

			SetEvent(resultEvent);
		}
		break;
		}
	}

public:
	int			type;		// 0 = 회원가입, 1 = 로그인
	int			result;		// 결과
	HANDLE		resultEvent;
};

LoginSceneManager::LoginSceneManager()
{
}

LoginSceneManager::~LoginSceneManager()
{
}

void LoginSceneManager::Init()
{
}

void LoginSceneManager::Start()
{
	std::ifstream	file("../Data/ip.txt");

	if (file.is_open())
	{
		std::string s;
		while (file)
		{
			std::getline(file, s);
			mMGServerIP = s;
		}
	}
	file.close();

	if (GameDataManager::mMGNetwork == nullptr)
	{
		GameDataManager::mMGNetwork = new MGNetwork();
		GameDataManager::mMGNetwork->ConnectServer("Login", 8110, mMGServerIP);
	}

	if (nullptr != GameDataManager::mMGNetwork->m_pPacketSystem)
	{
		delete GameDataManager::mMGNetwork->m_pPacketSystem;
		GameDataManager::mMGNetwork->m_pPacketSystem = nullptr;
	}
	GameDataManager::mMGNetwork->m_pPacketSystem = new LoginClient();

	// 씬매니저를 알고 있어야 로그를 띄울 수 있다.
	LoginClient* loginClient = static_cast<LoginClient*>(GameDataManager::mMGNetwork->m_pPacketSystem);
	loginClient->loginSceneManager = this;

	if (mBackground != nullptr) { return; }
	mBackground = new GameObject();

	mLoginPopupBack = new GameObject();
	mLoginBtn = new GameObject();
	mLocalBtn = new GameObject();
	mSignInBtn = new GameObject();

	mIdTextImg = new GameObject();
	mPasswordTextImg = new GameObject();

	mIdEditBox = new GameObject();
	mPasswordEditBox = new GameObject();

	mLoginPopupLog = new GameObject();
	mSignupPopupLog = new GameObject();

	mBackground->AddComponent<SpriteRenderer>()->SetSpriteByName(L"LoginSceneBackground");
	mBackground->mRectTransform->mRect = Vector4(0, 0, 0, 0);

	mLoginSceneUIVec.push_back(mLoginPopupBack->AddComponent<SpriteRenderer>());

	BTButton* loginButton = mLoginBtn->AddComponent<BTButton>();
	loginButton->SetBasicImgByName(L"Login_Btn_Up");
	loginButton->SetClickImgByName(L"Login_Btn_Up");
	loginButton->SetMouseOnImgByName(L"Login_Btn_Down");
	loginButton->SetClickEvent([&]() { OnLoginButtonClicked(); });

	mLoginSceneUIVec.push_back(mIdTextImg->AddComponent<SpriteRenderer>());
	mLoginSceneUIVec.push_back(mPasswordTextImg->AddComponent<SpriteRenderer>());

	BTButton* localButton = mLocalBtn->AddComponent<BTButton>();
	localButton->SetBasicImgByName(L"LocalPlay_Btn_Up");
	localButton->SetClickImgByName(L"LocalPlay_Btn_Up");
	localButton->SetMouseOnImgByName(L"LocalPlay_Btn_Down");
	localButton->SetClickEvent([&]() { OnLocalButtonClicked(); });

	BTButton* signInButton = mSignInBtn->AddComponent<BTButton>();
	signInButton->SetBasicImgByName(L"SignIn_Btn_up");
	signInButton->SetClickImgByName(L"SignIn_Btn_OnMouse");
	signInButton->SetMouseOnImgByName(L"SignIn_Btn_OnMouse");
	signInButton->SetClickEvent([&]() { OnSignUpButtonClicked(); });

	BTEditBox* idEditBox = mIdEditBox->AddComponent<BTEditBox>();
	BTEditBox* passwordEditBox = mPasswordEditBox->AddComponent<BTEditBox>();

	idEditBox->SetBackImageByName(L"Login_EditBox");
	idEditBox->SetInputLimit(15);
	idEditBox->SetPadding(Vector2(3, 5));

	passwordEditBox->SetBackImageByName(L"Login_EditBox");
	passwordEditBox->SetInputLimit(30);
	passwordEditBox->SetPasswordMode(true);
	passwordEditBox->SetPadding(Vector2(3, 5));

	mLoginSceneUIVec[0]->SetSpriteByName(L"Login_Popup_Back");
	mLoginSceneUIVec[1]->SetSpriteByName(L"ID_Text_Img");
	mLoginSceneUIVec[2]->SetSpriteByName(L"Password_Text_Img");

	mLoginPopupBack->mRectTransform->mRect = Vector4(0.6f, 0.1f, 0.25f, 0.25f);

	mLoginBtn->mRectTransform->mRect = Vector4(0.625f, 0.2625f, 0.465f + 0.13f, 0.465f - 0.13f);
	mLocalBtn->mRectTransform->mRect = Vector4(0.6375f + 0.125f, 0.25f - 0.125f, 0.465f + 0.13f, 0.465f - 0.13f);
	mSignInBtn->mRectTransform->mRect = Vector4(0.75f - 0.05f, 0.25f - 0.05f, 0.49f + 0.21f, 0.49f - 0.21f);

	mIdTextImg->mRectTransform->mRect = Vector4(0.625f, 0.35f, 0.48f - 0.10f, 0.48f + 0.10f);
	mPasswordTextImg->mRectTransform->mRect = Vector4(0.625f, 0.30f, 0.48f - 0.00f, 0.48f + 0.00f);

	mIdEditBox->mRectTransform->mRect = Vector4(0.715f, 0.125f, 0.48f - 0.10f, 0.48f + 0.10f);
	mPasswordEditBox->mRectTransform->mRect = Vector4(0.715f, 0.125f, 0.48f - 0.00f, 0.48f + 0.00f);

	mDimmed = new GameObject();
	SpriteRenderer* dimmed = mDimmed->AddComponent<SpriteRenderer>();
	dimmed->SetSpriteByName(L"Dimmed");
	dimmed->SetSortOrder(15);
	mDimmed->IsActive = false;

	mSignUpPopupBack = new GameObject();
	SpriteRenderer* singupBack = mSignUpPopupBack->AddComponent<SpriteRenderer>();
	singupBack->SetSpriteByName(L"Login_Popup_Back");
	mSignUpPopupBack->mRectTransform->mRect = Vector4(0.05f, 0.55f, 0.25f, 0.25f);
	singupBack->SetSortOrder(17);
	mSignUpPopupBack->IsActive = false;

	mIdEditBox_signup = new GameObject();
	mPasswordEdit_signup = new GameObject();
	mConfirmEdit_signup = new GameObject();

	mSignupPopup_OK_Btn = new GameObject();
	mSignupPopup_Cancel_Btn = new GameObject();

	mIdImg_signup = new GameObject();
	mPasswordImg_signup = new GameObject();
	mConfirmImg_signup = new GameObject();

	SpriteRenderer* idImg_su = mIdImg_signup->AddComponent<SpriteRenderer>();
	SpriteRenderer* passwordImg_su = mPasswordImg_signup->AddComponent<SpriteRenderer>();
	SpriteRenderer* confirmImg_su = mConfirmImg_signup->AddComponent<SpriteRenderer>();

	idImg_su->SetSortOrder(19);	passwordImg_su->SetSortOrder(19);	confirmImg_su->SetSortOrder(19);
	idImg_su->SetSpriteByName(L"ID_Text_Img");
	passwordImg_su->SetSpriteByName(L"Password_Text_Img");
	confirmImg_su->SetSpriteByName(L"Confirm_Text_Img");

	mIdImg_signup->IsActive = false;
	mPasswordImg_signup->IsActive = false;
	mConfirmImg_signup->IsActive = false;

	mIdImg_signup->mRectTransform->mRect = Vector4(0.1f, 0.86f, 0.36f + (0 * 0.08f), 0.60f - (0 * 0.08f));
	mPasswordImg_signup->mRectTransform->mRect = Vector4(0.1f, 0.82f, 0.36f + (1 * 0.08f), 0.60f - (1 * 0.08f));
	mConfirmImg_signup->mRectTransform->mRect = Vector4(0.1f, 0.835f, 0.36f + (2 * 0.08f), 0.60f - (2 * 0.08f));

	mSingupEditBoxVec.push_back(mIdEditBox_signup->AddComponent<BTEditBox>());
	mSingupEditBoxVec.push_back(mPasswordEdit_signup->AddComponent<BTEditBox>());
	mSingupEditBoxVec.push_back(mConfirmEdit_signup->AddComponent<BTEditBox>());

	for (int index = 0; index < (int)mSingupEditBoxVec.size(); ++index)
	{
		mSingupEditBoxVec[index]->SetBackImageByName(L"Login_EditBox");
		mSingupEditBoxVec[index]->mEditBoxImageRender->SetSortOrder(22);
		mSingupEditBoxVec[index]->SetSortOrder(25);
		mSingupEditBoxVec[index]->SetPadding(Vector2(3, 5));

		//0.2f 세로 길이
		mSingupEditBoxVec[index]->gameObject->mRectTransform->mRect
			= Vector4(0.215f, 0.625f, 0.36f + (index * 0.08f), 0.60f - (index * 0.08f));
		mSingupEditBoxVec[index]->gameObject->IsActive = false;
	}
	mSingupEditBoxVec[0]->SetInputLimit(15);
	mSingupEditBoxVec[1]->SetInputLimit(20);
	mSingupEditBoxVec[2]->SetInputLimit(20);
	mSingupEditBoxVec[1]->SetPasswordMode(true);
	mSingupEditBoxVec[2]->SetPasswordMode(true);

	BTButton* signupOkBtn = mSignupPopup_OK_Btn->AddComponent<BTButton>();
	signupOkBtn->SetBasicImgByName(L"Signup_Btn_Up");
	signupOkBtn->SetClickImgByName(L"Signup_Btn_Up");
	signupOkBtn->SetMouseOnImgByName(L"Signup_Btn_Down");
	mSignupPopup_OK_Btn->GetComponent<SpriteRenderer>()->SetSortOrder(30);
	signupOkBtn->SetClickUpEvent([&]() { OnSignUpOkButtonClicked(); });

	BTButton* signupCancelBtn = mSignupPopup_Cancel_Btn->AddComponent<BTButton>();
	signupCancelBtn->SetBasicImgByName(L"Signup_Cancel_Btn_Up");
	signupCancelBtn->SetClickImgByName(L"Signup_Cancel_Btn_Up");
	signupCancelBtn->SetMouseOnImgByName(L"Signup_Cancel_Btn_Down");
	mSignupPopup_Cancel_Btn->GetComponent<SpriteRenderer>()->SetSortOrder(30);
	signupCancelBtn->SetClickUpEvent([&]() { OnSignUpCancelButtonClicked(); });

	mSignupPopup_OK_Btn->mRectTransform->mRect = Vector4(0.1f, 0.77f, 0.465f + 0.15f, 0.465f - 0.15f);
	mSignupPopup_Cancel_Btn->mRectTransform->mRect = Vector4(0.15f + 0.1f, 0.625f, 0.465f + 0.15f, 0.465f - 0.15f);

	mSignupPopup_OK_Btn->IsActive = false;
	mSignupPopup_Cancel_Btn->IsActive = false;

	// LoginLog
	TextRenderer* loginLog = mLoginPopupLog->AddComponent<TextRenderer>();
	loginLog->SetSortOrder(0);
	loginLog->SetSize(1.3f);
	loginLog->SetText(L"회원가입 후 로그인\n또는 로컬 2인 플레이 중 선택해 주세요.");

	// SignupLog
	TextRenderer* signupLog = mSignupPopupLog->AddComponent<TextRenderer>();
	signupLog->SetSortOrder(33);
	signupLog->SetSize(1.3f);
	signupLog->SetColor(Vector4(0, 0, 0, 1));
	signupLog->SetText(L"**영문, 숫자만 입력 가능**");
	mSignupPopupLog->IsActive = false;
}

void LoginSceneManager::Update(float deltaTime)
{
	RECT popupRect = mLoginPopupBack->mRectTransform->GetRectSize();
	mLoginPopupLog->mTransform->Position = Vector3(popupRect.left + 15, popupRect.top + 20, 0);

	RECT signupRect = mSignUpPopupBack->mRectTransform->GetRectSize();
	mSignupPopupLog->mTransform->Position = Vector3(signupRect.left + 15, signupRect.top + 20, 0);
}

void LoginSceneManager::PopupSignUp()
{
	mIdEditBox_signup->GetComponent<BTEditBox>()->ClearText();
	mPasswordEdit_signup->GetComponent<BTEditBox>()->ClearText();
	mConfirmEdit_signup->GetComponent<BTEditBox>()->ClearText();

	bool bOn = !mDimmed->IsActive;
	mDimmed->IsActive = bOn;
	mSignUpPopupBack->IsActive = bOn;

	// 0 ID 
	// 1 Password
	// 2 PasswordConfirm
	for (int index = 0; index < (int)mSingupEditBoxVec.size(); ++index)
	{
		mSingupEditBoxVec[index]->gameObject->IsActive = bOn;
		mSingupEditBoxVec[index]->ClearText();
	}

	mIdImg_signup->IsActive = bOn;
	mPasswordImg_signup->IsActive = bOn;
	mConfirmImg_signup->IsActive = bOn;

	mSignupPopup_OK_Btn->IsActive = bOn;
	mSignupPopup_Cancel_Btn->IsActive = bOn;
	mSignupPopupLog->IsActive = bOn;
}


// 버튼 이벤트
// 로그인 버튼 누르면 실행 됨
void LoginSceneManager::OnLoginButtonClicked()
{
	// 접속된 서버가 없다. 로그인 서버가 없다...
	if (GameDataManager::mMGNetwork->m_ServerList.empty() == true)
	{
		GetLoginLog()->SetText(L"로그인 서버에 접속되지 않았습니다.\n로컬플레이를 이용해 주세요.");
		GetLoginLog()->SetColor(Vector4(1, 0, 0, 1));
		return;
	}

	// 눌렸다.

	// EditBox에 있는 ID와 비번을 받아온다.
	string ID, PASSWORD;
	wstring wID = mIdEditBox->GetComponent<BTEditBox>()->GetText();
	wstring wPASSWORD = mPasswordEditBox->GetComponent<BTEditBox>()->GetText();
	ID.assign(wID.begin(), wID.end());
	PASSWORD.assign(wPASSWORD.begin(), wPASSWORD.end());
	//mPasswordEditBox->GetComponent<BTEditBox>()->GetText().assign(PASSWORD.begin(), PASSWORD.end());

	Packet* packet = new Packet();
	packet->usType = PacketType::StringPakcet;
	char login[64] = { 0, };
	sprintf_s(login, sizeof(login), "1,%s,%s", ID.c_str(), PASSWORD.c_str());

	memcpy_s(packet->buffer, sizeof(packet->buffer), login, sizeof(login));
	GameDataManager::mMGNetwork->SendPakcet(packet, GameDataManager::mMGNetwork->m_ServerList.at("Login"));

	// 패킷을 구성 한다.

	// 보낸다.

	LoginClient* loginResult = reinterpret_cast<LoginClient*>(GameDataManager::mMGNetwork->m_pPacketSystem);
	WaitForSingleObjectEx(loginResult->resultEvent, INFINITE, true);
	if (loginResult->type == 1 && loginResult->result == 1)
	{
		if (GameDataManager::mMGNetwork->m_ServerList.end() == GameDataManager::mMGNetwork->m_ServerList.find("Game"))
		{
			GameDataManager::mMGNetwork->ConnectServer("Game", 9000, mMGServerIP);
		}

		//// 중복 로그인 확인용 패킷 전송
		StringPacket* stringPacket = new StringPacket();
		char check[64] = { 0, };
		sprintf_s(check, sizeof(check), "0,4,%s", ID.c_str());
		memcpy_s(stringPacket->buffer, sizeof(stringPacket->buffer), check, sizeof(check));
		GameDataManager::mMGNetwork->SendPakcet(stringPacket, GameDataManager::mMGNetwork->m_ServerList.at("Game"));
		WaitForSingleObjectEx(loginResult->resultEvent, INFINITE, true);

		if (loginResult->type == 1 && loginResult->result == 1)
		{
			sprintf_s(login, sizeof(login), "2,%s", ID.c_str());
			memcpy_s(stringPacket->buffer, sizeof(stringPacket->buffer), login, sizeof(login));
			GameDataManager::mMGNetwork->SendPakcet(stringPacket, GameDataManager::mMGNetwork->m_ServerList.at("Login"));
			WaitForSingleObjectEx(loginResult->resultEvent, INFINITE, true);
			
			GameDataManager::ID = wID;

			GameDataManager::IsOnlineMode = true;

			// 로비 씬으로 이동 ( 리시브 된 결과 값이 True일 때 실행 됨 )
			BTScene::LoadScene((int)EGunManScene::LobbyScene);
		}
	}
}

// 로컬 플레이 버튼 누르면 실행
void LoginSceneManager::OnLocalButtonClicked()
{
	GetLoginLog()->SetText(L"로컬 2인 플레이 게임을 시작합니다.");
	GetLoginLog()->SetColor(Vector4(0, 1, 0, 1));

	GameDataManager::IsOnlineMode = false;

	BTScene::LoadScene((int)EGunManScene::GameScene);
}

// 회원가입 버튼 누르면 실행
void LoginSceneManager::OnSignUpButtonClicked()
{
	// 접속된 서버가 없다. 로그인 서버가 없다...
	if (GameDataManager::mMGNetwork->m_ServerList.empty() == true)
	{
		GetLoginLog()->SetText(L"로그인 서버에 접속되지 않았습니다.\n회원가입이 불가능 합니다.");
		GetLoginLog()->SetColor(Vector4(1, 0, 0, 1));
		return;
	}

	PopupSignUp();
}

// Signup 가입하기  버튼 클릭
void LoginSceneManager::OnSignUpOkButtonClicked()
{
	// 0 ID 
	if (mSingupEditBoxVec[0]->GetText().empty() == true)
	{
		GetSignupLog()->SetColor(Vector4(1, 0, 0, 1));
		GetSignupLog()->SetText(L"**ID를 입력하세요**");
		return;
	}

	// 1 Password
	if (mSingupEditBoxVec[1]->GetText().empty() == true)
	{
		GetSignupLog()->SetColor(Vector4(1, 0, 0, 1));
		GetSignupLog()->SetText(L"**Password를 입력하세요**");
		return;
	}

	// 2 PasswordConfirm
	if (mSingupEditBoxVec[2]->GetText().empty() == true)
	{
		GetSignupLog()->SetColor(Vector4(1, 0, 0, 1));
		GetSignupLog()->SetText(L"**Password 재확인을 입력하세요**");
		return;
	}

	// 비밀번호와 비밀번호 확인이 다른경우
	if (mSingupEditBoxVec[1]->GetText() != mSingupEditBoxVec[2]->GetText())
	{
		GetSignupLog()->SetColor(Vector4(1, 0, 0, 1));
		GetSignupLog()->SetText(L"**Password가 Confirm과 다릅니다.**");
		return;
	}
	GetSignupLog()->SetColor(Vector4(0, 0, 0, 1));
	GetSignupLog()->SetText(L"**영문, 숫자만 입력 가능**");

	string ID, PASSWORD;
	wstring wID = mSingupEditBoxVec[0]->GetText();
	wstring wPASSWORD = mSingupEditBoxVec[1]->GetText();
	ID.assign(wID.begin(), wID.end());
	PASSWORD.assign(wPASSWORD.begin(), wPASSWORD.end());

	LoginClient* signinResult = reinterpret_cast<LoginClient*>(GameDataManager::mMGNetwork->m_pPacketSystem);
	StringPacket* strPacket = new StringPacket();
	char check[64] = { 0, };
	sprintf_s(check, sizeof(check), "0,%s,%s", ID.c_str(), PASSWORD.c_str());
	memcpy_s(strPacket->buffer, sizeof(strPacket->buffer), check, sizeof(check));
	GameDataManager::mMGNetwork->SendPakcet(strPacket, GameDataManager::mMGNetwork->m_ServerList.at("Login"));

	// 결과 처리 대기
	WaitForSingleObjectEx(signinResult->resultEvent, INFINITE, true);
	if (signinResult->type == 0 && signinResult->result == 1)
	{
		// 회원가입 성공 후...
		PopupSignUp();
	}
}

// 취소하기 버튼 클릭
void LoginSceneManager::OnSignUpCancelButtonClicked()
{
	GetSignupLog()->SetColor(Vector4(0, 0, 0, 1));
	GetSignupLog()->SetText(L"**영문, 숫자만 입력 가능**");

	PopupSignUp();
}


// Get & Set
TextRenderer* LoginSceneManager::GetLoginLog()
{
	if (mLoginPopupLog == nullptr) { return nullptr; }
	return mLoginPopupLog->GetComponent<TextRenderer>();
}

TextRenderer* LoginSceneManager::GetSignupLog()
{
	if (mSignupPopupLog == nullptr) { return nullptr; }
	return mSignupPopupLog->GetComponent<TextRenderer>();
}
