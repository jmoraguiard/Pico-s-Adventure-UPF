#include "Cal3DModelClass.h"

Cal3DModelClass::Cal3DModelClass():ModelClass()
{
	cal3dCoreModel_ = 0;
	cal3dModel_ = 0;
}

Cal3DModelClass::Cal3DModelClass(const Cal3DModelClass& other)
{
}

Cal3DModelClass::~Cal3DModelClass()
{
}

void Cal3DModelClass::destroy()
{
	// Release the vertex and index buffers.
	destroyBuffers();

	// Release the model
	destroyModel();

	return;
}

void Cal3DModelClass::destroyModel()
{
	if(cal3dModel_)
	{
		delete cal3dModel_;
		cal3dModel_ = 0;
	}

	if(cal3dCoreModel_)
	{
		delete cal3dCoreModel_;
		cal3dCoreModel_ = 0;
	}

	return;
}

