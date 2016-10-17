#ifndef _FRUIT_CLASS_H_
#define _FRUIT_CLASS_H_

#include "../Engine/Object3DFactory.h"
#include "../Engine/particleSystem.h"
#include "../Engine/sphereCollision.h"

#include "../Game/logClass.h"
#include "../Game/soundFirstClass.h"

#include "../Graphics/graphicsManager.h"

#include "../Math/pointClass.h"
#include "../Math/vectorClass.h"

#include "../Utils/clockClass.h"
#include "../Utils/notifierClass.h"

#include <math.h>

enum FruitEffect
	{
		COLOR,
		TEXTURE,
		HAT,
		BODY
	};

class FruitClass : public Notifier<FruitClass, Point>
{
	private:
		enum FruitState
			{
				SPAWNING,
				IN_TREE,
				SHACKEN,
				FALLING,
				IN_FLOOR
			};

	public:
		FruitClass();
		FruitClass(const FruitClass&);
		~FruitClass();

		bool setup(GraphicsManager* graphicsManager, SoundFirstClass* soundManager, std::string fileName, Point position, float floorHeight, Vector scaling, float rotX, float rotY, float rotZ);
		void update(float elapsedTime);
		void draw(GraphicsManager* graphicsManager, XMFLOAT4X4 worldMatrix, XMFLOAT4X4 viewMatrix, XMFLOAT4X4 projectionMatrix, LightClass* light, bool debug);
		void destroy();

		std::string getName();

		int getLogicID();
		void setLogicID(int id);

		void setLevel(int level);

		void setPosition(Point newPosition);
		Point getPosition();

		void setScale(Vector newScale);
		Vector getScale();

		void setRotationX(float rotX);
		float getRotationX();
		void setRotationY(float rotY);
		float getRotationY();
		void setRotationZ(float rotZ);
		float getRotationZ();

		SphereCollision* getCollisionSphere();

		ParticleSystem* leafs_;

		void setFruitEffectType(FruitEffect effect);
		FruitEffect getFruitEffect();

		void setColorEffect(XMFLOAT4 color);
		XMFLOAT4 getColorEffect();

		void setTextureEffect(TextureClass* texture);
		TextureClass* getTextureEffect();

		void setHatEffect(Object3D* hat);
		Object3D* getHatEffect();

		void setBodyEffect(Object3D* body, Object3D* tips);
		Object3D* getBodyEffect();
		Object3D* getTipsEffect();

		void shakeIt();
		bool makeItFall();
		void resetFruit();

		void resetSpawningTime();

		bool hasFallen();

		void activateAlert(bool active);

	private:
		Object3D*	model_;
		SphereCollision* collisionTest_;

		SoundFirstClass*	soundManager_;

		std::string name_;
		int			logicFruitID_;

		int			gameLevel_;

		Point		initialPosition_;
		Point		position_;
		Vector		initialScaling_;
		Vector		endScaling_;
		Vector		scaling_;
		Vector		velocity_;

		float		waitedTime_;
		float		initialSpawningTime_;
		float		spawningTime_;

		float		shakenTime_;
		float		initialFallTime_;
		float		fallTime_;
		bool		shaken_;

		float		rotX_;
		float		rotY_; 
		float		rotZ_;

		float		floorHeight_;

		bool		hasFallen_;

		FruitState	fruitState_;

		bool		inAlertMode_;
		float		alertTime_;
		bool		alertDisplay_;
		ClockClass* alertClock_;

		// EFFECTS
		FruitEffect fruitEffect_;
		XMFLOAT4	colorEffect_;
		TextureClass* textureEffect_;
		Object3D*	hatEffect_;
		Object3D*	bodyEffect_;
		Object3D*	tipsEffect_;

};

#endif //_FRUIT_CLASS_H_
