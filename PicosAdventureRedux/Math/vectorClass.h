#ifndef _VECTOR_CLASS_H_
#define _VECTOR_CLASS_H_

#include <math.h>

class Point;

class Vector
{
	public:
		Vector();
		Vector(float X, float Y, float Z);

		float length() const;
		float lengthSqrt() const;

		Vector operator+(const Vector& v) const;
		Vector operator-(const Vector& v) const;

		Vector operator*(float s) const;
		Vector operator/(float s) const;

		Vector normalize() const;

		float x, y, z;
};

#endif //_VECTOR_CLASS_H_