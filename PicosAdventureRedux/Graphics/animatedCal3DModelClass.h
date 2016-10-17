#ifndef _ANIMATED_CAL3D_MODEL_CLASS_H_
#define _ANIMATED_CAL3D_MODEL_CLASS_H_

#pragma warning( disable : 4005 )

#include <d3d11.h>
#include <d3dx10math.h>

#include <Windows.h>

#include "Cal3dModelClass.h"

#include "../Utils/pugiconfig.h"
#include "../Utils/pugixml.h"

#include <iostream>
#include <fstream>
#include <sstream>

class AnimatedCal3DModelClass : public Cal3DModelClass
{
	public:
		AnimatedCal3DModelClass();
		AnimatedCal3DModelClass(const AnimatedCal3DModelClass&);
		virtual ~AnimatedCal3DModelClass();

		virtual bool setup(ID3D11Device*, std::string);
		void update(float elapsedTime);
		virtual void draw(ID3D11Device*, ID3D11DeviceContext*);

		void decreaseAnimationToDisplay();
		void increaseAnimationToDisplay();

		void setAnimationToExecute(std::string name, float interpolationTime);
		void setAnimationToPlay(std::string name, float interpolationTime);

		void playAnimation();
		void stopAnimation();

		CalSkeleton* getSkeleton();
		void setSkeleton(CalSkeleton* skeleton);

		CalMesh* getMesh();
		void setMesh(CalMesh* mesh);

	private:
		virtual bool setupBuffers(ID3D11Device*);
		bool updateBuffers(ID3D11DeviceContext*);

		virtual bool parseModelConfiguration(std::string strFilename);

		// Animations
		std::map<std::string, int> animations_;
		std::string animationToPlay_;
		std::string initialAnimationToPlay_;
};

#endif //_ANIMATED_CAL3D_MODEL_CLASS_H_
