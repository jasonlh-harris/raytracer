
// ChildView.cpp : implementation of the CChildView class
//

#include "stdafx.h"
#include <string>
#include <sstream>
#include "RayTutorial.h"
#include "ChildView.h"
#include <grafx.h>
#include "GlRenderer.h"
#include "MyRaytraceRenderer.h"
#include "RayTracerOptions.h"
#include "AntiAliasReport.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif

using namespace std;

// Define our lights
const float LightAmbientColor[] = {0.2f, 0.2f, 0.2f, 1.0f};


const float Light0Pos[] = {200.f, 150.0f, 80.0f, 1.0f};
const float Light0Color[] = {0.6f, 0.6f, 0.6f, 1.0f};
const float Light1Color[] = {0.6f, 0.6f, 0.6f, 1.0f};
const float Light1Pos[] = {-100.f, 50.0f, 100.0f, 1.0f};

const float Light2Pos[] = {150.f, 200.0f, 80.0f, 1.0f};
const float Light2Color[] = {0.6f, 0.6f, 0.6f, 1.0f};
const float Light3Color[] = {0.6f, 0.6f, 0.6f, 1.0f};
const float Light3Pos[] = {-25.f, 50.0f, 120.0f, 1.0f};


// CChildView

CChildView::CChildView()
{
    m_camera.Set(25, 50, 50, 0, 12, 0, 0, 1, 0);
    m_camera.SetFieldOfView(35);
    SetDoubleBuffer(true);
	m_raytrace = false;
	m_rayimage = NULL;
    //
    // Load our object
    //

	m_recursion = 100;
	m_AdaptiveAntiAliasThreshold = 0.1;
	m_DepthOfFocusOn = true;
	m_DepthOfFocus = 300;
	m_DepthBluriness = 2;
	m_PenumbraCount = 8;
	m_AdaptiveRecursionThreshold = 0.1;

	m_Light0 = true;
	m_Light1 = true;
	m_Light2 = false;
	m_Light3 = false;

	m_softReflections = false;

    LoadModel(L"models/TrayScene.xmodl");



//    m_model.SetCamera(m_camera);
}

CChildView::~CChildView()
{
	if(m_raytrace)
	{
	    delete m_rayimage[0];
        delete m_rayimage;
        m_rayimage = NULL;
	}
}



//
// Name :         CChildView::LoadModel()
// Description :  Load a model to display.
//
void CChildView::LoadModel(const wchar_t *file)
{
    if(!m_model.LoadFile(file))
    {
        wstringstream msg;
        msg << L"Unable to open " << file << " - " << m_model.GetError() << ends;
        AfxMessageBox(msg.str().c_str());
    }

    // Set the camera to the settings from the file
    CGrVector cameraEye = m_model.GetCameraPosition();
    CGrVector cameraCenter = m_model.GetCameraTarget();
    if(cameraEye.W() != 0 && cameraCenter.W() != 0)
    {
        m_camera.SetEye(cameraEye);
        m_camera.SetCenter(cameraCenter);
    }
}


BEGIN_MESSAGE_MAP(CChildView, COpenGLWnd)
	ON_WM_PAINT()
    ON_WM_LBUTTONDOWN()
    ON_WM_RBUTTONDOWN()
    ON_WM_MOUSEMOVE()
    ON_WM_MOUSEWHEEL()
	ON_COMMAND(ID_RENDER_RAYTRACE, &CChildView::OnRenderRaytrace)
	ON_UPDATE_COMMAND_UI(ID_RENDER_RAYTRACE, &CChildView::OnUpdateRenderRaytrace)
	ON_COMMAND(ID_FILE_SAVEIMAGE, &CChildView::OnFileSaveimage)
	ON_COMMAND(ID_RENDER_SETANTIALIAS, &CChildView::OnRenderSetantialias)
END_MESSAGE_MAP()



// CChildView message handlers

BOOL CChildView::PreCreateWindow(CREATESTRUCT& cs) 
{
	if (!COpenGLWnd::PreCreateWindow(cs))
		return FALSE;

	cs.dwExStyle |= WS_EX_CLIENTEDGE;
	cs.style &= ~WS_BORDER;
	cs.lpszClass = AfxRegisterWndClass(CS_HREDRAW|CS_VREDRAW|CS_DBLCLKS, 
		::LoadCursor(NULL, IDC_ARROW), reinterpret_cast<HBRUSH>(COLOR_WINDOW+1), NULL);

	return TRUE;
}



void CChildView::OnGLDraw(CDC * pDC)
{
	if(m_raytrace)
	{
		//clear the color buffer
		glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		//set up for parallel projectoin
		int width, height;
		GetSize(width, height);

		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		glOrtho(0, width, 0, height, -1, 1);
		

		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();

		//if we got it, draw it
		if(m_rayimage)
		{
			glRasterPos3i(0,0,0);
			glDrawPixels(m_rayimagewidth, m_rayimageheight,
						GL_RGB, GL_UNSIGNED_BYTE, m_rayimage[0]);

		}
		return;
	}

    glClearColor(0.3f, 0.5f, 1, 0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    //
    // Set up the camera
    //

    int width, height;
    GetSize(width, height);
    m_camera.Apply(width, height);

    //
    // Some standard parameters
    //

    // Enable depth test
    glEnable(GL_DEPTH_TEST);

    // Cull backfacing polygons
    glCullFace(GL_BACK);
    glEnable(GL_CULL_FACE);

    // Enable lighting
    glEnable(GL_LIGHTING);

    GLfloat black[] = {0, 0, 0, 1.0f};

    // Set ambient light
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, LightAmbientColor);

	if(m_Light0)
	{
		// Set light 0
		glEnable(GL_LIGHT0);
		glLightfv(GL_LIGHT0, GL_POSITION, Light0Pos);
		glLightfv(GL_LIGHT0, GL_DIFFUSE, Light0Color);
		glLightfv(GL_LIGHT0, GL_SPECULAR, Light0Color);
		glLightfv(GL_LIGHT0, GL_AMBIENT, black);
	}
	if(m_Light1)
	{
		// Set light 1
		glEnable(GL_LIGHT1);
		glLightfv(GL_LIGHT1, GL_POSITION, Light1Pos);
		glLightfv(GL_LIGHT1, GL_DIFFUSE, Light1Color);
		glLightfv(GL_LIGHT1, GL_SPECULAR, Light1Color);
		glLightfv(GL_LIGHT1, GL_AMBIENT, black);
	}

	if(m_Light2)
	{
		// Set light2
		glEnable(GL_LIGHT2);
		glLightfv(GL_LIGHT2, GL_POSITION, Light2Pos);
		glLightfv(GL_LIGHT2, GL_DIFFUSE, Light2Color);
		glLightfv(GL_LIGHT2, GL_SPECULAR, Light2Color);
		glLightfv(GL_LIGHT2, GL_AMBIENT, black);
	}

	if(m_Light3)
	{
		// Set light 3
		glEnable(GL_LIGHT3);
		glLightfv(GL_LIGHT3, GL_POSITION, Light3Pos);
		glLightfv(GL_LIGHT3, GL_DIFFUSE, Light3Color);
		glLightfv(GL_LIGHT3, GL_SPECULAR, Light3Color);
		glLightfv(GL_LIGHT3, GL_AMBIENT, black);
	}

    CGlRenderer renderer;

	
	m_model.Draw(&renderer);
	glEnable(GL_NORMALIZE);


    glFlush();
}


void CChildView::OnLButtonDown(UINT nFlags, CPoint point)
{
    m_camera.MouseDown(point.x, point.y);

    COpenGLWnd ::OnLButtonDown(nFlags, point);
}

void CChildView::OnRButtonDown(UINT nFlags, CPoint point)
{
    m_camera.MouseDown(point.x, point.y, 2);

    COpenGLWnd::OnRButtonDown(nFlags, point);
}

void CChildView::OnMouseMove(UINT nFlags, CPoint point)
{
    if(m_camera.MouseMove(point.x, point.y, nFlags))
    {
         Invalidate();
    }

    COpenGLWnd ::OnMouseMove(nFlags, point);
}


BOOL CChildView::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
    m_camera.MouseWheel(zDelta);
    Invalidate();

    return COpenGLWnd::OnMouseWheel(nFlags, zDelta, pt);
}


void CChildView::OnRenderRaytrace()
{
	// TODO: Add your command handler code here
    m_raytrace = !m_raytrace;
    Invalidate();
    if(!m_raytrace)
        return;

    //
    // If an existing image already exists, delete it
    //

    if(m_rayimage != NULL)
    {
        delete m_rayimage[0];
        delete m_rayimage;
        m_rayimage = NULL;
    }


    GetSize(m_rayimagewidth, m_rayimageheight);

    m_rayimage = new BYTE *[m_rayimageheight];

    int rowwid = m_rayimagewidth * 3;
    while(rowwid % 4)
        rowwid++;

    m_rayimage[0] = new BYTE[m_rayimageheight * rowwid];

    for(int i=1; i<m_rayimageheight; i++)
    {
        m_rayimage[i] = m_rayimage[0] + i * rowwid;
    }


    for(int i=0; i<m_rayimageheight; i++)
    {
        // Fill the image with blue
        for(int j=0; j<m_rayimagewidth; j++)
        {
            m_rayimage[i][j * 3] = 0;               // red
            m_rayimage[i][j * 3 + 1] = BYTE(128);   // green
            m_rayimage[i][j * 3 + 2] = 0;           // blue
        }
    }


	//
	//Raytrace the image
	//
	
	//Instantiate the raytrace renderer
	CMyRaytraceRenderer renderer;

	// configure the renderer
	renderer.SetImage(m_rayimage, m_rayimagewidth, m_rayimageheight);
	renderer.SetCamera(&m_camera);
	renderer.SetAmbient(LightAmbientColor);
	if(m_Light0)
		renderer.AddLight(Light0Color, Light0Pos);
	if(m_Light1)
		renderer.AddLight(Light1Color, Light1Pos);
	if(m_Light2)
		renderer.AddLight(Light2Color, Light2Pos);
	if(m_Light3)
		renderer.AddLight(Light3Color, Light3Pos);
	renderer.SetWindow(this);
	renderer.SetBackgroudColor(CGrVector(0.0f, 0.0f, 0.0));

	renderer.setAntiAlias(4);



	renderer.setAdaptiveAntiAliasThreshold(m_AdaptiveAntiAliasThreshold);
	renderer.setFocusDepth(m_DepthOfFocus);
	m_DepthOfFocusOn ? m_DepthBluriness += 0 : m_DepthBluriness = 0;
	renderer.setFocusJitter(m_DepthBluriness);
	renderer.setPenembraCount(m_PenumbraCount);
	renderer.setMaxRecursion(m_recursion);
	renderer.setAdaptiveRecursionThreshold(m_AdaptiveRecursionThreshold);
	renderer.setSetSoftReflections(m_softReflections);

	renderer.RendererStart();

	// Render the scene
	m_model.Draw(&renderer);

	//tell it to do the actual ray tracing now
	renderer.RayTrace();


	Invalidate();
}


void CChildView::OnUpdateRenderRaytrace(CCmdUI *pCmdUI)
{
	// TODO: Add your command update UI handler code here
	pCmdUI->SetCheck(m_raytrace);
}


void CChildView::OnFileSaveimage()
{
	// TODO: Add your command handler code here
	COpenGLWnd::OnSaveImage();
}


void CChildView::OnRenderSetantialias()
{
	RayTracerOptions dlg;

	dlg.m_AntiAliasThreshold = m_AdaptiveAntiAliasThreshold;
	dlg.m_RecursionThreshold = m_recursion;
	dlg.m_DepthOFFocusEnabled = m_DepthOfFocusOn;
	dlg.m_DepthOfFocus= m_DepthOfFocus;
	dlg.m_DepthBluriness = m_DepthBluriness;
	dlg.m_PenumbraCount = m_PenumbraCount;
	dlg.m_AdaptiveRecursionThreshold = m_AdaptiveRecursionThreshold;
	dlg.m_Light0 = m_Light0;
	dlg.m_Light1 = m_Light1;
	dlg.m_Light2 = m_Light2;
	dlg.m_Light3 = m_Light3;
	dlg.m_softReflections = m_softReflections;
	if(dlg.DoModal() == IDOK)
	{
		m_AdaptiveAntiAliasThreshold = dlg.m_AntiAliasThreshold;
		m_recursion = dlg.m_RecursionThreshold;
		m_DepthOfFocusOn = dlg.m_DepthOFFocusEnabled;
		m_DepthOfFocus = dlg.m_DepthOfFocus;
		m_DepthBluriness = dlg.m_DepthBluriness;
		m_PenumbraCount = dlg.m_PenumbraCount;
		m_AdaptiveRecursionThreshold = dlg.m_AdaptiveRecursionThreshold;
		m_Light0 = dlg.m_Light0;
		m_Light1 = dlg.m_Light1;
		m_Light2 = dlg.m_Light2;
		m_Light3 = dlg.m_Light3;
		m_softReflections = dlg.m_softReflections;
		Invalidate();
	}



	// TODO: Add your command handler code here

}

void CChildView::reportAntiAliasTotal(int a)
{
	AntiAliasReport dlg;
	dlg.m_AntiAliasReport = L"The Current Anti Alias Value is: ";
	Invalidate();
}
