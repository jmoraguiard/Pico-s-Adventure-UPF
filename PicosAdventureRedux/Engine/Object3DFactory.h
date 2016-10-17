#ifndef _OBJECT_3D_FACTORY_H_
#define _OBJECT_3D_FACTORY_H_

#include "Object3D.h"
#include "StaticObject3D.h"
#include "AnimatedObject3D.h"

#include <map>

class Object3DFactory
{
	public:
		Object3DFactory();
		Object3DFactory(const Object3DFactory &);
		~Object3DFactory();

		static Object3DFactory* Instance();

		void Register(const std::string &object3DType, CreateObject3DFn pfnCreate);
		Object3D* CreateObject3D(const std::string &object3DType, GraphicsManager* graphicsManager, const std::string &modelName);

	private:
		// Singleton
		static Object3DFactory object3DFactory_;
		
		// Map where we are going to save the Create functions
		std::map<std::string, CreateObject3DFn>	factoryMap_;

		int createdObjects_;

};

#endif //_OBJECT_3D_FACTORY_H_