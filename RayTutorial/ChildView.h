
// ChildView.h : interface of the CChildView class
//


#pragma once

#include <grafx.h>
#include "graphics/OpenGLWnd.h"
#include "graphics/GrCamera.h"

// CChildView window

class CChildView : public COpenGLWnd
{
// Construction
public:
	CChildView();

// Attributes
public:

// Operations
public:

// Overrides
	protected:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);

// Implementation
public:
	virtual ~CChildView();
	void reportAntiAliasTotal(int a);

	// Generated message map functions
protected:
	DECLARE_MESSAGE_MAP()

private:
    void LoadModel(const wchar_t *file);

    CGrCamera m_camera;
    CGrModelX m_model;

	bool m_raytrace;
	BYTE **m_rayimage;
	int m_rayimagewidth;
	int m_rayimageheight;
	double m_recursion;
	double m_AdaptiveAntiAliasThreshold;
	bool m_DepthOfFocusOn;
	float m_DepthOfFocus;
	float m_DepthBluriness;
	float m_PenumbraCount;
	float m_AdaptiveRecursionThreshold;
	bool m_Light0;
	bool m_Light1;
	bool m_Light2;
	bool m_Light3;
	bool m_softReflections;
    virtual void OnGLDraw(CDC * pDC);

public:
    afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
    afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
    afx_msg void OnMouseMove(UINT nFlags, CPoint point);
    afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg void OnRenderRaytrace();
	afx_msg void OnUpdateRenderRaytrace(CCmdUI *pCmdUI);
	afx_msg void OnFileSaveimage();
	afx_msg void OnRenderSetantialias();
};

