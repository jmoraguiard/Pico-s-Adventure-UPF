#ifndef _POINT_CLASS_H_
#define _POINT_CLASS_H_

#include <math.h>

class Vector;

class Point
{
	public:
		Point();
		Point(float X, float Y);
		Point(float X, float Y, float Z);

		Point addVector(Vector v);
		double dist(Point a);

		float x, y, z;
};

#endif //_POINT_CLASS_H_