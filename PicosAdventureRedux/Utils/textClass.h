#ifndef _TEXT_CLASS_H_
#define _TEXT_CLASS_H_

#include "../Graphics/fontClass.h"
#include "../Graphics/shader2DClass.h"

#include "../Math/pointClass.h"

class TextClass
{
private:
	struct SentenceType
	{
		ID3D11Buffer *vertexBuffer, *indexBuffer;
		int vertexCount, indexCount, maxLength;
		float red, green, blue;
		int width;
	};

	struct VertexType
	{
		XMFLOAT3 position;
	    XMFLOAT2 texture;
	};

public:
	TextClass();
	TextClass(const TextClass&);
	~TextClass();

	bool setup(ID3D11Device* device, ID3D11DeviceContext* deviceContext, Shader2DClass* shader, int screenWidth, int screenHeight, float posX, float posY, std::string sentenceText);
	bool draw(ID3D11DeviceContext* deviceContext, XMFLOAT4X4 worldMatrix, XMFLOAT4X4 baseViewMatrix, XMFLOAT4X4 orthoMatrix);
	void destroy();

	bool setText(std::string text, ID3D11DeviceContext* deviceContext);

	void setPosition(float positionX, float positionY);

	Point getTextPosition();
	int getTextWidth();
	int getTextHeight();

private:
	bool setupSentence(SentenceType** sentence, int maxLength, ID3D11Device* device);
	bool updateSentence(SentenceType* sentence, std::string text, float positionX, float positionY, float red, float green, float blue, ID3D11DeviceContext* deviceContext);
	bool drawSentence(ID3D11DeviceContext* deviceContext, SentenceType* sentence, XMFLOAT4X4 worldMatrix, XMFLOAT4X4 baseViewMatrix, XMFLOAT4X4 orthoMatrix);
	void destroySentence(SentenceType** sentence);

	ID3D11DeviceContext* deviceContext_;
	FontClass* font_;
	Shader2DClass* fontShader_;
	int screenWidth_, screenHeight_;
	float positionX_, positionY_;
	SentenceType* sentence_;
	std::string text_;
};

#endif