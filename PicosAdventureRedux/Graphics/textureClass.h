#ifndef _TEXTURE_CLASS_H_
#define _TEXTURE_CLASS_H_

#pragma warning( disable : 4005 )

#include <d3d11.h>
#include <d3dx11tex.h>

#include <Windows.h>

#include <iostream>
#include <fstream>
#include <sstream>

class TextureClass
{
public:
	TextureClass();
	TextureClass(const TextureClass&);
	~TextureClass();

	bool setup(ID3D11Device*, const std::string& strFilename);
	void destroy();

	ID3D11ShaderResourceView* getTexture();
	std::string getTextureName();

private:
	ID3D11ShaderResourceView* texture_;
	std::string textureName_;
};

#endif