#ifndef _STAR_CLASS_H_
#define _STAR_CLASS_H_

#include "../Engine/Object3DFactory.h"
#include "../Engine/particleSystem.h"
#include "../Engine/sphereCollision.h"

#include "../Game/soundSecondClass.h"

#include "../Graphics/graphicsManager.h"

#include "../Math/pointClass.h"
#include "../Math/vectorClass.h"

#include <math.h>

class StarClass
{
	private:
		enum StarState
			{
				IN_SKY,
				FALLING,
				GO_TO_SPACESHIP,
				IN_FLOOR
			};

	public:
		StarClass();
		StarClass(const StarClass&);
		~StarClass();

		bool setup(GraphicsManager* graphicsManager, SoundSecondClass* soundManager, float floorHeight);
		void update(float elapsedTime);
		void draw(GraphicsManager* graphicsManager, XMFLOAT4X4 worldMatrix, XMFLOAT4X4 viewMatrix, XMFLOAT4X4 projectionMatrix, LightClass* light, bool debug);
		void destroy();

		std::string getName();

		void goToSpaceShip();

		void setInitialPosition(Point position);
		void setFinalPosition(Point position);
		void setPosition(Point position);
		Point getPosition();

		void setScale(Vector newScale);
		Vector getScale();

		SphereCollision* getCollisionSphere();

		void makeItFall(bool good);
		bool isFalling();
		bool hasFallen();
		bool isInTheFloor();
		bool isInTheSky();
		bool isGood();
		void reset();

	private:
		SoundSecondClass* soundManager_;

		Object3D*	model_;
		SphereCollision* collisionTest_;
		LightClass* badlight_;

		Point		initialPosition_;
		Point		finalPosition_;
		Point		position_;
		Vector		scaling_; 
		Vector		velocity_;

		bool		good_;

		float		waitedTime_;

		float		rotX_;
		float		rotY_; 
		float		rotZ_;

		float		floorHeight_;

		bool		hasFallen_;

		StarState	starState_;

};

#endif //_STAR_CLASS_H_
