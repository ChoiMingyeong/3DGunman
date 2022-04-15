// CTotalTab.cpp: 구현 파일
//

#include "pch.h"
#include "MGVisualServer.h"
#include "CTotalTab.h"
#include "afxdialogex.h"


// CTotalTab 대화 상자

IMPLEMENT_DYNAMIC(CTotalTab, CDialogEx)

CTotalTab::CTotalTab(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_MGVISUALSERVER_DIALOG, pParent)
{

}

CTotalTab::~CTotalTab()
{
}

void CTotalTab::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CTotalTab, CDialogEx)
END_MESSAGE_MAP()


// CTotalTab 메시지 처리기
