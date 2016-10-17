#include "pointClass.h"

#include "vectorClass.h"

Point::Point()
{

}

Point::Point(float X, float Y, float Z)
{
	x = X;
	y = Y;
	z = Z;
}

Point::Point(float X, float Y)
{
	x = X;
	y = Y;
	z = 0;
}

Point Point::addVector(Vector v)
{
	Point p2;

	p2.x = x + v.x;
	p2.z = y + v.y;
	p2.z = z + v.z;

	return p2;
}

double Point::dist(Point a)
{
	double xd = x - a.x;
	double yd = y - a.y;
	double yz = z - a.z;
	return sqrt(xd*xd + yd*yd + yz*yz);
}