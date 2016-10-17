#ifndef _PRESENT_CLASS_H_
#define _PRESENT_CLASS_H_

#include "../Engine/Object3DFactory.h"
#include "../Engine/particleSystem.h"
#include "../Engine/sphereCollision.h"

#include "../Game/friendSpaceShipClass.h"
#include "../Game/logClass.h"
#include "../Game/soundFourthClass.h"

#include "../Graphics/graphicsManager.h"
#include "../Graphics/imageClass.h"

#include "../Math/pointClass.h"
#include "../Math/vectorClass.h"

#include "../Utils/notifierClass.h"

#include <math.h>

class PresentClass : public Notifier<PresentClass, bool>
{
	private:
		enum PresentState
			{
				WAITING,
				SPAWNING,
				IN_BUILDING,
				TOUCHED,
				IN_PLATFORM,
				OPENING,
				DISAPEARING
			};
	public:
		PresentClass();
		PresentClass(const PresentClass&);
		~PresentClass();

		bool setup(GraphicsManager* graphicsManager, SoundFourthClass* soundManager, std::string fileName);
		void update(float elapsedTime);
		void draw(GraphicsManager* graphicsManager, XMFLOAT4X4 worldMatrix, XMFLOAT4X4 viewMatrix, XMFLOAT4X4 projectionMatrix, LightClass* light, bool debug);
		void destroy();

		void setPosition(Point newPosition);
		void setInitialPosition(Point newPosition);
		Point getPosition();

		void setScale(Vector newScale);
		Vector getScale();

		void setRotationX(float rotX);
		float getRotationX();
		void setRotationY(float rotY);
		float getRotationY();
		void setRotationZ(float rotZ);
		float getRotationZ();

		void setType(DirtColor color);
		DirtColor getType();

		Point getViewportPosition();
		SphereCollision* getCollisionSphere();

		bool isInBuilding();
		void setTouched();
		bool isInPlatform();
		void setInPlatform();
		void open();

	private:
		void updateViewportPosition();

		SoundFourthClass*	soundManager_;

		Object3D*			model_;
		ParticleSystem*		sparks_;
		SphereCollision*	collisionTest_;

		DirtColor			presentType_;
		PresentState		presentState_;

		Point				viewportPosition_;
		ImageClass*			viewportImage_;
		XMFLOAT4X4			worldMatrix_, viewMatrix_, projectionMatrix_;
		int					screenWidth_, screenHeight_;

		Point				position_;
		Vector				initialScaling_;
		Vector				endScaling_;
		Vector				scaling_;
		float				rotX_;
		float				rotY_; 
		float				rotZ_;

		float				waitedTime_;
		float				spawningTime_;
		float				openTime_;
};

#endif //_FRIEND_SPACESHIP_CLASS_H_
