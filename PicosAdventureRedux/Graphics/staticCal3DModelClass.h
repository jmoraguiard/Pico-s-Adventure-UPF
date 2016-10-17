#ifndef _STATIC_CAL3D_MODEL_CLASS_H_
#define _STATIC_CAL3D_MODEL_CLASS_H_

#pragma warning( disable : 4005 )

#include <d3d11.h>
#include <d3dx10math.h>

#include <Windows.h>

#include "Cal3DModelClass.h"

#include <iostream>
#include <fstream>
#include <sstream>

class StaticCal3DModelClass : public Cal3DModelClass
{
	public:
		StaticCal3DModelClass();
		StaticCal3DModelClass(const StaticCal3DModelClass&);
		virtual ~StaticCal3DModelClass();

		virtual bool setup(ID3D11Device*, std::string);
		virtual void draw(ID3D11Device*, ID3D11DeviceContext*);

		virtual bool parseModelConfiguration(std::string strFilename);

	private:
		virtual bool setupBuffers(ID3D11Device*);
};

#endif //_STATIC_CAL3D_MODEL_CLASS_H_
