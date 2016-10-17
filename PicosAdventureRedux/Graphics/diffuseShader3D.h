#ifndef _DIFFUSE_SHADER_3D_CLASS_H_
#define _DIFFUSE_SHADER_3D_CLASS_H_

#include "shader3DClass.h"

class GraphicsManager;

class DiffuseShader3DClass : public Shader3DClass
{

	public:
		DiffuseShader3DClass();
		DiffuseShader3DClass(const DiffuseShader3DClass&);
		~DiffuseShader3DClass();

		virtual bool setup(ID3D11Device* device);

		static Shader3DClass* __stdcall Create(GraphicsManager* graphicsManager);
	private:
		virtual bool setupShader(ID3D11Device* device, WCHAR* vsFilename, WCHAR* psFilename);
		virtual bool setShaderParameters(ID3D11DeviceContext* deviceContext, XMFLOAT4X4 worldMatrix, XMFLOAT4X4 viewMatrix, 
										 XMFLOAT4X4 projectionMatrix, ID3D11ShaderResourceView** textureArray, LightClass* light);

};

#endif //_DIFFUSE_SHADER_3D_CLASS_H_