#ifndef _FOURTH_SCREEN_STATE_H_
#define _FOURTH_SCREEN_STATE_H_

#include "../Application/applicationState.h"

#include "../Engine/Object3DFactory.h"
#include "../Engine/particleSystem.h"

#include "../Game/logClass.h"
#include "../Game/picoFourthClass.h"
#include "../Game/platformClass.h"
#include "../Game/presentClass.h"
#include "../Game/soundFourthClass.h"
#include "../Game/spriteAnimationClass.h"
#include "../Game/spaceShipClass.h"
#include "../Game/toyClass.h"

#include "../Graphics/Cal3DModelClass.h"
#include "../Graphics/imageClass.h"
#include "../Graphics/lightClass.h"
#include "../Graphics/textureClass.h"

#include "../Utils/listenerClass.h"
#include "../Utils/textClass.h"

#include "../Utils/clockClass.h"

#include <mmsystem.h>

class FourthScreenState: public ApplicationState, public Listener<KinectClass, KinectStruct>
{
	enum LevelState
		{
			INTRODUCTION,
			PLAYING,
			ENDING
		};

	enum IntroLevel
		{
			ARRIBING,
			PICO_NEXT_PLATFORM,
			GREETING,
			TURNING,
			GREETING_CHILDREN,
			PICO_ON_PLATFORM,
			PRESENT_APPEARING,
			POINTING,
			TRANSITION
		};

	enum PlayingLevel
		{
			LEVEL_1,
			TRANSITION_1_2,
			LEVEL_2,
			TRANSITION_2_3,
			LEVEL_3,
			LEVEL_4
		};

	public:
		FourthScreenState();
		virtual ~FourthScreenState();
		static FourthScreenState* Instance();

		virtual bool setup(ApplicationManager* appManager, GraphicsManager* graphicsManager, InputManager* inputManager, KinectClass* kinectManager);
		virtual void update(float elapsedTime);
		virtual void draw();
		virtual void destroy();

		virtual void notify(InputManager* notifier, InputStruct arg);
		void notify(KinectClass* notifier, KinectStruct arg);
	private:
		static FourthScreenState fourthScreenState_; //singleton

		bool setupDebugOptions();
		bool setupKinectHands();
		bool setupPresents();
		bool setupRayInfo();
		bool setupRays();
		bool setupSplashScreen();

		void updateIntroduction(float elapsedTime);
		void updateKinectHands();
		void updatePlaying(float elapsedTime);
		void updatePlayingSublevel1(float elapsedTime);
		void updatePlayingSublevel2(float elapsedTime);
		void updatePlayingSublevel3(float elapsedTime);
		void updateRayCrossing();
		void updateRayInfo();

		void drawSplashScreen();

		void openPresent(int i);
		void presentTouched(int i);
		void preparePresent(PresentClass* present, Point position, DirtColor type);
		Point intersection(Point p1, Point p2, Point p3, Point p4);

		void loadConfigurationFromXML();

		bool loadScenario(std::string scenario);
		void createScenarioObject(std::string scenario, std::string xmlName);

		bool loadPresents();

		void changeLevel(LevelState level);

		ImageClass*					splashScreen_;

		CameraClass*				camera_;
		LightClass*					light_;
		ClockClass*					gameClock_;
		SoundFourthClass*			soundManager_;
		KinectClass*				kinectClass_;

		int							screenWidth_;
		int							screenHeight_;

		// Game state and control variables
		LevelState					levelState_;
		IntroLevel					introLevelState_;
		PlayingLevel				playLevelState_;

		// Kinect
		Point						kinectHandViewPos_[2];
		Point						kinectHandWorldPos_[2];
		float						kinectHandRot_[2];
		float						kinectHandPreviousRot_[2];
		ImageClass*					kinectHand_;

		// Ray
		Point						rayEndViewPos_[2];
		Point						raysCrossing_;
		ImageClass*					raysCrossingImage_;
		SpriteAnimationClass*		blueRay_[2];
		SpriteAnimationClass*		redRay_[2];
		SpriteAnimationClass*		greenRay_[2];
		bool						drawRays_;
		DirtColor					activeRay_;
		float						distanceThreshold_;

		// Scenario structure
		ImageClass*					background_;
		Point						backgrounPosition_;

		std::vector<Object3D*>		scenario_;
		float						terrainHeight_;

		// Clocks
		ClockClass*					playClock_;
		ClockClass*					introClock_;
		ClockClass*					noClock_;

		// Game
		PicoFourthClass*			picos_[2];
		PresentClass*				presents_[3];
		PlatformClass*				platform_;

		std::vector<ToyClass*>		toys_[2];
		int							toysFound_;

		// Logic
		bool						bothTouching_;
		bool						prepareNewPresent_;
		bool						waitOtherPresent_;
		bool						picosPointing_;
		float						timeToPoint_;
		bool						picosDanced_;
		bool						makeRestAfterPresentAppeared_;

		// Keyboard logic
		bool						goToPresent_[3];

		// Celebrate
		ParticleSystem*				celebrationParticles_[5];
		float						celebrationTime_;

		// Debug
		bool						debug_;
		TextClass*					FPS_;
		TextClass*					kinectHands_[2];
		TextClass*					rayInfo_[2];
};

#endif //_THIRD_SCREEN_STATE_H_
