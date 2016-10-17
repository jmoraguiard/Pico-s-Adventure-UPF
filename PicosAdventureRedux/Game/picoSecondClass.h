#ifndef _PICO_SECOND_CLASS_H_
#define _PICO_SECOND_CLASS_H_

#include "../Application/inputManager.h"

#include "../Engine/Object3DFactory.h"

#include "../Game/soundSecondClass.h"

#include "../Graphics/graphicsManager.h"
#include "../Graphics/cameraClass.h"
#include "../Graphics/lightClass.h"

#include "../Math/pointClass.h"
#include "../Math/vectorClass.h"

#include "../Utils/listenerClass.h"
#include "../Utils/clockClass.h"
#include "../Utils/textClass.h"

#include "pieceClass.h"

#include <deque>

class PicoSecondClass : public Listener<InputManager, InputStruct>, public Listener<PieceClass, Point>, public Notifier<PicoSecondClass, bool>
{
	private:
		enum PicoStates
			{
				WAITING,
				WALKING,
				TURNING,
				CELEBRATING,
				LEAVING,
				WORKING
			};

		enum FaceStates
			{
				NORMAL,
				CHANGING,
				CHANGED
			};

	public:
		PicoSecondClass();
		PicoSecondClass(const PicoSecondClass&);
		~PicoSecondClass();

		bool setup(GraphicsManager* graphicsManager, CameraClass* camera, SoundSecondClass* soundManager);
		void update(float elapsedTime);
		void draw(GraphicsManager* graphicsManager, XMFLOAT4X4 worldMatrix, XMFLOAT4X4 viewMatrix, XMFLOAT4X4 projectionMatrix, LightClass* light, bool debug);
		void destroy();

		void setLevelState(int level);

		// Moving PICO
		void goToPosition(Point position);
		Point getPosition();
		bool checkPicoArrivedObjective();
		bool isPicoWaiting();

		void setSpaceShipPosition(Point position);

		// Changing Pico public behaviour
		void makeCelebrate();
		void makeDanceAss();
		void makeGreeting();
		void makeHappy();
		void makeLeave();
		void makeNo();
		void makePointing();
		void makeRest(bool lookCamera);
		void makeWork();

		// Change Pico configuration
		void changeAnimation(std::string name, float time);
		void executeAnimation(std::string name, float time);
		void changeExpression(std::string newExpression);

		SphereCollision* getCollisionSphere();

		virtual void notify(InputManager* notifier, InputStruct arg);
		virtual void notify(PieceClass* notifier, Point arg);

		// Light for shaders
		void setLightPositions(XMFLOAT4 pos1, XMFLOAT4 pos2, XMFLOAT4 pos3, XMFLOAT4 pos4, XMFLOAT4 pos5);

	private:
		void loadConfiguration(GraphicsManager* graphicsManager);
		void loadExpressions(GraphicsManager* graphicsManager);
		
		float approach(float goal, float current, float dt);
		void walk(float elapsedTime);
		bool lookAtCamera(bool check);

		CameraClass* camera_;
		SoundSecondClass* soundManager_;
		int			levelState_;

		// 3D Models
		Object3D*	body_;
		Object3D*	tips_;
		Object3D*	eyes_;
		Object3D*	hat_;

		SphereCollision* collisionTest_;

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
		float		workingTime_;
		float		inactivityTime_;

		// Loop behaviours
		bool		pointingPieces_;

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

		// Piece control
		std::deque<PieceClass*> fallenPieces_;
		bool		goingToPiece_;
		bool		draggingPiece_;
		Point		spaceShipPosition_;

		// Debug
		TextClass*	info_;
};

#endif //_PICO_SECOND_CLASS_H_
