#ifndef _PICO_FIRST_CLASS_H_
#define _PICO_FIRST_CLASS_H_

#include "../Application/inputManager.h"

#include "../Engine/Object3DFactory.h"

#include "../Game/soundFirstClass.h"

#include "../Graphics/graphicsManager.h"
#include "../Graphics/cameraClass.h"
#include "../Graphics/lightClass.h"

#include "../Math/pointClass.h"
#include "../Math/vectorClass.h"

#include "../Utils/listenerClass.h"
#include "../Utils/textClass.h"
#include "../Utils/clockClass.h"

#include "fruitClass.h"
#include "birdClass.h"

#include <deque>

#define UNHIDDING_STEPS 2

class PicoFirstClass : public Listener<InputManager, InputStruct>, public Listener<FruitClass, Point>, public Listener<BirdClass, bool>, public Notifier<PicoFirstClass, bool>
{
	private:
		enum PicoStates
			{
				HIDDING,
				HIDDEN,
				UNHIDDING,
				WAITING,
				WALKING,
				TURNING,
				EATING,
				CELEBRATING,
				SCARED,
				WAITING_GOODBYE,
				LEAVING
			};

		enum FaceStates
			{
				NORMAL,
				CHANGING,
				CHANGED
			};

	public:
		PicoFirstClass();
		PicoFirstClass(const PicoFirstClass&);
		~PicoFirstClass();

		bool setup(GraphicsManager* graphicsManager, CameraClass* camera, SoundFirstClass* soundManager);
		void update(float elapsedTime);
		void draw(GraphicsManager* graphicsManager, XMFLOAT4X4 worldMatrix, XMFLOAT4X4 viewMatrix, XMFLOAT4X4 projectionMatrix, LightClass* light, bool debug);
		void destroy();

		void setLevelState(int level);

		void goToPosition(Point position);
		void setToRest(bool ereasePrevious);

		void makeHappy();

		void sayHello();

		void makePointing();
		bool isPointing();
		int getLastEaten();
		int getPointedFruit();

		bool isWaiting();

		void makeGoodbye();

		void makeLeave();

		bool checkPicoArrivedObjective();

		void changeAnimation(std::string name, float time);
		void executeAnimation(std::string name, float time);
		void changeExpression(std::string newExpression);

		Point getPosition();

		void setTipsColor(XMFLOAT4 color);
		void setBodyTexture(TextureClass* texture);
		void setHat(Object3D* hat);
		void setBody(Object3D* body);
		void setTips(Object3D* tips);

		SphereCollision* getCollisionSphere();

		virtual void notify(InputManager* notifier, InputStruct arg);
		virtual void notify(FruitClass* notifier, Point arg);
		virtual void notify(BirdClass* notifier, bool arg);

	private:
		void loadExpressions(GraphicsManager* graphicsManager);
		
		float approach(float goal, float current, float dt);
		void walk(float elapsedTime);
		bool lookAtCamera(bool check);

		void eatFruit();

		void savePicoConfiguration();

		CameraClass* camera_;

		// 3D Models
		Object3D*	body_[3];
		int			bodyToDraw_;
		Object3D*	tips_[3];
		int			tipsToDraw_;
		Object3D*	eyes_;

		Object3D*	hats_[4];
		bool		drawHat_;
		int			hatToDraw_;

		SphereCollision* collisionTest_;

		int			levelState_;

		// Expressions
		std::map<std::string, TextureClass*> expressions_;

		XMFLOAT4	tipsColor_;
		LightClass* tipsLight_;

		std::string	textureName_;

		PicoStates  picoState_;

		FaceStates	faceState_;
		float		expressionChangeTime_;
		float		expressionPercentage_;
		std::string actualExpression_;
		std::string newExpression_;
		ClockClass* expressionClock_;
		ClockClass* inactivityClock_;

		float		behindFruit_;

		float		waitedTime_;
		float		eatingWaitTime_;
		float		celebratingWaitTime_;
		
		// Logic behind the thing of discerning if child will understand or not what Pico asks
		float		inactivityTime1_;
		float		inactivityTime2_;
		float		inactivityTime3_;
		float		pointingTime_;

		bool		pointing1_;
		bool		pointed1_;
		bool		pointing2_;
		bool		pointed2_;
		bool		pointing3_;
		bool		pointed3_;

		bool		pointed_;

		int			previousFruitEatenID_;
		int			lastFruitEatenID_;
		int			pointedFruitID_;

		// For leaving
		float		leavingTime_;
		float		sayingGoddbyeTime_;

		bool		saidGoodbye_;

		// Sound
		SoundFirstClass*	soundManager_;

		// Fruits
		std::deque<FruitClass*> fallenFruits_;

		// Position and movement
		Point		position_;
		Vector		velocity_;
		Point		objective_;
		Vector		lookAt_;

		Vector		scaling_; 
		float		rotX_;
		float		rotY_; 
		float		newRotY_;
		float		rotZ_;

		// Beginning
		Point		hiddingPosition_;
		bool		hasToHide_;

		// Unhidding
		Point		positionUnhidding_[UNHIDDING_STEPS];
		int			unhiddingStep_;

		TextClass*	info_;
};

#endif //_PICO_FIRST_CLASS_H_
