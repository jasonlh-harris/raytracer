#include "StdAfx.h"
#include "MyMaterial.h"


CMyMaterial::CMyMaterial(void)
{
}


CMyMaterial::~CMyMaterial(void)
{
}


double CMyMaterial::getEta(void)
{
    if(!wcscmp(m_effect->GetName(), L"GreenGlass"))
    {
		return 1.52;
	}
	else
		return 1;
}

const float *CMyMaterial::GetTransparency()
{
    if(!wcscmp(m_effect->GetName(), L"GreenGlass"))
    {
        // Must be static or a member variable for the return to work
        static float glass[] =  {0.3f, 0.66f, 0.81f, 1};
        return glass;
    }

    static float black[] = {0, 0, 0, 0};
    return black;
}
const float *CMyMaterial::GetSpecularOther()
{
    if(wcscmp(m_effect->GetName(), L"Tray") == 0)
    {
        // Must be static or a member variable for the return to work
        static float tray[] =  {0.5f, 0.5f, 0.5f, 1};
        return tray;
    }

	if(wcscmp(m_effect->GetName(), L"GreenGlass") == 0)
	{
		static float GreenGlass[] = { 0.2f, 0.4f, 0.2f, 1.f};
		return GreenGlass;
	}
	
	if(wcscmp(m_effect->GetName(), L"CandleHolder") == 0)
	{
		static float CandleHolder[] = {0.2f, 0.15f, 0.15f , 1.f};
		return CandleHolder;
	}

    static float black[] = {0, 0, 0, 0};
    return black;
}