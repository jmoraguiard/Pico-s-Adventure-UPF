#ifndef _FONT_CLASS_H_
#define _FONT_CLASS_H_

#pragma warning( disable : 4005 )

#include <d3d11.h>
#include <xnamath.h>

#include <fstream>

#include "textureClass.h"

class FontClass
{
private:
	struct FontType
	{
		float left, right;
		int size;
	};

	struct VertexType
	{
		XMFLOAT3 position;
	    XMFLOAT2 texture;
	};

public:
	FontClass();
	FontClass(const FontClass&);
	~FontClass();

	bool setup(ID3D11Device*, const std::string& fontFilename);
	void destroy();

	ID3D11ShaderResourceView* getTexture();

	int buildVertexArray(void*, std::string, float, float);

private:
	bool loadFontData(const std::string&);
	void destroyFontData();
	bool loadTexture(ID3D11Device*, const std::string&);
	void destroyTexture();

private:
	FontType* font_;
	TextureClass* texture_;
};

#endif