///////////////////////////////////////////////////////////////////////////////
// Filename: textclass.cpp
///////////////////////////////////////////////////////////////////////////////
#include "textclass.h"

TextClass::TextClass()
{
	font_ = 0;
	fontShader_ = 0;

	sentence_ = 0;
}

TextClass::TextClass(const TextClass& other)
{
}

TextClass::~TextClass()
{
}

bool TextClass::setup(ID3D11Device* device, ID3D11DeviceContext* deviceContext, Shader2DClass* shader, int screenWidth, int screenHeight, float posX, float posY, std::string sentenceText)
{
	bool result;

	deviceContext_ = deviceContext;

	// Store the screen width and height.
	screenWidth_ = screenWidth;
	screenHeight_ = screenHeight;

	// Store the position
	positionX_ = posX;
	positionY_ = posY;

	// Create the font object.
	font_ = new FontClass;
	if(!font_)
	{
		return false;
	}

	// setup the font object.
	result = font_->setup(device, "basicFont");
	if(!result)
	{
		MessageBoxA(NULL, "Could not setup the font object.", "Text - Error", MB_ICONERROR | MB_OK);
		return false;
	}

	// Create the font shader object.
	fontShader_ = shader;

	// setup the first sentence.
	result = setupSentence(&sentence_, 500, device);
	if(!result)
	{
		return false;
	}

	// Now update the sentence vertex buffer with the new string information.
	result = updateSentence(sentence_, sentenceText, positionX_, positionY_, 0.0f, 0.0f, 0.0f, deviceContext);
	if(!result)
	{
		return false;
	}

	text_ = sentenceText;

	return true;
}

bool TextClass::draw(ID3D11DeviceContext* deviceContext, XMFLOAT4X4 worldMatrix, XMFLOAT4X4 baseViewMatrix, XMFLOAT4X4 orthoMatrix)
{
	bool result;

	// Draw the first sentence.
	result = drawSentence(deviceContext, sentence_, worldMatrix, baseViewMatrix, orthoMatrix);
	if(!result)
	{
		return false;
	}

	return true;
}

void TextClass::destroy()
{
	// destroy the first sentence.
	destroySentence(&sentence_);

	// destroy the font shader object.
	if(fontShader_)
	{
		fontShader_->destroy();
		delete fontShader_;
		fontShader_ = 0;
	}

	// destroy the font object.
	if(font_)
	{
		font_->destroy();
		delete font_;
		font_ = 0;
	}

	return;
}

bool TextClass::setText(std::string text, ID3D11DeviceContext* deviceContext)
{
	// Update the sentence vertex buffer with the new string information.
	char * str = const_cast<char *>(text.c_str());
	if(!updateSentence(sentence_, str, positionX_, positionY_, 0.0f, 0.0f, 0.0f, deviceContext))
	{
		MessageBoxA(NULL, "Could not update text.", "Text - Error", MB_ICONERROR | MB_OK);
		return false;
	}

	text_ = text;

	return true;
}

Point TextClass::getTextPosition()
{
	return Point(positionX_, positionY_, 0.0f);
}

int TextClass::getTextWidth()
{
	return sentence_->width;
}

int TextClass::getTextHeight()
{
	return 16;
}

bool TextClass::setupSentence(SentenceType** sentence, int maxLength, ID3D11Device* device)
{
	VertexType* vertices;
	unsigned long* indices;
	D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
    D3D11_SUBRESOURCE_DATA vertexData, indexData;
	HRESULT result;
	int i;


	// Create a new sentence object.
	*sentence = new SentenceType;
	if(!*sentence)
	{
		return false;
	}

	// setup the sentence buffers to null.
	(*sentence)->vertexBuffer = 0;
	(*sentence)->indexBuffer = 0;

	// Set the maximum length of the sentence.
	(*sentence)->maxLength = maxLength;

	// Set the number of vertices in the vertex array.
	(*sentence)->vertexCount = 6 * maxLength;

	// Set the number of indexes in the index array.
	(*sentence)->indexCount = (*sentence)->vertexCount;

	// Create the vertex array.
	vertices = new VertexType[(*sentence)->vertexCount];
	if(!vertices)
	{
		return false;
	}

	// Create the index array.
	indices = new unsigned long[(*sentence)->indexCount];
	if(!indices)
	{
		return false;
	}

	// setup vertex array to zeros at first.
	memset(vertices, 0, (sizeof(VertexType) * (*sentence)->vertexCount));

	// setup the index array.
	for(i=0; i<(*sentence)->indexCount; i++)
	{
		indices[i] = i;
	}

	// Set up the description of the dynamic vertex buffer.
    vertexBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
    vertexBufferDesc.ByteWidth = sizeof(VertexType) * (*sentence)->vertexCount;
    vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    vertexBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	// Give the subresource structure a pointer to the vertex data.
    vertexData.pSysMem = vertices;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	// Create the vertex buffer.
    result = device->CreateBuffer(&vertexBufferDesc, &vertexData, &(*sentence)->vertexBuffer);
	if(FAILED(result))
	{
		return false;
	}

	// Set up the description of the static index buffer.
    indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
    indexBufferDesc.ByteWidth = sizeof(unsigned long) * (*sentence)->indexCount;
    indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
    indexBufferDesc.CPUAccessFlags = 0;
    indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;

	// Give the subresource structure a pointer to the index data.
    indexData.pSysMem = indices;
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;

	// Create the index buffer.
	result = device->CreateBuffer(&indexBufferDesc, &indexData, &(*sentence)->indexBuffer);
	if(FAILED(result))
	{
		return false;
	}

	// destroy the vertex array as it is no longer needed.
	delete [] vertices;
	vertices = 0;

	// destroy the index array as it is no longer needed.
	delete [] indices;
	indices = 0;

	return true;
}

bool TextClass::updateSentence(SentenceType* sentence, std::string text, float positionX, float positionY, float red, float green, float blue,
							   ID3D11DeviceContext* deviceContext)
{
	int numLetters;
	VertexType* vertices;
	float drawX, drawY;
	HRESULT result;
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	VertexType* verticesPtr;

	text_ = text;

	// Store the color of the sentence.
	sentence->red = red;		
	sentence->green = green;
	sentence->blue = blue;

	// Get the number of letters in the sentence.
	numLetters = (int)strlen(text.c_str());

	// Check for possible buffer overflow.
	if(numLetters > sentence->maxLength)
	{
		return false;
	}

	// Create the vertex array.
	vertices = new VertexType[sentence->vertexCount];
	if(!vertices)
	{
		return false;
	}

	// setup vertex array to zeros at first.
	memset(vertices, 0, (sizeof(VertexType) * sentence->vertexCount));

	// Calculate the X and Y pixel position on the screen to start drawing to.
	drawX = (float)(((screenWidth_ / 2) * -1) + positionX);
	drawY = (float)((screenHeight_ / 2) - positionY);

	// Use the font class to build the vertex array from the sentence text and sentence draw location.
	sentence->width = font_->buildVertexArray((void*)vertices, text, drawX, drawY);

	// Lock the vertex buffer so it can be written to.
	result = deviceContext->Map(sentence->vertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if(FAILED(result))
	{
		return false;
	}

	// Get a pointer to the data in the vertex buffer.
	verticesPtr = (VertexType*)mappedResource.pData;

	// Copy the data into the vertex buffer.
	memcpy(verticesPtr, (void*)vertices, (sizeof(VertexType) * sentence->vertexCount));

	// Unlock the vertex buffer.
	deviceContext->Unmap(sentence->vertexBuffer, 0);

	// destroy the vertex array as it is no longer needed.
	delete [] vertices;
	vertices = 0;

	return true;
}

void TextClass::setPosition(float positionX, float positionY)
{
	updateSentence(sentence_, text_, positionX, positionY, sentence_->red, sentence_->green, sentence_->blue, deviceContext_);
}

void TextClass::destroySentence(SentenceType** sentence)
{
	if(*sentence)
	{
		// destroy the sentence vertex buffer.
		if((*sentence)->vertexBuffer)
		{
			(*sentence)->vertexBuffer->Release();
			(*sentence)->vertexBuffer = 0;
		}

		// destroy the sentence index buffer.
		if((*sentence)->indexBuffer)
		{
			(*sentence)->indexBuffer->Release();
			(*sentence)->indexBuffer = 0;
		}

		// destroy the sentence.
		delete *sentence;
		*sentence = 0;
	}

	return;
}

bool TextClass::drawSentence(ID3D11DeviceContext* deviceContext, SentenceType* sentence, XMFLOAT4X4 worldMatrix, XMFLOAT4X4 baseViewMatrix, 
							   XMFLOAT4X4 orthoMatrix)
{
	unsigned int stride, offset;
	XMFLOAT4 pixelColor;
	bool result;


	// Set vertex buffer stride and offset.
    stride = sizeof(VertexType); 
	offset = 0;

	// Set the vertex buffer to active in the input assembler so it can be drawed.
	deviceContext->IASetVertexBuffers(0, 1, &sentence->vertexBuffer, &stride, &offset);

    // Set the index buffer to active in the input assembler so it can be drawed.
	deviceContext->IASetIndexBuffer(sentence->indexBuffer, DXGI_FORMAT_R32_UINT, 0);

    // Set the type of primitive that should be drawed from this vertex buffer, in this case triangles.
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// Create a pixel color vector with the input sentence color.
	pixelColor = XMFLOAT4(sentence->red, sentence->green, sentence->blue, 1.0f);

	// draw the text using the font shader.
	result = fontShader_->draw(deviceContext, sentence->indexCount, worldMatrix, baseViewMatrix, orthoMatrix, font_->getTexture(), pixelColor);
	if(!result)
	{
		false;
	}

	return true;
}