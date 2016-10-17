#ifndef _INTRO_SCREEN_STATE_H_
#define _INTRO_SCREEN_STATE_H_

#include "../Application/applicationState.h"

#include "../Graphics/imageClass.h"
#include "../Graphics/lightClass.h"

#include "../GUI/GUIManager.h"

#include "../Utils/clockClass.h"
#include "../Utils/listenerClass.h"

#include <mmsystem.h>

class IntroScreenState: public ApplicationState, public Listener<KinectClass, KinectStruct>
{

	public:
		IntroScreenState();
		virtual ~IntroScreenState();
		static IntroScreenState* Instance();

		virtual bool setup(ApplicationManager* appManager, GraphicsManager* graphicsManager, InputManager* inputManager, KinectClass* kinectManager);
		virtual void update(float elapsedTime);
		virtual void draw();
		virtual void destroy();

		virtual void notify(InputManager* notifier, InputStruct arg);
		void notify(KinectClass* notifier, KinectStruct arg);

	private:
		static IntroScreenState introScreenState_; //singleton

		CameraClass*				camera_;
		LightClass*					light_;
		ClockClass*					gameClock_;

		int							screenWidth_;
		int							screenHeight_;

		// Scenario structure
		ImageClass*					background_;
		Point						backgrounPosition_;
};

#endif //_FIRST_SCREEN_STATE_H_
