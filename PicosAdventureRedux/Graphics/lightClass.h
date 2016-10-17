#ifndef _LIGHT_CLASS_H_
#define _LIGHT_CLASS_H_

#include <windows.h>

#include <xnamath.h>

class LightClass
{
public:
	LightClass();
	LightClass(const LightClass&);
	~LightClass();

	void setAmbientColor(float red, float green, float blue, float alpha);
	void setDiffuseColor(float red, float green, float blue, float alpha);
	void setDirection(float x, float y, float z);

	XMFLOAT4 getAmbientColor();
	XMFLOAT4 getDiffuseColor();
	XMFLOAT3 getDirection();

private:
	XMFLOAT4 ambientColor_;
	XMFLOAT4 diffuseColor_;
	XMFLOAT3 direction_;
};

#endif
