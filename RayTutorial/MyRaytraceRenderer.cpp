#include "StdAfx.h"
#include "MyRaytraceRenderer.h"
#include "Poisson2D.h"
#include <iostream>
#include <fstream>


/******
*
*	DISCLAIMER:
*	Use of this file, in whole or in part must be credited to both
*	Dr. Owen of Michigan State University who gave the tutorial and framework codes
*	and Jason Harris who implemented the ray tracer.
*
*	Any use of this file must include this in it's credits with any changes the other
*	author(s) make documented below:
*
*	Any use of this file without proper accredation will be punished.
*
*
*****/


/******
*	DOCUMENTED CHANGES:
*	CHANGE	DATE	AUTHOR
******/


//
// Name:	ByteRange()
// Description: Convert a value in the range 0 to 1 to a 
//				BYTE value with range bounding
//
inline BYTE ByteRange(double d)
{
	if(d > 1)
		return 255;
	else if( d < 0)
		return 0;
	return BYTE(255 * d);
}

inline double DotProduct(const CGrVector &a, const CGrVector &b)
{
	return a[0] * b[0] + a[1] * b[1] + a[2] * b[2];
}

inline double Magnitude(const CGrVector &a)
{
	return sqrt((a[0] * a[0] + a[1] * a[1] + a[2] * a[2]));
}

inline double DotProd(const CGrVector &a, const CGrVector &b)
{
	double sum = 0;
	sum += a[0] * b[0] + a[1] * b[1] + a[2] * b[2];
	sum /= (Magnitude(a) + Magnitude(b));
	return sum;
}



inline CGrVector Mult(const CGrVector &a, const CGrVector &b)
{
    return CGrVector(a[0] * b[0], a[1] * b[1], a[2] * b[2]);
}

CMyRaytraceRenderer::CMyRaytraceRenderer(void)
{
	m_camera = NULL;
	m_currenteffect = NULL;
	m_currentmaterial = NULL;
	m_antialias = 4;
	m_AdaptiveAAThreshold = 0.1;
	m_AdaptiveRecursionThreshold = 0.1;
	m_penumbraCnt = 8;
	THREAD_COUNT = omp_get_max_threads();
	m_focusDepth = 300;
	m_jitterVal = 0;
	m_recursionVal = 7;
	m_totalAntiAlias = 0;
	m_softReflections = false;
	for( int i = 0; i < THREAD_COUNT; i++)
		m_intersectionVec.push_back(new CRayIntersection);
}


CMyRaytraceRenderer::~CMyRaytraceRenderer(void)
{
}

void CMyRaytraceRenderer::SetAmbient(const float *ambient)
{
	for(int i = 0; i < 4; i ++)
		m_ambient[i] = ambient[i];
}

void CMyRaytraceRenderer::AddLight(const float *color, const CGrVector & pos)
{
	Light light;
	for(int i = 0; i < 4; i++)
		light.m_color[i] = color[i];
	light.m_pos = pos;
	m_lights.push_back(light);
}
void CMyRaytraceRenderer::RendererStart()
{
	#pragma omp parallel for num_threads(THREAD_COUNT)
	for(int i = 0; i < THREAD_COUNT; i++)
		m_intersectionVec[i]->Initialize();

	// we have to do all of the matrix work ourselves.
	// set up the matrix stack
	CGrTransform t;
	t.SetLookAt(m_camera->GetEyeX(), m_camera->GetEyeY(), m_camera->GetEyeZ(),
				m_camera->GetCenterX(), m_camera->GetCenterY(), m_camera->GetCenterZ(),
				m_camera->GetUpX(), m_camera->GetUpY(), m_camera->GetUpZ());
	m_mstack.push_back(t);
	for(std::vector<Light>::iterator it = m_lights.begin(); it != m_lights.end(); ++it)
	{
		(*it).m_eyePos = (m_mstack.back() * (*it).m_pos);
		m_lightsInEyeSpace.push_back((*it));
	}
}

void CMyRaytraceRenderer::SetEffect(CGrModelX::IEffect *effect)
{
	//if the effect does not change, we don't need to do anything
	if(effect == m_currenteffect)
		return;

	//make this the current effect
	m_currenteffect = effect;

	//find the effect in the map
	std::map<CGrModelX::IEffect *, CMyMaterial>::iterator f = m_materials.find(effect);
	if(f == m_materials.end())
	{
		//the effect was not found, create a new item
		CMyMaterial mat;
		mat.SetEffect(effect);
		m_materials[effect] = mat;
		m_currentmaterial = &m_materials[effect];

	}
	else
	{
		// we found thematerial in the map
		m_currentmaterial = &f->second;
	}
}

void CMyRaytraceRenderer::EndTriangles()
{
	#pragma omp parallel for num_threads(THREAD_COUNT)
	for(int i = 0; i < THREAD_COUNT; i++)
	{
		std::vector<CGrVector>::const_iterator normal = m_normals.begin();
		std::vector<CGrVector>::const_iterator vertex = m_vertices.begin();
		std::vector<CGrVector>::const_iterator textures = m_textures.begin();

    // Triangles are groups of three vertices and normals
    // This loops t over them in groups of 3

		for(int t=0;  t<(int)m_vertices.size() / 3; t++)
		{
			// Allocate a new triangle in the ray intersection system
			m_intersectionVec[i]->TriangleBegin();
			m_intersectionVec[i]->Material(m_currentmaterial);

			for(int j=0;  j<3;  j++)
			{
				// This allows for fewer normals than vertices
				if(normal != m_normals.end())
				{
					CGrVector transformedNormal = m_mstack.back() * *normal;
					m_intersectionVec[i]->Normal(transformedNormal);
					normal++;
				}

				CGrVector transformedVertex = m_mstack.back() * *vertex;
				m_intersectionVec[i]->Vertex(transformedVertex);
				vertex++;

				if(textures != m_textures.end())
				{
					m_intersectionVec[i]->TexVertex(*textures);
					textures++;
				}
			}

			m_intersectionVec[i]->TriangleEnd();
		}
	}
}

void CMyRaytraceRenderer::RayTrace()
{
	#pragma omp parallel for num_threads(THREAD_COUNT)
	for(int i = 0; i < THREAD_COUNT; i ++)
		m_intersectionVec[i]->LoadingComplete();
	const double PI = 3.1415926535;
	double FoV = m_camera->GetFieldOfView() * PI / 180;
	double aspectRatio = double(m_rayimagewidth) / double(m_rayimageheight);
	double ymin = tan(FoV /2);
	double yhit = 2 * ymin;
	double xmin = ymin * aspectRatio;
	double xwid = 2 * xmin;
	int max;
	std::vector<int> maxVec;
	std::vector<int> TotalAntiAliasVec;
	for( int i = 0; i < THREAD_COUNT; i++)
	{
		TotalAntiAliasVec.push_back(0);
		maxVec.push_back(0);
	}
	#pragma omp parallel for num_threads(THREAD_COUNT)
	for(int i = 0; i < THREAD_COUNT; i++)
	{
		for(int r=0;  r<m_rayimageheight;  r++)
		{
			for(int c=i;  c<m_rayimagewidth;  c+= THREAD_COUNT)
			{
				CPoisson2D poisson;
				CPoisson2D DepthPoisson;

				bool adapting = true;
				int curAlias = 0;
				int curAliasForPoisson = 1;
				double colorTotal[3] = {0, 0, 0};
				while(adapting)
				{
					m_totalAntiAlias ++;
					curAlias += 1;

					poisson.SetMinDistance(curAliasForPoisson);
					DepthPoisson.SetMinDistance(curAliasForPoisson);
					curAliasForPoisson += 2;

					poisson.Generate();
					DepthPoisson.GenerateHeart();

					double x = -xmin + (c + poisson.X()) / m_rayimagewidth * xwid;
					double y = -ymin + (r + poisson.Y()) / m_rayimageheight * yhit;
					// Construct a ray
					CRay ray(CGrVector(0, 0, 0), Normalize3(CGrVector(x, y, -1, 0)));

					CGrVector FocalPoint = Normalize3(CGrVector(x, y, -1, 0)) * m_focusDepth;
					CGrVector StartingPoint = CGrVector( m_jitterVal* DepthPoisson.X(), m_jitterVal * DepthPoisson.Y(),0 );

					CRay jitteredRay(StartingPoint, Normalize3(FocalPoint - StartingPoint));


					CGrVector color = RayColor(jitteredRay, m_recursionVal, NULL);

					if(curAlias % 2 ==0 &&
						!(color.X() > m_AdaptiveAAThreshold*colorTotal[0] ||
						color.Y() > m_AdaptiveAAThreshold*colorTotal[1] ||
						color.Z() > m_AdaptiveAAThreshold*colorTotal[2]))
					{
						adapting = false;
					}


					colorTotal[0] += color.X();
					colorTotal[1] += color.Y();
					colorTotal[2] += color.Z();

				}
				if (curAlias > maxVec[omp_get_thread_num()])
					maxVec[omp_get_thread_num()] = curAlias;
				m_rayimage[r][c * 3] = ByteRange(colorTotal[0] / curAlias);
				m_rayimage[r][c * 3 + 1] = ByteRange(colorTotal[1] / curAlias);
				m_rayimage[r][c * 3 + 2] = ByteRange(colorTotal[2] / curAlias);
				TotalAntiAliasVec[omp_get_thread_num()] += curAlias;

			}
			if(omp_get_thread_num() == 0)
			{
				m_window->Invalidate();
				m_window->UpdateWindow();
				MSG msg;
				while(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
					DispatchMessage(&msg);
			}
		}
	}

	m_totalAntiAlias = 0;
	std::ofstream theFile;
	theFile.open("Average Anti Alias");
	for(int i = 0; i < THREAD_COUNT; i++)
	{
		theFile << "****************************************************************" << std::endl;
		theFile << "Statistics for thread: " << i << std::endl;
		theFile << "The Count for thread is: " << TotalAntiAliasVec[i] << std::endl;
		theFile << "The max for thread is: " << maxVec[i] << std::endl;
		m_totalAntiAlias += TotalAntiAliasVec[i];
		theFile << "The average for thread is: " << TotalAntiAliasVec[i] / (m_rayimageheight * (m_rayimagewidth / THREAD_COUNT)) << std::endl;
	}
	m_totalAntiAlias /= (m_rayimageheight * m_rayimagewidth);
	theFile << "******************************************************************" << std::endl;
	theFile << "Total combined statistics" << std::endl;
	theFile << "The Avrage is: " << m_totalAntiAlias << std::endl;
	theFile.close();

}

CGrVector CMyRaytraceRenderer::RayColor(const CRay & ray,
										int recurse, const CRayIntersection::Object *ignore)
{


	if (recurse < 0)
		return CGrVector(0,0,0);

    double t;                                   // Will be distance to intersection
    CGrVector intersect;                         // Will by x,y,z location of intersection
    const CRayIntersection::Object *nearest;    // Pointer to intersecting object
	CPoisson2D poisson;
	double lightArea = 16;
	poisson.setMax(lightArea);
	poisson.SetMinDisatncePoisson( lightArea / (2 * sqrt(double(m_penumbraCnt))));
    if(!m_intersectionVec[omp_get_thread_num()]->Intersect(ray, 1e20, ignore, nearest, t, intersect))
    {
        // We didn't hit anything.
        // Return the background color
		if (recurse <= 1)
		{
			float black[] = {0, 0, 0, 0};
			return black;
		}
        return background;
    }

    // Determine information about the intersection
    CGrVector N;
    IMaterial *material;
    ITexture *texture;
    CGrVector texcoord;

    m_intersectionVec[omp_get_thread_num()]->IntersectInfo(ray, nearest, t, N, material, texture, texcoord);          // White
	CMyMaterial *mat = (CMyMaterial *)material;

	CGrVector diffuse = mat->GetDiffuse();
	CGrVector V = -ray.Direction();

	//CGrVector colorChange(0,0,0);

	// If we have a texture, determine the texture color here
    if(mat->GetTexture() != NULL)
    {
        CGrTexture *texture = mat->GetTexture();
        CGrVector tcolor = GetTextureColor(texture, texcoord);
        for(int c=0;  c<3;  c++)
			diffuse[c] *= tcolor[c];
    }


	//Color value, starting with emissive and ambient
	CGrVector color = CGrVector(mat->GetEmissive()) + Mult(m_ambient, diffuse);
	CGrVector colorChange(0,0,0);
	if (Dot3(N,V) > 0)
	{
		for(std::vector<Light>::iterator light = m_lightsInEyeSpace.begin(); light != m_lightsInEyeSpace.end(); light++)
		{
			poisson.createNewSet();
			for(int p = 0; p < m_penumbraCnt; p++){
				poisson.Generate();
				CGrVector penumbraPoint;
				penumbraPoint.X() = poisson.X();
				penumbraPoint.Z() = poisson.Y();
				penumbraPoint.Y() = 0;

				CGrVector L;
				if(light->m_eyePos[3] == 0)
					L = penumbraPoint + light->m_eyePos;
				else
					L = penumbraPoint + light->m_eyePos - intersect;
				L.Normalize();
        
				//Computing shadows

				//Shadow feeler
				CRay  shadowfeeler(intersect, L);

				//Object to ignore
				const CRayIntersection::Object *shadownearest;
		
				//Value for computing t distance to intersection point.
				//Dont really need it (yet), except for giving it a value in the function
				double shadowt;

				//Point of intersection
				//Again, don't really need it (yet)
				CGrVector shadowintersect;

				//Max t, max distance the ray should travel
				double maxT;
				if (light->m_eyePos[3] == 0)
				{
					//Its a point, it shouldn't really have a max. Set it 
					//to something super high
					maxT = 1e20;
				}
				else
				{
					//Its a vector, set the max to the magnitude
					maxT = (penumbraPoint + light->m_eyePos - intersect).Length();
				}



				if(!m_intersectionVec[omp_get_thread_num()]->Intersect(shadowfeeler, maxT, nearest, 
											shadownearest, shadowt, shadowintersect))
				{
					//We've hit nothing, so do color
					color += Mult(light->m_color, diffuse) * DotProduct(N, L) / m_penumbraCnt; // diffuse
					colorChange +=  Mult(light->m_color, diffuse) * DotProduct(N, L) / m_penumbraCnt; // diffuse
					
					CGrVector H = (L + V) / (L + V).Length();
					
					color += Mult(light->m_color, mat->GetSpecular()) * pow((float)DotProduct(N, H), mat->GetShininess()) / m_penumbraCnt;//specular
					colorChange += Mult(light->m_color, mat->GetSpecular()) * pow((float)DotProduct(N, H), mat->GetShininess()) / m_penumbraCnt;
				
				}
		


			}//END PENUMBRA LOOP

		}//END LIGHTS


		//Reflection from other objects
		const float* specularother = mat->GetSpecularOther();
		if (recurse > 1 &&
				(specularother[0] > 0 || specularother[1] > 0 || specularother[2] > 0))
		{
			CPoisson2D reflectionPoisson;
			reflectionPoisson.setMax(2);
			reflectionPoisson.SetMinDisatncePoisson( 1 / (2 * sqrt(double(m_penumbraCnt))));
			int reflectionCount;
			m_softReflections ? reflectionCount = m_penumbraCnt : reflectionCount = 1;
			for(int softReflection = 0; softReflection < reflectionCount; softReflection++)
			{
				CRay reflectionRay;
				if(m_softReflections)
				{
					reflectionPoisson.Generate();
					CGrVector ReflectionPoint(reflectionPoisson.X(), 0, reflectionPoisson.Y());
					reflectionRay = CRay(intersect + ReflectionPoint, N * 2 * Dot3(N, V) - V);
				}
				else
				{
					reflectionRay = CRay(intersect, N * 2 * Dot3(N, V) - V);
				}


				CGrVector reflectionColor;

				if(colorChange[0] > color[0] * m_AdaptiveRecursionThreshold ||
					colorChange[1] > color[1] * m_AdaptiveRecursionThreshold ||
					colorChange[2] > color[2] * m_AdaptiveRecursionThreshold)
				{
					reflectionColor = RayColor(reflectionRay, recurse - 1, nearest);
				}

				for(int c=0; c<3; c++)
				{
					color[c] += specularother[c] * reflectionColor[c];
				}
			}
		}

	}//ENDIF
	



	if(recurse > 1 && CGrVector(mat->GetTransparency()).Length() != 0)
	{
		double etaInside = mat->getEta();
		double etaR = 1. / etaInside;
		double theDot = Dot3(N,V);
		//are we hitting the front or the back
		if(theDot <= 0)
		{
			// We are hitting the back
			N= -N;
			etaR = 1. / etaR;
			theDot = Dot3(N,V);
		}

		double rootTerm = 1 - etaR*etaR * (1 - (theDot * theDot));

		if(rootTerm >= 0)
		{

			CGrVector T = N * (etaR * theDot - sqrt(rootTerm)) - V * etaR;
			CGrVector tcolor(0,0,0);
			CRay rayT(intersect, T);


			tcolor = Mult(mat->GetTransparency(), RayColor(rayT, recurse - 1, nearest));

			color += tcolor; /*not really right yet*/

		}
	}


	float fogcolor[3] = {.8f,.8f,.8f};
	double fogdensity = pow(.5,1/t);
	float fog = pow((float)fogdensity, float(t));
	//color[0] = fog * fogcolor[0] + (1-fog) * color[0];
	//color[1] = fog * fogcolor[1] + (1-fog) * color[1];
	//color[2] = fog * fogcolor[2] + (1-fog) * color[2];

    return color;

}

CGrVector CMyRaytraceRenderer::GetTextureColor(CGrTexture *p_texture, const CGrVector &p_coord)
{
	double tx = fmod(p_coord.X(), 1);
    double ty = fmod(p_coord.Y(), 1);

	double x = tx * p_texture->Width();
	double y = ty * p_texture->Height();

	int ix = int(x);      // Integer part of x
    int ix1 = ix + 1;     // X + 1
    if(ix1 >= p_texture->Width())   // Ensure ix1 is valid
        ix1 = 0;

	int iy = int(y);
	int iy1 = iy + 1;
	if (iy1 >= p_texture->Height())
		iy1 = 0;

	double fx=fmod(x,1);
	double fy=fmod(y,1);

	double bt=(*p_texture)[iy1][ix * 3] * (1.-fx) + (*p_texture)[iy1][ix1 * 3]*fx;
	double bb=(*p_texture)[iy][ix * 3] * (1.-fx) + (*p_texture)[iy][ix1 * 3]*fx;
	//color[2] = (*p_texture)[int(y)][int(x) * 3] / 255.;
	bt /= 255.0;
	bb /= 255.0;

	double gt=(*p_texture)[iy1][ix * 3 + 1] * (1.-fx) + (*p_texture)[iy1][ix1 * 3 + 1]*fx;
	double gb=(*p_texture)[iy][ix * 3 + 1] * (1.-fx) + (*p_texture)[iy][ix1 * 3 + 1]*fx;
	gt /= 255.0;
	gb /= 255.0;

	double rt=(*p_texture)[iy1][ix * 3 + 2] * (1.-fx) + (*p_texture)[iy1][ix1 * 3 + 2]*fx;
	double rb=(*p_texture)[iy][ix * 3 + 2] * (1.-fx) + (*p_texture)[iy][ix1 * 3 + 2]*fx;
	rt /= 255.0;
	rb /= 255.0;

	CGrVector color = CGrVector(rt * fy + rb * ( 1. - fy),
		gt * fy + gb * ( 1. - fy),
		bt * fy + bb * ( 1. - fy));

	return color;
}