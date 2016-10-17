#ifndef _PARTICLES_SHADER_3D_CLASS_H_
#define _PARTICLES_SHADER_3D_CLASS_H_

#include "shader3DClass.h"

class GraphicsManager;

class ColorShader3DClass : public Shader3DClass
{
	private:
		struct ColorLightBufferType
			{
				XMFLOAT4 diffuseColor;
			};

	public:
		ColorShader3DClass();
		ColorShader3DClass(const ColorShader3DClass&);
		~ColorShader3DClass();

		virtual bool setup(ID3D11Device* device);

		static Shader3DClass* __stdcall Create(GraphicsManager* graphicsManager);
	private:
		virtual bool setupShader(ID3D11Device* device, WCHAR* vsFilename, WCHAR* psFilename);
		virtual bool setShaderParameters(ID3D11DeviceContext* deviceContext, XMFLOAT4X4 worldMatrix, XMFLOAT4X4 viewMatrix, 
										 XMFLOAT4X4 projectionMatrix, ID3D11ShaderResourceView** textureArray, LightClass* light);

		ID3D11Buffer* colorLightBuffer_;

};

#endif //_DIFFUSE_SHADER_3D_CLASS_H_