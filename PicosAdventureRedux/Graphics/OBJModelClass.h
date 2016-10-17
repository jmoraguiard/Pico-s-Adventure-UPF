#ifndef _OBJ_MODEL_CLASS_H_
#define _OBJ_MODEL_CLASS_H_

#pragma warning( disable : 4005 )

#include <d3d11.h>

#include <Windows.h>

#include "ModelClass.h"

#include <iostream>
#include <fstream>

class OBJModelClass : public ModelClass
{
	typedef struct
		{
			float x, y, z;
		}VertexType;

	typedef struct
		{
			int vIndex1, vIndex2, vIndex3;
			int tIndex1, tIndex2, tIndex3;
			int nIndex1, nIndex2, nIndex3;
		}FaceType;

	typedef struct
		{
			VertexType *vertices, *texcoords, *normals;
			FaceType *faces;
			int vertexCount, textureCount, normalCount, faceCount;
		}OBJModel;

	public:
		OBJModelClass();
		OBJModelClass(const OBJModelClass&);
		virtual ~OBJModelClass();

		virtual bool setup(ID3D11Device*, std::string);
		virtual void draw(ID3D11Device*, ID3D11DeviceContext*);
		virtual void destroy();

	protected:
		virtual bool setupBuffers(ID3D11Device*);

		virtual bool parseModelConfiguration(std::string strFilename);

		// OBJModel
		OBJModel objModel_;
};

#endif //_OBJ_MODEL_CLASS_H_
