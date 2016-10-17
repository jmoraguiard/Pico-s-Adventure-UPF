#include "fontClass.h"

FontClass::FontClass()
{
	font_ = 0;
	texture_ = 0;
}


FontClass::FontClass(const FontClass& other)
{
}


FontClass::~FontClass()
{
}


bool FontClass::setup(ID3D11Device* device, const std::string& fontFilename)
{
	bool result;

	std::string fontDataFilename = "./Data/fonts/" + fontFilename + "/fontdata.txt";
	std::string fontTextureFilename = "./Data/fonts/" + fontFilename + "/font.dds";

	// Load in the text file containing the font data.
	result = loadFontData(fontDataFilename);
	if(!result)
	{
		MessageBoxA(NULL, "Could not load font data.", "Font - Error", MB_ICONERROR | MB_OK);
		return false;
	}

	// Load the texture that has the font characters on it.
	result = loadTexture(device, fontTextureFilename);
	if(!result)
	{
		MessageBoxA(NULL, "Could not load the texture for the font.", "Font - Error", MB_ICONERROR | MB_OK);
		return false;
	}

	return true;
}


void FontClass::destroy()
{
	// Release the font texture.
	destroyTexture();

	// Release the font data.
	destroyFontData();

	return;
}


bool FontClass::loadFontData(const std::string& fontDataFilename)
{
	std::ifstream fin;
	int i;
	char temp;


	// Create the font spacing buffer.
	font_ = new FontType[95];
	if(!font_)
	{
		return false;
	}

	// Read in the font size and spacing between chars.
	fin.open(fontDataFilename);
	if(fin.fail())
	{
		return false;
	}

	// Read in the 95 used ascii characters for text.
	for(i=0; i<95; i++)
	{
		fin.get(temp);
		while(temp != ' ')
		{
			fin.get(temp);
		}
		fin.get(temp);
		while(temp != ' ')
		{
			fin.get(temp);
		}

		fin >> font_[i].left;
		fin >> font_[i].right;
		fin >> font_[i].size;
	}

	// Close the file.
	fin.close();

	return true;
}


void FontClass::destroyFontData()
{
	// Release the font data array.
	if(font_)
	{
		delete [] font_;
		font_ = 0;
	}

	return;
}


bool FontClass::loadTexture(ID3D11Device* device, const std::string& filename)
{
	bool result;


	// Create the texture object.
	texture_ = new TextureClass;
	if(!texture_)
	{
		return false;
	}

	// Initialize the texture object.
	result = texture_->setup(device, filename);
	if(!result)
	{
		return false;
	}

	return true;
}


void FontClass::destroyTexture()
{
	// Release the texture object.
	if(texture_)
	{
		texture_->destroy();
		delete texture_;
		texture_ = 0;
	}

	return;
}


ID3D11ShaderResourceView* FontClass::getTexture()
{
	return texture_->getTexture();
}


int FontClass::buildVertexArray(void* vertices, std::string sentence, float drawX, float drawY)
{
	VertexType* vertexPtr;
	int numLetters, index, i, letter;
	int totalSize = 0;


	// Coerce the input vertices into a VertexType structure.
	vertexPtr = (VertexType*)vertices;

	// Get the number of letters in the sentence.
	numLetters = (int)strlen(sentence.c_str());

	// Initialize the index to the vertex array.
	index = 0;

	// Draw each letter onto a quad.
	for(i=0; i<numLetters; i++)
	{
		letter = ((int)sentence[i]) - 32;

		// If the letter is a space then just move over three pixels.
		if(letter == 0)
		{
			drawX = drawX + 3.0f;
		}
		else
		{
			// First triangle in quad.
			vertexPtr[index].position = XMFLOAT3(drawX, drawY, 0.0f);  // Top left.
			vertexPtr[index].texture = XMFLOAT2(font_[letter].left, 0.0f);
			index++;

			vertexPtr[index].position = XMFLOAT3((drawX + font_[letter].size), (drawY - 16), 0.0f);  // Bottom right.
			vertexPtr[index].texture = XMFLOAT2(font_[letter].right, 1.0f);
			index++;

			vertexPtr[index].position = XMFLOAT3(drawX, (drawY - 16), 0.0f);  // Bottom left.
			vertexPtr[index].texture = XMFLOAT2(font_[letter].left, 1.0f);
			index++;

			// Second triangle in quad.
			vertexPtr[index].position = XMFLOAT3(drawX, drawY, 0.0f);  // Top left.
			vertexPtr[index].texture = XMFLOAT2(font_[letter].left, 0.0f);
			index++;

			vertexPtr[index].position = XMFLOAT3(drawX + font_[letter].size, drawY, 0.0f);  // Top right.
			vertexPtr[index].texture = XMFLOAT2(font_[letter].right, 0.0f);
			index++;

			vertexPtr[index].position = XMFLOAT3((drawX + font_[letter].size), (drawY - 16), 0.0f);  // Bottom right.
			vertexPtr[index].texture = XMFLOAT2(font_[letter].right, 1.0f);
			index++;

			// Update the x location for drawing by the size of the letter and one pixel.
			drawX = drawX + font_[letter].size + 1.0f;
			totalSize += font_[letter].size+2;
		}
	}

	return totalSize;
}