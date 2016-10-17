#ifndef _MULTITEXTURE_SHADER_3D_CLASS_H_
#define _MULTITEXTURE_SHADER_3D_CLASS_H_

#include "shader3DClass.h"

class GraphicsManager;

class MultiTextureShader3DClass : public Shader3DClass
{
	private:
		struct MixingBufferType
			{
				float percentage;
				XMFLOAT3 padding;  // Added extra padding so structure is a multiple of 16 for CreateBuffer function requirements.
			};

	public:
		MultiTextureShader3DClass();
		MultiTextureShader3DClass(const MultiTextureShader3DClass&);
		~MultiTextureShader3DClass();

		virtual bool setup(ID3D11Device* device);

		float getPercentage();
		void setPercentage(float percentage);

		static Shader3DClass* __stdcall Create(GraphicsManager* graphicsManager);
	private:
		virtual bool setupShader(ID3D11Device* device, WCHAR* vsFilename, WCHAR* psFilename);
		virtual bool setShaderParameters(ID3D11DeviceContext* deviceContext, XMFLOAT4X4 worldMatrix, XMFLOAT4X4 viewMatrix, 
										 XMFLOAT4X4 projectionMatrix, ID3D11ShaderResourceView** textureArray, LightClass* light);

		ID3D11Buffer* mixingBuffer_;

		float mixingPercentage_;

};

#endif //_MULTITEXTURE_SHADER_3D_CLASS_H_