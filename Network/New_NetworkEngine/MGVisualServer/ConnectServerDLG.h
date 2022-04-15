#pragma once


// ConnectServerDLG 대화 상자

class ConnectServerDLG : public CDialogEx
{
	DECLARE_DYNAMIC(ConnectServerDLG)

public:
	ConnectServerDLG(CWnd* pParent = nullptr);   // 표준 생성자입니다.
	virtual ~ConnectServerDLG();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ConnectServerDLG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
};
