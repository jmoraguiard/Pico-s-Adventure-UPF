#include "vectorClass.h"

#include "pointClass.h"

Vector::Vector()
{

}

Vector::Vector(float X, float Y, float Z)
{
	x = X;
	y = Y;
	z = Z;
}

float dotProduct(const Vector& a, const Vector& b)
{
	return a.x*b.x + a.y*b.y + a.z*b.z;
}

float Vector::length() const
{
	float length;

	length = sqrt(x*x + y*y + z*z);

	return length;
}

float Vector::lengthSqrt() const
{
	float length;

	length = x*x + y*y + z*z;

	return length;
}

Vector Vector::operator+(const Vector& v) const
{
	Vector r;

	r.x = x + v.x;
	r.y = y + v.y;
	r.z = z + v.z;

	return r;
}

Vector Vector::operator-(const Vector& v) const
{
	return Vector(x - v.x, y - v.y, z - v.z);
}

Vector Vector::operator*(float s) const
{
	Vector scaled;

	scaled.x = x*s;
	scaled.y = y*s;
	scaled.z = z*s;

	return scaled;
}

Vector Vector::operator/(float s) const
{
	Vector scaled;

	scaled.x = x/s;
	scaled.y = y/s;
	scaled.z = z/s;

	return scaled;
}

Vector Vector::normalize() const
{
	Vector normalized;

	normalized = (*this) / length();

	return normalized;
}

Vector operator-(Point a, Point b)
{
	Vector v;

	v.x = a.x - b.x;
	v.y = a.y - b.y;
	v.z = a.z - b.z;

	return v;
}
