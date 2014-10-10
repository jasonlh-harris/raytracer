#include <vector>
#pragma once

class CPoisson2D
{
private:
	double minDistance;
	double minDistanceSquared;
	struct Point
	{
		double x;
		double y;
	};
	Point curPoint;
	std::vector<Point> history;
	double m_max;
public:
	CPoisson2D(void);
	~CPoisson2D(void);
	void SetMinDistance(double min){minDistance = (1. / (2* sqrt(min))); minDistanceSquared = minDistance * minDistance;}
	void SetMinDisatncePoisson(double min){minDistance = min; minDistanceSquared = minDistance * minDistance;}
	void createNewSet(void){history.clear();}
	double X(void);
	double Y(void);
	void setMax(double m){m_max = m;}
	void Generate(void);
	double CPoisson2D::Distance(Point a, Point b);
	void GenerateCircle(void);
	void GenerateHeart(void);

};

