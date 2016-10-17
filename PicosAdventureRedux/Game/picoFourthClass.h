#ifndef _PICO_FOURTH_CLASS_H_
#define _PICO_FOURTH_CLASS_H_

#include "../Application/inputManager.h"

#include "../Engine/Object3DFactory.h"

#include "../Game/soundFourthClass.h"

#include "../Graphics/graphicsManager.h"
#include "../Graphics/cameraClass.h"
#include "../Graphics/lightClass.h"

#include "../Math/pointClass.h"
#include "../Math/vectorClass.h"

#include "../Utils/listenerClass.h"
#include "../Utils/clockClass.h"
#include "../Utils/textClass.h"

#include "friendSpaceShipClass.h"

class PicoFourthClass : public Listener<InputManager, InputStruct>, public Listener<FriendSpaceShipClass, bool>, public Notifier<PicoFourthClass, bool>
{
	private:
		enum PicoStates
			{
				WAITING,
				WALKING,
				TURNING,
				CELEBRATING,
				LEAVING
			};

		enum FaceStates
			{
				NORMAL,
				CHANGING,
				CHANGED
			};

	public:
		PicoFourthClass();
		PicoFourthClass(const PicoFourthClass&);
		~PicoFourthClass();

		bool setup(GraphicsManager* graphicsManager, CameraClass* camera, SoundFourthClass* soundManager, int num);
		void update(float elapsedTime);
		void draw(GraphicsManager* graphicsManager, XMFLOAT4X4 worldMatrix, XMFLOAT4X4 viewMatrix, XMFLOAT4X4 projectionMatrix, LightClass* light, bool debug);
		void destroy();

		void setLevelState(int level);

		// Moving PICO
		void goToPosition(Point position);
		Point getPosition();
		void setPosition(Point pos);
		bool checkPicoArrivedObjective();
		bool isPicoWaiting();

		bool lookAtCamera(bool check);

		// Changing Pico public behaviour
		void makeCelebrate();
		void makeDanceAss();
		void makeGreeting();
		void makeHappy();
		void makeLeave();
		void makeNo();
		void makePointing(bool right);
		void makeRest(bool lookCamera);

		// Change Pico configuration
		void changeAnimation(std::string name, float time);
		void executeAnimation(std::string name, float time);
		void changeExpression(std::string newExpression);

		void notify(InputManager* notifier, InputStruct arg);
		void notify(FriendSpaceShipClass* notifier, bool arg);
	private:
		void loadConfiguration(GraphicsManager* graphicsManager, int num);
		void loadExpressions(GraphicsManager* graphicsManager);
		
		float approach(float goal, float current, float dt);
		void walk(float elapsedTime);

		CameraClass* camera_;
		SoundFourthClass* soundManager_;
		int			levelState_;

		// 3D Models
		Object3D*	body_;
		Object3D*	tips_;
		Object3D*	eyes_;
		Object3D*	hat_;

		// Pico state
		XMFLOAT4	tipsColor_;
		LightClass* tipsLight_;

		PicoStates  picoState_;

		// Expressions
		std::map<std::string, TextureClass*> expressions_;

		FaceStates	faceState_;
		float		expressionChangeTime_;
		float		expressionPercentage_;
		std::string actualExpression_;
		std::string newExpression_;
		ClockClass* expressionClock_;

		ClockClass* inactivityClock_;

		// Time thresholds
		float		waitedTime_;
		float		eatingWaitTime_;
		float		celebratingWaitTime_;
		float		inactivityTime_;

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

		// Debug
		TextClass*	info_;
};

#endif //_PICO_THIRD_CLASS_H_
