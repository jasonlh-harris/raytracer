#pragma once


// AntiAliasReport dialog

class AntiAliasReport : public CDialog
{
	DECLARE_DYNAMIC(AntiAliasReport)

public:
	AntiAliasReport(CWnd* pParent = NULL);   // standard constructor
	virtual ~AntiAliasReport();

// Dialog Data
	enum { IDD = IDD_DIALOG1 };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CString m_AntiAliasReport;
};
