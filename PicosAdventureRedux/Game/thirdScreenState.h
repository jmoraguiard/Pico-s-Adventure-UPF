#ifndef _THIRD_SCREEN_STATE_H_
#define _THIRD_SCREEN_STATE_H_

#include "../Application/applicationState.h"

#include "../Game/friendSpaceShipClass.h"
#include "../Game/logClass.h"
#include "../Game/picoThirdClass.h"
#include "../Game/soundThirdClass.h"
#include "../Game/spriteAnimationClass.h"
#include "../Game/spaceShipClass.h"

#include "../Graphics/Cal3DModelClass.h"
#include "../Graphics/imageClass.h"
#include "../Graphics/lightClass.h"
#include "../Graphics/textureClass.h"

#include "../Utils/listenerClass.h"
#include "../Utils/textClass.h"

#include "../Engine/Object3DFactory.h"
#include "../Engine/particleSystem.h"

#include "../Utils/clockClass.h"

#include <mmsystem.h>

class ThirdScreenState: public ApplicationState, public Listener<KinectClass, KinectStruct>, public Listener<FriendSpaceShipClass, bool>
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
			LANDING,
			PICO_APPEARS,
			LEAVING,
			GREETING,
			TRANSITION
		};

	enum PlayingLevel
		{
			LEVEL_1,
			CELEBRATING_LEVEL_1,
			LEVEL_2,
			CELEBRATING_LEVEL_2,
			LEVEL_3,
			CELEBRATING_LEVEL_3,
			LEVEL_4,
			CELEBRATING_LEVEL_4,
			FADING,
			SELECT_MEDAL
		};

	public:
		ThirdScreenState();
		virtual ~ThirdScreenState();
		static ThirdScreenState* Instance();

		virtual bool setup(ApplicationManager* appManager, GraphicsManager* graphicsManager, InputManager* inputManager, KinectClass* kinectManager);
		virtual void update(float elapsedTime);
		virtual void draw();
		virtual void destroy();

		virtual void notify(InputManager* notifier, InputStruct arg);
		void notify(KinectClass* notifier, KinectStruct arg);
		void notify(FriendSpaceShipClass* notifier, bool arg);
	private:
		static ThirdScreenState thirdScreenState_; //singleton

		bool setupAllSpaceShips();
		bool setupDebugOptions();
		bool setupFriendSpaceShips();
		bool setupKinectHands();
		bool setupRayInfo();
		bool setupRays();
		bool setupSplashScreen();

		void updateIntroduction(float elapsedTime);
		void updateKinectHands();
		void updatePlaying(float elapsedTime);
		void updatePlayingSublevel1(float elapsedTime);
		void updatePlayingSublevel2(float elapsedTime);
		void updatePlayingSublevel3(float elapsedTime);
		void updatePlayingSublevel4(float elapsedTime);
		void updateCelebration(float elapsedTime);
		void updateRayInfo();

		void drawSplashScreen();

		void launchRandomFriendSpaceShip();

		void loadConfigurationFromXML();

		bool loadScenario(std::string scenario);
		void createScenarioObject(std::string scenario, std::string xmlName);

		void changeLevel(LevelState level);

		ImageClass*					splashScreen_;

		CameraClass*				camera_;
		LightClass*					light_;
		LightClass*					piecesLight_;
		ClockClass*					gameClock_;
		SoundThirdClass*			soundManager_;
		KinectClass*				kinectClass_;

		int							screenWidth_;
		int							screenHeight_;

		// Game state and control variables
		LevelState					levelState_;
		IntroLevel					introLevelState_;
		PlayingLevel				playLevelState_;

		// Introduction control variables
		ClockClass*					introClock_;
		ClockClass*					playClock_;
		ClockClass*					pointingClock_;
		ClockClass*					noClock_;
		bool						isPointing_;

		// Kinect
		Point						kinectHandViewPos_[2];
		Point						kinectHandWorldPos_[2];
		float						kinectHandRot_[2];
		float						kinectHandPreviousRot_[2];
		ImageClass*					kinectHand_;

		// Scenario structure
		ImageClass*					background_;
		Point						backgrounPosition_;

		std::vector<Object3D*>		scenario_;
		float						terrainHeight_;

		PicoThirdClass*				pico_;

		// Timers
		float						spaceShipIntroWaitTime_;
		float						introFallTime_;
		float						introGreetTime_;
		float						introSpaceTime_;

		//Object3D
		SpaceShipClass*				spaceShip_;
		FriendSpaceShipClass*		friendSpacheShips_[5];
		int							actualFriendSpaceShip_;

		//Object2D
		SpriteAnimationClass*		blueRay_[2];
		SpriteAnimationClass*		redRay_[2];
		SpriteAnimationClass*		greenRay_[2];
		bool						drawRays_;

		// Playing logic
		float						distanceThreshold_;
		int							activeRay_;
		int							secondLevelIteration_;

		bool						bothTouching_;
		bool						lastOutLeft_;
		int							subLevel4Freed_;

		bool						picoDanced_;
		int							thirdLevelTimes_;

		// Celebrate
		ParticleSystem*				celebrationParticles_[5];
		float						celebrationTime_;

		// Medals
		Object3D*					medals_[3];
		float						fadeTime_;
		int							actualLevelState_;
	
		// Debug
		bool						debug_;
		TextClass*					FPS_;
		TextClass*					kinectHands_[2];
		TextClass*					rayInfo_[2];
};

#endif //_THIRD_SCREEN_STATE_H_
