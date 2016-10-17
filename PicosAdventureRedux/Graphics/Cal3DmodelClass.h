#ifndef _CAL3D_MODEL_CLASS_H_
#define _CAL3D_MODEL_CLASS_H_

#pragma warning( disable : 4005 )

#include <d3d11.h>

#include <Windows.h>

#include "cal3d/cal3d.h"

#include "modelClass.h"

#include <iostream>
#include <fstream>
#include <sstream>

class Cal3DModelClass : public ModelClass
{
	public:
		Cal3DModelClass();
		Cal3DModelClass(const Cal3DModelClass&);
		virtual ~Cal3DModelClass();

		virtual void destroy();

	protected:
		void destroyModel();

		// Cal3D Model
		CalCoreModel* cal3dCoreModel_;
		CalModel* cal3dModel_;

		// Meshes
		int modelMeshID;
};

#endif //_CAL3D_MODEL_CLASS_H_
