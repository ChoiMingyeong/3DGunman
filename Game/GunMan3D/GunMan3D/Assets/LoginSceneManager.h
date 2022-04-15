#pragma once
class LoginSceneManager : public BTBehaviour
{
public:
	LoginSceneManager();
	virtual ~LoginSceneManager();

public:
	virtual void Init();
	virtual void Start();
	virtual void Update(float deltaTime);

	void PopupSignUp();

	void OnLoginButtonClicked();
	void OnLocalButtonClicked();

	void OnSignUpButtonClicked();
	void OnSignUpOkButtonClicked();
	void OnSignUpCancelButtonClicked();


public:
	TextRenderer* GetLoginLog();
	TextRenderer* GetSignupLog();

private:
	GameObject* mBackground;

	// 로그인 팝업
	GameObject* mLoginPopupBack;
	GameObject* mLoginBtn;
	GameObject* mLocalBtn;
	GameObject* mSignInBtn;

	GameObject* mIdTextImg;
	GameObject* mPasswordTextImg;

	GameObject* mIdEditBox;
	GameObject* mPasswordEditBox;

	GameObject* mLoginPopupLog;
	GameObject* mSignupPopupLog;

	// 회원 가입 팝업
	GameObject* mDimmed;
	GameObject* mSignUpPopupBack;

	GameObject* mIdEditBox_signup;		// 0 아이디
	GameObject* mPasswordEdit_signup;	// 1 비밀번호
	GameObject* mConfirmEdit_signup;	// 2 비밀번호 확인

	GameObject* mIdImg_signup;
	GameObject* mPasswordImg_signup;
	GameObject* mConfirmImg_signup;

	GameObject* mSignupPopup_OK_Btn;
	GameObject* mSignupPopup_Cancel_Btn;

	vector<BTEditBox*> mSingupEditBoxVec;

	vector<SpriteRenderer*> mLoginSceneUIVec;

	string		mMGServerIP;
};

