#include "StdAfx.h"
#include "Poisson2D.h"
#include <time.h>

CPoisson2D::CPoisson2D(void)
{
	srand (time(NULL));
	m_max = 1;
}


CPoisson2D::~CPoisson2D(void)
{
}
double CPoisson2D::X(void)
{
	return curPoint.x;
}
double CPoisson2D::Y(void)
{
	return curPoint.y;
}


void CPoisson2D::GenerateHeart(void)
{
	bool valid;
	Point p;
	do
	{
		valid = true;
		p.x = -1 + 2 * double(rand()) / double(RAND_MAX);
		p.y = -1 + 2 * double(rand()) / double(RAND_MAX);
		if ( p.x*p.x + ( (5*p.y / 4 - sqrt(abs(p.x))) * (5*p.y / 4 - sqrt(abs(p.x)))) <= 1 )
		{
			for(std::vector<Point>::iterator it = history.begin(); it != history.end(); it++)
			{
				Point point;
				point = *it;
				if( Distance(point, p) < minDistanceSquared)
				{
					valid = false;
					break;
				}
			}
		}
		else
		{
			valid = false;
		}
	}while (!valid);
	history.push_back(p);
	curPoint = p;
}

void CPoisson2D::GenerateCircle(void)
{
	bool valid;
	Point p;
	do
	{
		valid = true;
		p.x = -1 + 2 * double(rand()) / double(RAND_MAX);
		p.y = -1 + 2 * double(rand()) / double(RAND_MAX);
		if ( p.y*p.y + p.x*p.x < 1){
			for(std::vector<Point>::iterator it = history.begin(); it != history.end(); it++)
			{
				Point point;
				point = *it;
				if( Distance(point, p) < minDistanceSquared)
				{
					valid = false;
					break;
				}
			}
		}
		else
		{
			valid = false;
		}
	}while (!valid);
	history.push_back(p);
	curPoint = p;
}

void CPoisson2D::Generate(void)
{
	bool valid;
	Point p;
	do
	{
		valid = true;
		p.x = m_max * double(rand()) / double(RAND_MAX);
		p.y = m_max * double(rand()) / double(RAND_MAX);
		for(std::vector<Point>::iterator it = history.begin(); it != history.end(); it++)
		{
			Point point;
			point = *it;
			if( Distance(point, p) < minDistanceSquared)
			{
				valid = false;
				break;
			}
		}
	}while (!valid);
	history.push_back(p);
	curPoint = p;
}
double CPoisson2D::Distance(Point a, Point b)
{
 return (a.x - b.x) * (a.x - b.x) + (a.y - b.y) * (a.y - b.y);
}