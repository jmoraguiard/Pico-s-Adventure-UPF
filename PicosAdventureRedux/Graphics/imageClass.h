#ifndef _IMAGE_CLASS_H_
#define _IMAGE_CLASS_H_

#include <windows.h>

#pragma warning( disable : 4005 )

#include <d3d11.h>
#include <xnamath.h>

#include "textureclass.h"
#include "../Graphics/shader2DClass.h"

class ImageClass
{
private:
	struct VertexType
	{
		XMFLOAT3 position;
	    XMFLOAT2 texture;
	};

public:
	ImageClass();
	ImageClass(const ImageClass&);
	~ImageClass();

	bool setup(ID3D11Device* device, Shader2DClass* shader, int screenWidth, int screenHeight, const std::string& textureFilename, int bitmapWidth, int bitmapHeight);
	bool update(ID3D11DeviceContext*, int, int);
	bool draw(ID3D11DeviceContext* deviceContext, int positionX, int positionY, XMFLOAT4X4 worldMatrix, XMFLOAT4X4 baseViewMatrix,  XMFLOAT4X4 orthoMatrix, XMFLOAT4 color);
	void destroy();

	int getIndexCount();
	ID3D11ShaderResourceView* getTexture();

private:
	bool setupBuffers(ID3D11Device*);
	bool updateBuffers(ID3D11DeviceContext*, int, int);
	void drawBuffers(ID3D11DeviceContext*);
	void destroyBuffers();

	bool loadTexture(ID3D11Device*, const std::string&);
	void releaseTexture();

private:
	ID3D11Buffer *vertexBuffer_, *indexBuffer_;
	int vertexCount_, indexCount_;
	TextureClass* texture_;
	int screenWidth_, screenHeight_;
	int bitmapWidth_, bitmapHeight_;
	int previousPosX_, previousPosY_;

	Shader2DClass* imageShader_;
};

#endif