// RayTracerOptions.cpp : implementation file
//

#include "stdafx.h"
#include "RayTutorial.h"
#include "RayTracerOptions.h"
#include "afxdialogex.h"


// RayTracerOptions dialog

IMPLEMENT_DYNAMIC(RayTracerOptions, CDialog)

RayTracerOptions::RayTracerOptions(CWnd* pParent /*=NULL*/)
	: CDialog(RayTracerOptions::IDD, pParent)
	, m_DepthBluriness(0)
	, m_PenumbraCount(0)
	, m_RecursionThreshold(0)
	, m_AntiAliasThreshold(0)
	, m_DepthOFFocusEnabled(FALSE)
	, m_DepthOfFocus(0)
	, m_AdaptiveRecursionThreshold(0)
	, m_Light0(FALSE)
	, m_Light1(FALSE)
	, m_Light2(FALSE)
	, m_Light3(FALSE)
	, m_softReflections(FALSE)
{

}

RayTracerOptions::~RayTracerOptions()
{
}

void RayTracerOptions::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_FocusBluriness, m_DepthBluriness);
	DDX_Text(pDX, IDC_PenumbraLevel, m_PenumbraCount);
	DDX_Text(pDX, IDC_RecursionLevel, m_RecursionThreshold);
	DDX_Text(pDX, IDC_AntiAliasThreshold, m_AntiAliasThreshold);
	DDX_Check(pDX, IDC_DepthEnable, m_DepthOFFocusEnabled);
	DDX_Text(pDX, IDC_DepthOfFocus, m_DepthOfFocus);
	DDX_Text(pDX, IDC_FocusBluriness, m_DepthBluriness);
	DDX_Text(pDX, IDC_PenumbraLevel, m_PenumbraCount);
	DDX_Text(pDX, IDC_AdaptiveRecursionThreshold, m_AdaptiveRecursionThreshold);
	DDX_Check(pDX, IDC_EnableLight0, m_Light0);
	DDX_Check(pDX, IDC_EnableLight1, m_Light1);
	DDX_Check(pDX, IDC_EnableLight2, m_Light2);
	DDX_Check(pDX, IDC_EnableLight3, m_Light3);
	DDX_Check(pDX, IDC_EnableSoftReflections, m_softReflections);
}


BEGIN_MESSAGE_MAP(RayTracerOptions, CDialog)
	ON_EN_CHANGE(IDC_AdaptiveRecursionThreshold, &RayTracerOptions::OnEnChangeAdaptiverecursionthreshold)
END_MESSAGE_MAP()


// RayTracerOptions message handlers


void RayTracerOptions::OnEnChangeAdaptiverecursionthreshold()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
}
