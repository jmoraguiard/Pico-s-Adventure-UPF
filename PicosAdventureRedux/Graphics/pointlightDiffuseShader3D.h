#ifndef _POINTLIGHT_DIFFUSE_SHADER_3D_CLASS_H_
#define _POINTLIGHT_DIFFUSE_SHADER_3D_CLASS_H_

#include "shader3DClass.h"

class GraphicsManager;

#define NUM_LIGHTS 5

class PointlightDiffuseShader3DClass : public Shader3DClass
{
	private:
		struct LightColorBufferType
			{
				XMFLOAT4 diffuseColor[NUM_LIGHTS];
			};

		struct LightPositionBufferType
			{
				XMFLOAT4 lightPosition[NUM_LIGHTS];
			};

	public:
		PointlightDiffuseShader3DClass();
		PointlightDiffuseShader3DClass(const PointlightDiffuseShader3DClass&);
		~PointlightDiffuseShader3DClass();

		virtual bool setup(ID3D11Device* device);

		void setPositions(XMFLOAT4 pos1, XMFLOAT4 pos2, XMFLOAT4 pos3, XMFLOAT4 pos4, XMFLOAT4 pos5);

		static Shader3DClass* __stdcall Create(GraphicsManager* graphicsManager);

	private:
		virtual bool setupShader(ID3D11Device* device, WCHAR* vsFilename, WCHAR* psFilename);
		virtual bool setShaderParameters(ID3D11DeviceContext* deviceContext, XMFLOAT4X4 worldMatrix, XMFLOAT4X4 viewMatrix, 
										 XMFLOAT4X4 projectionMatrix, ID3D11ShaderResourceView** textureArray, LightClass* light);

		ID3D11Buffer* lightColorBuffer_;
		ID3D11Buffer* lightPositionBuffer_;

		XMFLOAT4	  lightPos1_;
		XMFLOAT4	  lightPos2_;
		XMFLOAT4	  lightPos3_;
		XMFLOAT4	  lightPos4_;
		XMFLOAT4	  lightPos5_;

};

#endif //_POINTLIGHT_DIFFUSE_SHADER_3D_CLASS_H_