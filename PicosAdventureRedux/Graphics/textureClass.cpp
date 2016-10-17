////////////////////////////////////////////////////////////////////////////////
// Filename: textureclass.cpp
////////////////////////////////////////////////////////////////////////////////
#include "textureClass.h"

TextureClass::TextureClass()
{
	texture_ = 0;
}

TextureClass::TextureClass(const TextureClass& other)
{
}

TextureClass::~TextureClass()
{
}

bool TextureClass::setup(ID3D11Device* device, const std::string& strFilename)
{
	HRESULT result;

	// Load the texture in.
	result = D3DX11CreateShaderResourceViewFromFileA(device, strFilename.c_str(), NULL, NULL, &texture_, NULL);
	if(FAILED(result))
	{
		//MessageBoxA(NULL, "Could not create the texture shader resource from path.", "Texture - Error", MB_ICONERROR | MB_OK);
		return false;
	}

	unsigned int firstFound = strFilename.find("d-");
	unsigned int secondFound = strFilename.find(".dds");

	if(firstFound != std::string::npos && secondFound != std::string::npos)
	{
		textureName_ = strFilename.substr(firstFound, secondFound-firstFound);
	}

	return true;
}

void TextureClass::destroy()
{
	// Release the texture resource.
	if(texture_)
	{
		texture_->Release();
		texture_ = 0;
	}

	return;
}

ID3D11ShaderResourceView* TextureClass::getTexture()
{
	return texture_;
}

std::string TextureClass::getTextureName()
{
	return textureName_;
}