// AntiAliasReport.cpp : implementation file
//

#include "stdafx.h"
#include "RayTutorial.h"
#include "AntiAliasReport.h"
#include "afxdialogex.h"


// AntiAliasReport dialog

IMPLEMENT_DYNAMIC(AntiAliasReport, CDialog)

AntiAliasReport::AntiAliasReport(CWnd* pParent /*=NULL*/)
	: CDialog(AntiAliasReport::IDD, pParent)
	, m_AntiAliasReport(_T(""))
{

}

AntiAliasReport::~AntiAliasReport()
{
}

void AntiAliasReport::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_AntiAliasReport, m_AntiAliasReport);
}


BEGIN_MESSAGE_MAP(AntiAliasReport, CDialog)
END_MESSAGE_MAP()


// AntiAliasReport message handlers
