#ifndef _MODEL_CLASS_H_
#define _MODEL_CLASS_H_

#pragma warning( disable : 4005 )

#include <d3d11.h>

#include <Windows.h>

#include "graphicsManager.h"

#include <iostream>
#include <fstream>
#include <sstream>

class ModelClass
{
	public:
		ModelClass();
		ModelClass(const ModelClass&);
		virtual ~ModelClass();

		virtual bool setup(ID3D11Device*, std::string) = 0;
		virtual void draw(ID3D11Device*, ID3D11DeviceContext*) = 0;
		virtual void destroy() = 0;

		int getIndexCount();
		std::string getModelName();

	protected:
		virtual bool setupBuffers(ID3D11Device*) = 0;
		virtual void drawBuffers(ID3D11DeviceContext*);
		virtual void destroyBuffers();

		virtual bool parseModelConfiguration(std::string strFilename) = 0;

		ID3D11Buffer *vertexBuffer_, *indexBuffer_;
		int vertexCount_, indexCount_;

		std::string modelName_;
};

#endif
