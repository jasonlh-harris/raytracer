#pragma once

#include <grafx.h>
#include <vector>
#include "graphics\GrCamera.h"
#include "graphics\OpenGLWnd.h"
#include "gl/gl.h"
#include "graphics\RayIntersection.h"
#include <list>
#include <map>
#include "MyMaterial.h"
#include <omp.h>

class CMyRaytraceRenderer : public CGrModelX::IRenderer
{
private:
	
	struct Light
	{
		float m_color[4];
		CGrVector m_pos;
		CGrVector m_eyePos;
	};

	std::vector<Light> m_lights;
	std::vector<Light> m_lightsInEyeSpace;
	
	//triangle rendering support
	std::vector<CGrVector> m_vertices;
	std::vector<CGrVector> m_normals;
	std::vector<CGrVector> m_textures;


	//Description of the image to render to
	int m_rayimagewidth;
	int m_rayimageheight;
	BYTE **m_rayimage;
	CGrCamera *m_camera;

	//backround color
	CGrVector background;

	//ambient light
	float m_ambient[4];

	CWnd *m_window;

	float m_focusDepth;
	float m_jitterVal;
	float m_recursionVal;
	float m_AdaptiveAAThreshold;
	float m_AdaptiveRecursionThreshold;
	int m_penumbraCnt;
	int m_totalAntiAlias;

	bool m_softReflections;

	std::vector<CRayIntersection*> m_intersectionVec;
	int THREAD_COUNT;
	
	//Matrix stack
	std::list<CGrTransform> m_mstack;

	//effects map
	std::map<CGrModelX::IEffect *, CMyMaterial> m_materials;
	CGrModelX::IEffect *m_currenteffect;
	CMyMaterial *m_currentmaterial;

	int m_antialias;
public:
    CMyRaytraceRenderer(void);
    virtual ~CMyRaytraceRenderer(void);

    virtual void PushMatrix() {m_mstack.push_back(m_mstack.back());}
    virtual void PopMatrix() {m_mstack.pop_back();}
    virtual void MultMatrix(const CGrTransform &t) {m_mstack.back() *= t;}
    virtual void SetEffect(CGrModelX::IEffect *effect);
    virtual void EndEffect(CGrModelX::IEffect *effect) {}
    virtual void BeginTriangles() { m_vertices.clear(); m_normals.clear(); m_textures.clear();}
    virtual void EndTriangles();
    virtual void TexCoord2fv(float *t) {m_textures.push_back(t);}
    virtual void Normal3fv(float *n) {m_normals.push_back(CGrVector(n[0], n[1], n[2], 0));}
    virtual void Vertex3fv(float *v) {m_vertices.push_back(CGrVector(v[0], v[1], v[2], 1));}
	void SetImage(BYTE **image, int w, int h)
		{m_rayimagewidth = w; m_rayimageheight = h; m_rayimage=image;}
	void SetCamera(CGrCamera *camera){m_camera = camera;}
	void SetAmbient(const float *ambient);
	void AddLight(const float *color, const CGrVector & pos);
	void SetWindow(CWnd *window){m_window = window;}
	void RendererStart();
	void RayTrace();
	CGrVector GetTextureColor(CGrTexture *p_texture, const CGrVector &p_coord);
	CGrVector RayColor(const CRay&, int , const CRayIntersection::Object *);
	void SetBackgroudColor(const CGrVector &color){background = color;}
	void setAntiAlias(int a){m_antialias = a;}
	void setAdaptiveAntiAliasThreshold(double a){m_AdaptiveAAThreshold = a;}
	void setFocusDepth(double d){m_focusDepth = d;}
	void setAdaptiveRecursionThreshold(double r){m_AdaptiveRecursionThreshold = r;}
	void setFocusJitter(double d){m_jitterVal = d;}
	void setMaxRecursion(int d){m_recursionVal = d;}
	void setPenembraCount(int d){m_penumbraCnt = d;}
	void setSetSoftReflections(bool d){m_softReflections = d;}

};