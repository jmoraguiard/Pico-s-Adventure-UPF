#ifndef _FIRST_SCREEN_STATE_H_
#define _FIRST_SCREEN_STATE_H_

#include "../Application/applicationState.h"

#include "../Engine/Object3DFactory.h"

#include "../Game/logClass.h"
#include "../Game/soundFirstClass.h"

#include "../Graphics/Cal3DModelClass.h"
#include "../Graphics/textureClass.h"
#include "../Graphics/lightClass.h"

#include "../GUI/GUIManager.h"

#include "../Utils/clockClass.h"
#include "../Utils/listenerClass.h"
#include "../Utils/textClass.h"

#include "picoFirstClass.h"
#include "fruitClass.h"
#include "birdClass.h"
#include "spaceShipClass.h"

#include <mmsystem.h>

class FirstScreenState: public ApplicationState, public Listener<GUIButton, ButtonStruct>, public Listener<KinectClass, KinectStruct>, public Listener<FruitClass, Point>
{
	enum LevelState
		{
			INTRODUCTION,
			FIRST_LEVEL = 1,
			SECOND_LEVEL,
			THIRD_LEVEL,
			FOURTH_LEVEL,
			ENDING
		};

	enum SubLevelState
		{
			PLAYING,
			FADING,
			SELECT_POLAROID
		};

	public:
		FirstScreenState();
		virtual ~FirstScreenState();
		static FirstScreenState* Instance();

		virtual bool setup(ApplicationManager* appManager, GraphicsManager* graphicsManager, InputManager* inputManager, KinectClass* kinectManager);
		virtual void update(float elapsedTime);
		virtual void draw();
		virtual void destroy();

		virtual void notify(InputManager* notifier, InputStruct arg);
		virtual void notify(GUIButton* notifier, ButtonStruct arg);
		virtual void notify(KinectClass* notifier, KinectStruct arg);
		virtual void notify(FruitClass* notifier, Point arg);

	private:
		static FirstScreenState firstScreenState_; //singleton

		void updateLevel();

		void setupGUI(GraphicsManager* graphicsManager, InputManager* inputManager, KinectClass* kinectManager);

		void loadConfigurationFromXML();

		void loadScenario(std::string scenario);
		void createScenarioObject(std::string scenario, std::string xmlName);

		bool loadFruits();
		void addFruitsToGame();
		void clearFruits();
		void clearFruitsInGame();

		bool createPolaroids();
		void clearPolaroids();

		bool setupSplashScreen();
		void drawSplashScreen();

		void changeLevel(LevelState level);

		ImageClass*					splashScreen_;

		CameraClass*				camera_;
		LightClass*					light_;
		ClockClass*					gameClock_;

		int							screenWidth_;
		int							screenHeight_;

		// Game state and control variables
		LevelState					levelState_;
		SubLevelState				subLevelState_;

		float						introductionTime_;
		float						playingTime_;
		float						fadeTime_;

		Point						kinectTorsoPos_;
		Point						kinectHandPos_;
		ImageClass*					kinectHand_;
		TextClass*					kinectHandText_;

		// Save positions
		ClockClass*					savePositionsClock_;
		float						timeToSavePositions_;

		// Scenario structure
		ImageClass*					background_;
		Point						backgrounPosition_;

		std::vector<Object3D*>		scenario_;
		float						terrainHeight_;

		// First fruit
		SphereCollision*			firstAppleCollisionTest_;
		bool						firstFallen_;

		// Characters!
		PicoFirstClass*				pico_;
		bool						picoHeadTouchedRight_;
		bool						picoHeadTouchedLeft_;
		Point						picoScreenPos_;

		bool						firstLeave_;

		bool						activatedSignaledFruit_;

		BirdClass*					bird_;

		SpaceShipClass*				spaceShip_;
		float						spaceshipSoundTime_;
		bool						spaceshipSoundPlayed_;

		// Fruit vector and in game fruit vector
		std::vector<FruitClass*>	fruits_;
		std::vector<FruitClass*>	fruitsInGame_;

		// Classes for managing between game image selection
		GUIManager*					polaroidGUI_;
		GUIFrame*					polaroidFrame_;
		std::vector<std::string>	polaroidsName_;

		// Sound
		SoundFirstClass*			soundManager_;

		// Debug
		bool						debug_;
		TextClass*					FPS_;
};

#endif //_FIRST_SCREEN_STATE_H_
