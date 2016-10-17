#ifndef _FRIEND_SPACESHIP_CLASS_H_
#define _FRIEND_SPACESHIP_CLASS_H_

#include "../Engine/Object3DFactory.h"
#include "../Engine/particleSystem.h"
#include "../Engine/sphereCollision.h"

#include "../Game/logClass.h"
#include "../Game/soundThirdClass.h"

#include "../Graphics/graphicsManager.h"
#include "../Graphics/imageClass.h"

#include "../Math/pointClass.h"
#include "../Math/vectorClass.h"

#include "../Utils/notifierClass.h"

#include <math.h>

enum DirtColor
	{
		BLUE,
		GREEN,
		RED
	};

class FriendSpaceShipClass : public Notifier<FriendSpaceShipClass, bool>
{
	private:
		enum FriendSpaceShipState
			{
				WAITING,
				ENTERING,
				STUCK,
				SLOW_MOTION,
				GOING_AWAY,
				OUTSIDE,
				CELEBRATING,
				CELEBRATING_OUTSIDE
			};

	public:
		FriendSpaceShipClass();
		FriendSpaceShipClass(const FriendSpaceShipClass&);
		~FriendSpaceShipClass();

		bool setup(GraphicsManager* graphicsManager, SoundThirdClass* soundManager, std::string fileName);
		void update(float elapsedTime);
		void draw(GraphicsManager* graphicsManager, XMFLOAT4X4 worldMatrix, XMFLOAT4X4 viewMatrix, XMFLOAT4X4 projectionMatrix, LightClass* light, bool debug);
		void destroy();

		void setPosition(Point newPosition);
		Point getPosition();

		void goToPosition(Point pos);
		void flyToPosition(Point pos);
		void celebrateToPosition(Point pos);
		bool checkArrivedObjective();

		void freeIt(float elapsedTime);
		void makeItFree();

		bool isOut();
		bool hasCelebrated();
		bool isStuck();
		bool isWaiting();

		void setScale(Vector newScale);
		Vector getScale();

		void setRotationX(float rotX);
		float getRotationX();
		void setRotationY(float rotY);
		float getRotationY();
		void setRotationZ(float rotZ);
		float getRotationZ();

		void setType(DirtColor color);

		Point getViewportPosition();
		bool toLeft_;

	private:
		void updateViewportPosition();

		SoundThirdClass*	soundManager_;

		ParticleSystem*		spaceShipParticles_;
		ParticleSystem*		dirt_;
		float				dirtDistance_;
		float				initialDirtDistance_;
		DirtColor			dirtColor_;

		Object3D*			model_;

		Point				viewportPosition_;
		ImageClass*			viewportImage_;
		XMFLOAT4X4			worldMatrix_, viewMatrix_, projectionMatrix_;
		int					screenWidth_, screenHeight_;

		Point				position_;
		Point				endPosition_;
		Point				newPosition_;
		Vector				scaling_; 
		float				rotX_;
		float				rotY_; 
		float				rotZ_;

		bool				touched_;

		Vector				velocity_;

		FriendSpaceShipState friendSpaceShipState_;

};

#endif //_FRIEND_SPACESHIP_CLASS_H_
