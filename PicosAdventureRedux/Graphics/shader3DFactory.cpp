#include "shader3DFactory.h"

Shader3DFactory Shader3DFactory::shader3DFactory_;

Shader3DFactory::Shader3DFactory()
{
	Register("ColorShader3D", &ColorShader3DClass::Create);
	Register("DiffuseShader3D", &DiffuseShader3DClass::Create);
	Register("MultiTextureShader3D", &MultiTextureShader3DClass::Create);
	Register("PointlightDiffuseShader3D", &PointlightDiffuseShader3DClass::Create);

	createdShaders_ = 0;
}

Shader3DFactory::Shader3DFactory(const Shader3DFactory &)
{

}

Shader3DFactory::~Shader3DFactory()
{

}

Shader3DFactory* Shader3DFactory::Instance()
{
	return (&shader3DFactory_);
}

void Shader3DFactory::Register(const std::string &object3DType, CreateShader3DFn pfnCreate)
{
	factoryMap_[object3DType] = pfnCreate;
}

Shader3DClass* Shader3DFactory::CreateShader3D(const std::string &object3DType, GraphicsManager* graphicsManager)
{
	std::map<std::string, CreateShader3DFn>::iterator it = factoryMap_.find(object3DType);
	if( it != factoryMap_.end() )
	{
		createdShaders_++;

		return it->second(graphicsManager);
	}
	return NULL;
}