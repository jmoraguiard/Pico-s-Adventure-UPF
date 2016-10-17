#ifndef _PIECE_CLASS_H_
#define _PIECE_CLASS_H_

#include "../Engine/Object3DFactory.h"
#include "../Engine/particleSystem.h"
#include "../Engine/sphereCollision.h"

#include "../Game/logClass.h"
#include "../Game/soundSecondClass.h"

#include "../Graphics/graphicsManager.h"

#include "../Math/pointClass.h"
#include "../Math/vectorClass.h"

#include "../Utils/notifierClass.h"

#include <math.h>

class PieceClass : public Notifier<PieceClass, Point>
{
	private:
		enum PieceState
			{
				SPAWNING,
				IN_TREE,
				SHACKEN,
				FALLING,
				IN_FLOOR,
				IN_SPACESHIP
			};

	public:
		PieceClass();
		PieceClass(const PieceClass&);
		~PieceClass();

		bool setup(GraphicsManager* graphicsManager, SoundSecondClass* soundManager, std::string fileName, Point finalPos, Point position, float floorHeight, Vector scaling, float rotX, float rotY, float rotZ, int logicID);
		void update(float elapsedTime);
		void draw(GraphicsManager* graphicsManager, XMFLOAT4X4 worldMatrix, XMFLOAT4X4 viewMatrix, XMFLOAT4X4 projectionMatrix, LightClass* light, bool debug);
		void destroy();

		std::string getName();
		int  logicId_;

		void setFinalPosition();
		void setPosition(Point newPosition);
		Point getPosition();
		bool getInFinalPosition();

		void setScale(Vector newScale);
		Vector getScale();

		void setRotationX(float rotX);
		float getRotationX();
		void setRotationY(float rotY);
		float getRotationY();
		void setRotationZ(float rotZ);
		float getRotationZ();

		SphereCollision* getCollisionSphere();

		void lightIt();
		void makeItFall();

		bool hasFallen();

	private:
		SoundSecondClass* soundManager_;

		ParticleSystem* sparks_;

		Object3D*	model_;
		SphereCollision* collisionTest_;

		std::string name_;

		Point		finalPosition_;
		Point		position_;
		Vector		initialScaling_;
		Vector		endScaling_;
		Vector		scaling_; 
		Vector		velocity_;

		float		waitedTime_;
		float		spawningTime_;

		float		lightLevel_;
		float		lightedTime_;
		float		fallTime_;
		bool		shaken_;

		float		rotX_;
		float		rotY_; 
		float		rotZ_;

		float		floorHeight_;

		bool		hasFallen_;

		PieceState	pieceState_;

};

#endif //_FRUIT_CLASS_H_
