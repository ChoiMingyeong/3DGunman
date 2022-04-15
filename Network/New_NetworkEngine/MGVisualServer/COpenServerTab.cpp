// COpenServerTab.cpp: 구현 파일
//

#include "pch.h"
#include "MGVisualServer.h"
#include "COpenServerTab.h"
#include "afxdialogex.h"


// COpenServerTab 대화 상자

IMPLEMENT_DYNAMIC(COpenServerTab, CDialogEx)

COpenServerTab::COpenServerTab(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_OPENSERVER_TAB, pParent)
{

}

COpenServerTab::~COpenServerTab()
{
}

void COpenServerTab::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(COpenServerTab, CDialogEx)
END_MESSAGE_MAP()


// COpenServerTab 메시지 처리기
