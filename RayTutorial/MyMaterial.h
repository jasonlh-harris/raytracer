#pragma once
#include <grafx.h>
#include "graphics/RayInterfaces.h"
#include <map>
#include "MyMaterial.h"

class CMyMaterial :
	public IMaterial
{
public:
	CMyMaterial(void);
	~CMyMaterial(void);
	void SetEffect(CGrModelX::IEffect *effect){m_effect = effect;}
	const float *GetDiffuse() {return m_effect->GetDiffuse();}
	const float *GetEmissive(){return m_effect->GetEmissive();}
	const float *GetSpecular(){return m_effect->GetSpecular();}
	float GetShininess(){return m_effect->GetShininess();}
	CGrTexture *GetTexture(){return m_effect->GetTexture();}
	const float *CMyMaterial::GetSpecularOther();
	double getEta(void);
	const float *GetTransparency();
	
private:
	CGrModelX::IEffect *m_effect;

};

