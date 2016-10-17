#ifndef _PLATFORM_CLASS_H_
#define _PLATFORM_CLASS_H_

#include "../Engine/Object3DFactory.h"
#include "../Engine/particleSystem.h"

#include "../Game/soundFourthClass.h"

#include "../Graphics/graphicsManager.h"

#include "../Math/pointClass.h"
#include "../Math/vectorClass.h"


class PlatformClass
{
	private:
		enum PlatformState
			{
				WAITING,
				GOING_TO,
				GETTING_PRESENT,
				COMING_BACK,
				BRINGING_BACK
			};

	public:
		PlatformClass();
		PlatformClass(const PlatformClass&);
		~PlatformClass();

		bool setup(GraphicsManager* graphicsManager, SoundFourthClass* soundManager);
		void update(float elapsedTime);
		void draw(GraphicsManager* graphicsManager, XMFLOAT4X4 worldMatrix, XMFLOAT4X4 viewMatrix, XMFLOAT4X4 projectionMatrix, LightClass* light, bool debug);
		void destroy();

		void goToPosition(Point position);
		void bringBackPresent();
		bool checkArrivedObjective();

		Point getPosition();
		bool isGettingPresent();
		bool isWaiting();

	private:
		void fly(float elapsedTime);

		SoundFourthClass* soundManager_;

		Object3D*	model_;
		ParticleSystem*	platformParticles_[2];
		PlatformState platformState_;


		Point		initialPosition_;
		Point		position_;
		Vector		velocity_;
		Point		objective_;

		float		floorHeight_;

		Vector		scaling_; 
		float		rotX_;
		float		rotY_; 
		float		rotZ_;
};

#endif
