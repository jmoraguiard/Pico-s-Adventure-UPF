#ifndef _SHADER_2D_CLASS_H_
#define _SHADER_2D_CLASS_H_

#include <windows.h>

#pragma warning( disable : 4005 )

#include <d3d11.h>
#include <xnamath.h>
#include <d3dx11async.h>

#include <fstream>

class Shader2DClass
{
private:
	struct MatrixBufferType
	{
		XMFLOAT4X4 world;
		XMFLOAT4X4 view;
		XMFLOAT4X4 projection;
	};

	struct PixelBufferType
	{
		XMFLOAT4 pixelColor;
	};

public:
	Shader2DClass();
	Shader2DClass(const Shader2DClass&);
	~Shader2DClass();

	bool setup(ID3D11Device*);
	bool draw(ID3D11DeviceContext*, int, XMFLOAT4X4, XMFLOAT4X4, XMFLOAT4X4, ID3D11ShaderResourceView*, XMFLOAT4);
	void destroy();

private:
	bool setupShader(ID3D11Device*, WCHAR*, WCHAR*);
	void drawShader(ID3D11DeviceContext*, int);
	void destroyShader();
	void outputShaderErrorMessage(ID3D10Blob*, WCHAR*);

	bool setShaderParameters(ID3D11DeviceContext*, XMFLOAT4X4, XMFLOAT4X4, XMFLOAT4X4, ID3D11ShaderResourceView*, XMFLOAT4);

private:
	ID3D11VertexShader* vertexShader_;
	ID3D11PixelShader* pixelShader_;
	ID3D11InputLayout* layout_;
	ID3D11Buffer* matrixBuffer_;
	ID3D11SamplerState* sampleState_;
	ID3D11Buffer* pixelBuffer_;
};

#endif