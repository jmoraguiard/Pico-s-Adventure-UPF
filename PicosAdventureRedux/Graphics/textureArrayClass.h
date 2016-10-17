#ifndef _TEXTURE_ARRAY_CLASS_H_
#define _TEXTURE_ARRAY_CLASS_H_

#pragma warning( disable : 4005 )

#include <d3d11.h>
#include <d3dx11tex.h>

#include <Windows.h>

#include <iostream>
#include <fstream>
#include <sstream>

class TextureArrayClass
{
	public:
		TextureArrayClass();
		TextureArrayClass(const TextureArrayClass&);
		~TextureArrayClass();

		bool setup(ID3D11Device*, const std::string& strFilename1, const std::string& strFilename2, const std::string& strFilename3, int num);
		void destroy();

		ID3D11ShaderResourceView** getTexturesArray();
		std::string* getNamesArray();
		
		int getNumberTextures();
		void setNumberTextures(int num);

	private:
		ID3D11ShaderResourceView* textures_[3];
		std::string				  name_[3];
		int						  numberTextures_;
};

#endif //_TEXTURE_ARRAY_CLASS_H_