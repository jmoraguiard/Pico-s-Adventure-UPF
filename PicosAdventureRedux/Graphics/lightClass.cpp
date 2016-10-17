#include "lightClass.h"

LightClass::LightClass()
{
}


LightClass::LightClass(const LightClass& other)
{
}


LightClass::~LightClass()
{
}

void LightClass::setAmbientColor(float red, float green, float blue, float alpha)
{
	ambientColor_ = XMFLOAT4(red, green, blue, alpha);
	return;
}

void LightClass::setDiffuseColor(float red, float green, float blue, float alpha)
{
	diffuseColor_ = XMFLOAT4(red, green, blue, alpha);
	return;
}


void LightClass::setDirection(float x, float y, float z)
{
	direction_ = XMFLOAT3(x, y, z);
	return;
}

XMFLOAT4 LightClass::getAmbientColor()
{
	return ambientColor_;
}

XMFLOAT4 LightClass::getDiffuseColor()
{
	return diffuseColor_;
}


XMFLOAT3  LightClass::getDirection()
{
	return direction_;
}
