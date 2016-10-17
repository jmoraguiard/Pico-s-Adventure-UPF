#ifndef _SPACE_SHIP_CLASS_H_
#define _SPACE_SHIP_CLASS_H_

#include "../Engine/Object3DFactory.h"
#include "../Engine/particleSystem.h"

#include "../Game/soundSecondClass.h"

#include "../Graphics/graphicsManager.h"

#include "../Math/pointClass.h"
#include "../Math/vectorClass.h"

enum SpaceShipState
	{
		WAITING,
		LAUNCHING,
		FLYING,
		ENLARGING,
		PREPARED
	};

class SpaceShipClass
{
	public:
		SpaceShipClass();
		SpaceShipClass(const SpaceShipClass&);
		~SpaceShipClass();

		bool setup(GraphicsManager* graphicsManager, SoundSecondClass* soundManager, float velocityConstant);
		void update(float elapsedTime);
		void draw(GraphicsManager* graphicsManager, XMFLOAT4X4 worldMatrix, XMFLOAT4X4 viewMatrix, XMFLOAT4X4 projectionMatrix, LightClass* light, bool debug);
		void destroy();

		void goToPosition(Point position);

		void makeLaunch(int level);
		void makeBig(Point newPos, Vector newScale);

		bool isPrepared();
		bool checkArrivedObjective();

		void setTexture(std::string color);

		// Getters and setters
		Object3D* getObject();
		
		void setInitialPosition(Point pos);
		void setPosition(Point pos);
		Point getPosition();
		void setScale(Vector scale);
		void setRotation(float x, float y, float z);
		void setFloorHeight(float floor);

		void setSpaceShipState(SpaceShipState state);

	private:
		void fly(float elapsedTime);

		SoundSecondClass* soundManager_;

		Object3D*	model_;
		ParticleSystem*	spaceShipParticles_;
		int			spaceShipEnergylevel_;

		TextureClass* orange_;
		TextureClass* blue_; 
		TextureClass* green_;

		SpaceShipState spaceShipState_;

		Point		initialPosition_;
		Point		position_;
		Vector		velocity_;
		float		velocityConstant_;
		Point		objective_;

		float		floorHeight_;

		Vector		scaling_; 
		float		rotX_;
		float		rotY_; 
		float		rotZ_;

		Point		enlargedInitialPosition_;
		Vector		enlargedInitialScale_;
		Point		enlargedFinalPosition_;
		Vector		enlargedFinalScale_;
		float		enlargingTime_;
		float		enlargingElapsedTime_;
};

#endif //_BIRD_CLASS_H_
