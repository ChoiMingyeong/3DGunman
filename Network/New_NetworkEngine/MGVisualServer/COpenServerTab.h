#pragma once


// COpenServerTab 대화 상자

class COpenServerTab : public CDialogEx
{
	DECLARE_DYNAMIC(COpenServerTab)

public:
	COpenServerTab(CWnd* pParent = nullptr);   // 표준 생성자입니다.
	virtual ~COpenServerTab();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_OPENSERVER_TAB };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
};
