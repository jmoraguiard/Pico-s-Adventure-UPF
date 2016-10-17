#ifndef _SECOND_SCREEN_STATE_H_
#define _SECOND_SCREEN_STATE_H_

#include "../Application/applicationState.h"

#include "../Game/logClass.h"
#include "../Game/soundSecondClass.h"
#include "../Game/spaceShipClass.h"
#include "../Game/starClass.h"

#include "../Graphics/Cal3DModelClass.h"
#include "../Graphics/textureClass.h"
#include "../Graphics/lightClass.h"

#include "../Utils/listenerClass.h"
#include "../Utils/textClass.h"

#include "../GUI/GUIManager.h"

#include "../Engine/Object3DFactory.h"
#include "../Engine/particleSystem.h"

#include "../Utils/clockClass.h"

#include "picoSecondClass.h"
#include "pieceClass.h"

#include <mmsystem.h>

class SecondScreenState: public ApplicationState, public Listener<KinectClass, KinectStruct>, public Listener<GUIButton, ButtonStruct>
{
	enum LevelState
		{
			INTRODUCTION,
			MOUNTING = 1,
			PRE_FADING,
			FADING,
			SELECT_POLAROID,
			TRANSITION,
			INTRO_COLLECTING,
			COLLECTING,
			ENDING
		};

	enum IntroLevel
		{
			WAIT_INIT_MOVEMENT,
			TO_NIGHT,
			PICO_ENTERING,
			GREETING,
			WAIT_HI,
			TO_PIECE,
			LIGHT_IT,
			DRAG_IT,
			WORK,
			GIVE_POWER,
			POWER_GIVEN
		};

	enum StarsIntroLevel
		{
			FIRST_STAR,
			FIRST_REACTION,
			PICO_GUIDING
		};

	public:
		SecondScreenState();
		virtual ~SecondScreenState();
		static SecondScreenState* Instance();

		virtual bool setup(ApplicationManager* appManager, GraphicsManager* graphicsManager, InputManager* inputManager, KinectClass* kinectManager);
		virtual void update(float elapsedTime);
		virtual void draw();
		virtual void destroy();

		virtual void notify(InputManager* notifier, InputStruct arg);
		virtual void notify(GUIButton* notifier, ButtonStruct arg);
		void notify(KinectClass* notifier, KinectStruct arg);

	private:
		static SecondScreenState secondScreenState_; //singleton

		void updatePicoScreenposition(XMFLOAT4X4 worldMatrix, XMFLOAT4X4 viewMatrix, XMFLOAT4X4 projectionMatrix);
		void updateIntroduction(float elapsedTime);
		void updateIntroCollecting(float elapsedTime);

		void updatePieces(float elapsedTime);
		void updateLightPositions();
		void updateParticlesPositions();

		void makeFirstStarFall();
		void makeStarFall();
		void makeShipIgnite(int level);

		void loadConfigurationFromXML();

		void loadScenario(std::string scenario);
		void createScenarioObject(std::string scenario, std::string xmlName);

		bool loadPieces();
		bool loadStars();

		void setupGUI(GraphicsManager* graphicsManager, InputManager* inputManager, KinectClass* kinectManager);
		bool createPolaroids();
		void clearPolaroids();

		bool powerToUser(float elapsedTime);

		void changeLevel(LevelState level);

		bool setupSplashScreen();
		void drawSplashScreen();

		ImageClass*					splashScreen_;

		CameraClass*				camera_;
		LightClass*					light_;
		LightClass*					piecesLight_;
		ClockClass*					gameClock_;
		SoundSecondClass*			soundManager_;
		KinectClass*				kinectClass_;

		int							screenWidth_;
		int							screenHeight_;

		// Game state and control variables
		LevelState					levelState_;
		IntroLevel					introLevelState_;
		StarsIntroLevel				starsIntroLevelState_;

		float						playingTime_;
		float						fadeTime_;

		bool						childSaysHi_;

		int							starLevel_;
		bool						starsFalling_;
		float						starFallTime_;
		float						betweenStarsTime_;
		Point						initialPositions_[3];
		Point						finalPositions_[3];
		bool						starTouched_;
		ClockClass*					starTouchedClock_;
		std::vector<int>			origins_;
		std::vector<int>			endings_;

		bool						hasPicoGreeted_;

		// Introduction control variables
		ClockClass*					introClock_;
		float						timeGreeting_;
		float						timeToNight_;

		// Kinect
		Point						kinectHandViewPos_[2];
		Point						kinectHandWorldPos_[2];
		Point						kinectHoldViewPos_;
		Point						kinectHoldWorldPos_;
		ImageClass*					kinectHand_;
		bool						kinectTracksSmbdy_;

		// Scenario structure
		ImageClass*					background_;
		ImageClass*					background2_;
		Point						backgrounPosition_;

		std::vector<Object3D*>		scenario_;
		float						terrainHeight_;

		//Object3D*					spaceShipObject_;
		SpaceShipClass*				spaceShip_;
		std::vector<PieceClass*>	pieces_;
		int							spaceShipLevel_;

		PicoSecondClass*			pico_;
		Point						picoScreenPos_;
		LightClass*					picoLight_;

		// Classes for managing between game image selection
		GUIManager*					polaroidGUI_;
		GUIFrame*					polaroidFrame_;
		std::vector<std::string>	polaroidsName_;

		StarClass*					star_;
		bool						starsOrder_[20];
		int							starIndex_;
		float						timeToWaitFirstStar_;

		// Pointlights
		Object3D*					lightBody_;
		XMFLOAT4					lightPos_[5];
		ParticleSystem*				lightParticles_[4];

		// Debug
		bool						debug_;
		TextClass*					FPS_;
		TextClass*					kinectHands_;
		TextClass*					lightPositions_;
};

#endif //_SECOND_SCREEN_STATE_H_
