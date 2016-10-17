////////////////////////////////////////////////////////////////////////////////
// Filename: textureclass.cpp
////////////////////////////////////////////////////////////////////////////////
#include "textureArrayClass.h"


TextureArrayClass::TextureArrayClass()
{
	textures_[0] = 0;
	textures_[1] = 0;
	textures_[3] = 0;

	numberTextures_ = 0;
}


TextureArrayClass::TextureArrayClass(const TextureArrayClass& other)
{
}


TextureArrayClass::~TextureArrayClass()
{
}


bool TextureArrayClass::setup(ID3D11Device* device, const std::string& strFilename1, const std::string& strFilename2, const std::string& strFilename3, int num)
{
	HRESULT result;

	// Load the texture in.
	result = D3DX11CreateShaderResourceViewFromFileA(device, strFilename1.c_str(), NULL, NULL, &textures_[0], NULL);
	if(FAILED(result))
	{
		MessageBoxA(NULL, "Could not create the texture shader resource 1 from path.", "Texture - Error", MB_ICONERROR | MB_OK);
		return false;
	}

	if(strFilename2 == "")
	{
		result = D3DX11CreateShaderResourceViewFromFileA(device, strFilename1.c_str(), NULL, NULL, &textures_[1], NULL);
	}
	else
	{
		result = D3DX11CreateShaderResourceViewFromFileA(device, strFilename2.c_str(), NULL, NULL, &textures_[1], NULL);
	}
	if(FAILED(result))
	{
		MessageBoxA(NULL, "Could not create the texture shader resource 2 from path.", "Texture - Error", MB_ICONERROR | MB_OK);
		return false;
	}

	if(strFilename3 == "")
	{
		result = D3DX11CreateShaderResourceViewFromFileA(device, strFilename1.c_str(), NULL, NULL, &textures_[2], NULL);
	}
	else
	{
		result = D3DX11CreateShaderResourceViewFromFileA(device, strFilename3.c_str(), NULL, NULL, &textures_[2], NULL);
	}
	if(FAILED(result))
	{
		MessageBoxA(NULL, "Could not create the texture shader resource 3 from path.", "Texture - Error", MB_ICONERROR | MB_OK);
		return false;
	}


	numberTextures_ = num;

	return true;
}


void TextureArrayClass::destroy()
{
	// Release the texture resource.
	if(textures_[0])
	{
		textures_[0]->Release();
		textures_[0] = 0;
	}

	if(numberTextures_> 1)
	{
		if(textures_[1])
		{
			textures_[1]->Release();
			textures_[1] = 0;
		}
		
		if(numberTextures_> 2)
		{
			if(textures_[2])
			{
				textures_[2]->Release();
				textures_[2] = 0;
			}
		}
	}

	return;
}

ID3D11ShaderResourceView** TextureArrayClass::getTexturesArray()
{
	return textures_;
}

std::string* TextureArrayClass::getNamesArray()
{
	return name_;
}

int TextureArrayClass::getNumberTextures()
{
	return numberTextures_;
}

void TextureArrayClass::setNumberTextures(int num)
{
	numberTextures_ = num;
}