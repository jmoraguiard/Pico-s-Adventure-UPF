#ifndef _SHADER_3D_FACTORY_H_
#define _SHADER_3D_FACTORY_H_

#include "shader3DClass.h"

#include "colorShader3D.h"
#include "diffuseShader3D.h"
#include "multitextureShader3D.h"
#include "pointlightDiffuseShader3D.h"

#include <map>

class Shader3DFactory
{
	public:
		Shader3DFactory();
		Shader3DFactory(const Shader3DFactory &);
		~Shader3DFactory();

		static Shader3DFactory* Instance();

		void Register(const std::string &shader3DType, CreateShader3DFn pfnCreate);
		Shader3DClass* CreateShader3D(const std::string &shader3DType, GraphicsManager* graphicsManager);

	private:
		// Singleton
		static Shader3DFactory shader3DFactory_;
		
		// Map where we are going to save the Create functions
		std::map<std::string, CreateShader3DFn>	factoryMap_;

		int createdShaders_;

};

#endif //_SHADER_3D_FACTORY_H_