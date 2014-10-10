#pragma once


// RayTracerOptions dialog

class RayTracerOptions : public CDialog
{
	DECLARE_DYNAMIC(RayTracerOptions)

public:
	RayTracerOptions(CWnd* pParent = NULL);   // standard constructor
	virtual ~RayTracerOptions();

// Dialog Data
	enum { IDD = IDD_OptionsDlg };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	float m_RecursionThreshold;
	float m_AntiAliasThreshold;
	BOOL m_DepthOFFocusEnabled;
	float m_DepthOfFocus;
	float m_DepthBluriness;
	float m_PenumbraCount;
	float m_AdaptiveRecursionThreshold;
	BOOL m_Light0;
	BOOL m_Light1;
	BOOL m_Light2;
	BOOL m_Light3;
	afx_msg void OnEnChangeAdaptiverecursionthreshold();
	BOOL m_softReflections;
};
