#ifdef _WIN32
#include <windows.h>
#endif

#include <GL/gl.h>
#include <GL/glu.h>

#include "firstScreenState.h"

FirstScreenState FirstScreenState::firstScreenState_;

FirstScreenState::FirstScreenState()
{
	camera_ = 0;
	light_ = 0;
	gameClock_ = 0;

	soundManager_ = 0;

	pico_ = 0;
	bird_ = 0;
}

FirstScreenState::~FirstScreenState()
{

}

FirstScreenState* FirstScreenState::Instance()
{
	return (&firstScreenState_);
}

bool FirstScreenState::setup(ApplicationManager* appManager, GraphicsManager* graphicsManager, InputManager * inputManager, KinectClass* kinectManager)
{
	// We get a pointer to the graphicsManager
	graphicsManager_ = graphicsManager;

	graphicsManager_->getScreenSize(screenWidth_, screenHeight_);

	LogClass::Instance()->setup("first_level_log");

	// Create the camera object.
	camera_ = new CameraClass();
	if(!camera_)
	{
		return false;
	}

	// Set the initial position of the camera.
	camera_->setPosition(0.0f, 2.5f, -10.0f);
	camera_->setup(XMFLOAT3(0.0f, 2.5f, -10.0f), XMFLOAT3(0.0f, 0.0f, 0.0f));

	if(!setupSplashScreen())
	{
		return false;
	}

	drawSplashScreen();

	// Create the light object.
	light_ = new LightClass;
	if(!light_)
	{
		return false;
	}

	// Initialize the light object.
	light_->setAmbientColor(0.1f, 0.1f, 0.1f, 1.0f);
	light_->setDiffuseColor(1.0f, 1.0f, 1.0f, 1.0f);
	light_->setDirection(0.0f, -0.5f, 1.0f);

	// load background and calculate its position
	background_ = new ImageClass;
	if(!background_)
	{
		MessageBoxA(NULL, "Could not initialize the background image instance.", "SecondScreen - Error", MB_OK);
		return false;
	}

	if(!background_->setup(graphicsManager_->getDevice(), graphicsManager_->getShader2D(), screenWidth_, screenHeight_, "sky_background", screenWidth_, screenHeight_))
	{
		MessageBoxA(NULL, "Could not setup the background image.", "SecondScreen - Error", MB_OK);
		return false;
	}

	backgrounPosition_.x = (screenWidth_/2)*-1;
	backgrounPosition_.y = (screenHeight_/2)+2;

	//drawSplashScreen();

	// Load the first level scenario
	loadScenario("level1");

	// Get terrain height for posterior setup of different objects
	terrainHeight_ = 0;
	std::vector<Object3D*>::iterator it;
	for(it = scenario_.begin(); it != scenario_.end(); it++)
	{
		if((*it)->getName().find("terreno") != std::string::npos)
		{
			terrainHeight_ = (*it)->getPosition().y-0.1f;
		}
	}

	// Set the level state to the first iteration and load fruits accordingly
	levelState_ = INTRODUCTION;
	subLevelState_ = PLAYING;

	loadConfigurationFromXML();

	// SOUND
	soundManager_ = new SoundFirstClass;
	if(!soundManager_)
	{
		return false;
	}
 
	// Initialize the sound object.
	if(!soundManager_->setup(graphicsManager_->getWindowHandler(), "comer_1"))
	{
		MessageBox(NULL, L"Could not initialize Direct Sound.", L"FirstScreen - Error", MB_OK);
		return false;
	}

	// Create the Pico object.
	pico_ = new PicoFirstClass();
	if(!pico_)
	{
		return false;
	}

	if(!pico_->setup(graphicsManager_, camera_, soundManager_))
	{
		MessageBoxA(NULL, "Could not initialize Pico :(.", "Error", MB_ICONERROR | MB_OK);
		return false;
	}
	inputManager->addListener(*pico_);

	picoHeadTouchedRight_ = false;
	picoHeadTouchedLeft_ = false;

	firstLeave_ = false;

	// Load the fruits
	loadFruits();

	firstAppleCollisionTest_ = new SphereCollision();
	firstAppleCollisionTest_->setup(graphicsManager, Point(-1.25f, 2.0f, -3.0f), 1.0f);
	firstFallen_ = false;

	// Create the bird object.
	bird_ = new BirdClass;
	if(!bird_)
	{
		return false;
	}

	if(!bird_->setup(graphicsManager_, soundManager_))
	{
		MessageBoxA(NULL, "Could not initialize the bird (is the word).", "Error", MB_ICONERROR | MB_OK);
		return false;
	}

	bird_->addListener(*pico_);
	pico_->addListener(*bird_);

	// Create the space ship.
	spaceShip_ = new SpaceShipClass;
	if(!spaceShip_)
	{
		return false;
	}

	if(!spaceShip_->setup(graphicsManager_, NULL, 7.0f))
	{
		MessageBoxA(NULL, "Could not initialize the SpaceShip.", "Error", MB_ICONERROR | MB_OK);
		return false;
	}
	spaceShip_->setInitialPosition(Point(40.0f, 25.0f, -5.0f));
	spaceShip_->setPosition(Point(40.0f, 25.0f, -5.0f));
	spaceShip_->setScale(Vector(0.03f, 0.03f, 0.03f));
	spaceShip_->setRotation(XM_PI/1.5f, XM_PI, XM_PI/8.0f);
	spaceShip_->goToPosition(Point(-20.0f, 0.0f, 30.0f));

	spaceshipSoundTime_ = 7.0f;
	spaceshipSoundPlayed_ = false;

	debug_ = false;

	FPS_ = new TextClass();
	if(!FPS_)
	{
		return false;
	}

	// Initialize the text object.
	if(!FPS_->setup(graphicsManager->getDevice(), graphicsManager->getDeviceContext(), graphicsManager->getShader2D(), screenWidth_, screenHeight_, 20, 20, "FPS: "))
	{
		MessageBoxA(NULL, "Could not initialize the FPS text object.", "GUIFrame - Error", MB_OK);
		return false;
	}

	kinectHandText_ = new TextClass();
	if(!kinectHandText_)
	{
		return false;
	}

	// Initialize the text object.
	if(!kinectHandText_->setup(graphicsManager->getDevice(), graphicsManager->getDeviceContext(), graphicsManager->getShader2D(), screenWidth_, screenHeight_, 20, 20, "Kinect: "))
	{
		std::string textToDisplay = "Could not initialize the frame text object for Kinect text.";
		MessageBoxA(NULL, textToDisplay.c_str(), "GUIFrame - Error", MB_OK);
		return false;
	}

	setupGUI(graphicsManager, inputManager, kinectManager);

	// Setup clock at the end so it starts when we run
	gameClock_ = new ClockClass();
	if(!gameClock_)
	{
		return false;
	}
	gameClock_->reset();

	savePositionsClock_ = new ClockClass();
	if(!savePositionsClock_)
	{
		return false;
	}
	savePositionsClock_->reset();

	timeToSavePositions_ = 1.0f;

	kinectManager->addListener(*this);
	inputManager->addListener(*this);

	soundManager_->playForest();

	activatedSignaledFruit_ = false;

	return true;
}

void FirstScreenState::update(float elapsedTime)
{	
	gameClock_->tick();
	savePositionsClock_->tick();
	
	LogClass::Instance()->update();

	firstAppleCollisionTest_->setPosition(Point(0.0f, 0.0f, 0.0f));

	// Update fruits logic
	if(subLevelState_ == PLAYING)
	{
		std::vector<FruitClass*>::iterator fruitIt;
		for(fruitIt = fruitsInGame_.begin(); fruitIt != fruitsInGame_.end(); fruitIt++)
		{
			(*fruitIt)->update(elapsedTime);
		}
	
		pico_->update(elapsedTime);

		if(pico_->isPointing() && !activatedSignaledFruit_)
		{
			switch(pico_->getPointedFruit())
			{
				case 1:
					{
						fruitsInGame_.at(0)->activateAlert(true);
						LogClass::Instance()->addEntry("Fruit_Visual_Aid", levelState_, 0);
						activatedSignaledFruit_ = true;
					}
					break;
				case 2:
					{
						fruitsInGame_.at(1)->activateAlert(true);
						LogClass::Instance()->addEntry("Fruit_Visual_Aid", levelState_, 1);
						activatedSignaledFruit_ = true;
					}
					break;
				case 3:
					{
						fruitsInGame_.at(2)->activateAlert(true);
						LogClass::Instance()->addEntry("Fruit_Visual_Aid", levelState_, 2);
						activatedSignaledFruit_ = true;
					}
					break;
				case 4:
					{
						fruitsInGame_.at(3)->activateAlert(true);
						LogClass::Instance()->addEntry("Fruit_Visual_Aid", levelState_, 3);
						activatedSignaledFruit_ = true;
					}
					break;
				default:
					{

					}
					break;
			}
		}
	}

	switch(levelState_)
	{
		case INTRODUCTION:
			{
				spaceShip_->update(elapsedTime);

				if(!spaceshipSoundPlayed_)
				{
					if(gameClock_->getTime() > spaceshipSoundTime_)
					{
						soundManager_->playSpaceshipFalling();
						spaceshipSoundPlayed_ = true;
					}
				}

				if(gameClock_->getTime() > introductionTime_)
				{
					changeLevel(FIRST_LEVEL);
				}
			}
			break;
		case FIRST_LEVEL:
		case SECOND_LEVEL:
			{
				updateLevel();
			}
			break;
		case THIRD_LEVEL:
		case FOURTH_LEVEL:
			{
				updateLevel();

				bird_->update(elapsedTime);
			}
			break;
		case ENDING:
			{
				if(pico_->getPosition().x < -6.0f && !firstLeave_)
				{
					pico_->goToPosition(Point(-0.0f, 0.0f, -3.0f));
					firstLeave_ = true;
				}
				if(pico_->isWaiting() && firstLeave_)
				{
					pico_->makeGoodbye();
				}
			}
			break;
		default:
			{

			}
			break;
	}

	if(subLevelState_ == SELECT_POLAROID)
	{
		if(gameClock_->getTime() > 5)
		{
			polaroidGUI_->update(elapsedTime);
		}
	}

	/*if(savePositionsClock_->getTime() > timeToSavePositions_)
	{
		LogClass::Instance()->addEntry("Pico_Position", picoScreenPos_.x, picoScreenPos_.y);
		LogClass::Instance()->addEntry("User_Position", kinectTorsoPos_.x, kinectTorsoPos_.y);
		savePositionsClock_->reset();
	}*/

	std::stringstream FPSText;
	FPSText << "FPS: " << 1/elapsedTime;
	FPS_->setText(FPSText.str(), graphicsManager_->getDeviceContext());
}

void FirstScreenState::draw()
{
	XMFLOAT4X4 viewMatrix;
	camera_->getViewMatrix(viewMatrix);

	XMFLOAT4X4 projectionMatrix, worldMatrix, orthoMatrix;
	graphicsManager_->getWorldMatrix(worldMatrix);
	graphicsManager_->getProjectionMatrix(projectionMatrix);
	graphicsManager_->getOrthoMatrix(orthoMatrix);

	XMFLOAT4 picoPos;
	Point picoPoint = pico_->getPosition();
	picoPos.x = picoPoint.x;
	picoPos.y = picoPoint.y;
	picoPos.z = picoPoint.z;
	picoPos.w = 1.0f;

	XMFLOAT4 resultPos;
	XMStoreFloat4(&resultPos, XMVector4Transform(XMLoadFloat4(&picoPos), XMLoadFloat4x4(&worldMatrix)));
	XMStoreFloat4(&resultPos, XMVector4Transform(XMLoadFloat4(&resultPos), XMLoadFloat4x4(&viewMatrix)));
	XMStoreFloat4(&resultPos, XMVector4Transform(XMLoadFloat4(&resultPos), XMLoadFloat4x4(&projectionMatrix)));

	kinectTorsoPos_.x = resultPos.x;
	kinectTorsoPos_.y = resultPos.y;

	graphicsManager_->turnZBufferOff();
		background_->draw(graphicsManager_->getDeviceContext(), backgrounPosition_.x, backgrounPosition_.y, worldMatrix, viewMatrix, orthoMatrix, light_->getDiffuseColor());
	graphicsManager_->turnZBufferOn();

	// We iterate over each loaded Object to call its draw function and draw the scenario
	std::vector<Object3D*>::iterator objectsIt;
	for(objectsIt = scenario_.begin(); objectsIt != scenario_.end(); objectsIt++)
	{
		(*objectsIt)->draw(graphicsManager_->getDevice() ,graphicsManager_->getDeviceContext(), worldMatrix, viewMatrix, projectionMatrix, light_);
	}

	if(levelState_ == INTRODUCTION)
	{
		spaceShip_->draw(graphicsManager_, worldMatrix, viewMatrix, projectionMatrix, light_, debug_);
	}

	// Draw the loaded fruits in game
	std::vector<FruitClass*>::iterator fruitIt;
	for(fruitIt = fruitsInGame_.begin(); fruitIt != fruitsInGame_.end(); fruitIt++)
	{
		(*fruitIt)->draw(graphicsManager_, worldMatrix, viewMatrix, projectionMatrix, light_, debug_);
	}

	pico_->draw(graphicsManager_, worldMatrix, viewMatrix, projectionMatrix, light_, debug_);
	
	bird_->draw(graphicsManager_, worldMatrix, viewMatrix, projectionMatrix, light_, debug_);

	if(subLevelState_ == SELECT_POLAROID)
	{
		graphicsManager_->turnZBufferOff();
		graphicsManager_->turnOnAlphaBlending();
			polaroidGUI_->draw(graphicsManager_->getDeviceContext(), worldMatrix, viewMatrix, orthoMatrix);
		graphicsManager_->turnOffAlphaBlending();
		graphicsManager_->turnZBufferOn();
	}

	if(debug_)
	{
		graphicsManager_->turnZBufferOff();
		graphicsManager_->turnOnAlphaBlending();
			FPS_->draw(graphicsManager_->getDeviceContext(), worldMatrix, viewMatrix, orthoMatrix);
		graphicsManager_->turnOffAlphaBlending();
		graphicsManager_->turnZBufferOn();

		graphicsManager_->turnOnWireframeRasterizer();
			firstAppleCollisionTest_->draw(graphicsManager_->getDevice(), graphicsManager_->getDeviceContext(), worldMatrix, viewMatrix, projectionMatrix, light_);
		graphicsManager_->turnOnSolidRasterizer();
	}
}

void FirstScreenState::destroy()
{
	LogClass::Instance()->setEndTime();

	// We delete the bird
	if(bird_)
	{
		bird_->destroy();
		delete bird_;
		bird_ = 0;
	}

	// WE KILL PICO MUAHAHA
	if(pico_)
	{
		pico_->destroy();
		delete pico_;
		pico_ = 0;
	}

	// Release the background image
	if(background_)
	{
		background_->destroy();
		delete background_;
		background_ = 0;
	}

	std::vector<Object3D*>::iterator objectsIt;
	for(objectsIt = scenario_.begin(); objectsIt != scenario_.end(); objectsIt++)
	{
		(*objectsIt)->destroy();
	}

	std::vector<FruitClass*>::iterator fruitIt;
	for(fruitIt = fruitsInGame_.begin(); fruitIt != fruitsInGame_.end(); fruitIt++)
	{
		(*fruitIt)->destroy();
	}
}

void FirstScreenState::notify(InputManager* notifier, InputStruct arg)
{
	switch(arg.keyPressed){
		case 49:
		case 50:
		case 51:
		case 52:
			{
				if(subLevelState_ == PLAYING)
				{
					if(arg.keyPressed-49 < fruitsInGame_.size())
					{
						LogClass::Instance()->addEntry("Fruit_Keyboard", 0, 0);
						fruitsInGame_.at(arg.keyPressed-49)->makeItFall();
					}
				}
				if(subLevelState_ == SELECT_POLAROID)
				{
					if(arg.keyPressed-49 < polaroidFrame_->getNumberButtons())
					{
						polaroidFrame_->selectButton(polaroidsName_.at(arg.keyPressed-49));
					}
				}
			}
			break;
		case 68: //D
		case 100: //d
			{
				debug_ = !debug_;
			}
			break;
		case 80: //P
		case 112: //p
			{
				subLevelState_ = FADING;
				soundManager_->playChangeLevel();
				gameClock_->reset();
				LogClass::Instance()->addEntry("Polaroids_Forced", levelState_, 0);
			}
			break;
		case 83: //S
		case 115: //s
			{
				if(levelState_ == ENDING)
				{
					LogClass::Instance()->addEntry("User_Bye", levelState_, 0);
					pico_->makeLeave();
				}
				else
				{
					pico_->sayHello();
				}
			}
			break;
		case 13: // Enter
			{
				bird_->setStealFood(!bird_->getStealFood());
				if(bird_->getStealFood())
				{
					LogClass::Instance()->addEntry("Activated_Bird", levelState_, 0);
				}
				else
				{
					LogClass::Instance()->addEntry("Desactivated_Bird", levelState_, 0);
				}
			}
			break;
		case 32: // Space
			{
				bird_->scared(true, false);
				LogClass::Instance()->addEntry("Bird_Scared_Keyboard", levelState_, 0);
			}
			break;
		default:
			{
				
			}
			break;
	}

	switch(arg.mouseButton)
	{
		// Check if the left mouse is pressed to interested objects
		case LEFT_BUTTON:
			{
				bool touchedFruits = false;
				std::vector<FruitClass*>::iterator fruitIt;
				for(fruitIt = fruitsInGame_.begin(); fruitIt != fruitsInGame_.end(); fruitIt++)
				{
					if((*fruitIt)->getCollisionSphere()->testIntersection(camera_, arg.mouseInfo.x, arg.mouseInfo.y))
					{
						LogClass::Instance()->addEntry("Fruit_Mouse_Clicked", 0, 0);
						(*fruitIt)->makeItFall();
						touchedFruits = true;
					}
				}

				if(touchedFruits)
				{
					for(fruitIt = fruitsInGame_.begin(); fruitIt != fruitsInGame_.end(); fruitIt++)
					{
						(*fruitIt)->activateAlert(false);
						activatedSignaledFruit_ = false;
					}
				}

				if(subLevelState_ == PLAYING && pico_->getCollisionSphere()->testIntersection(camera_, arg.mouseInfo.x, arg.mouseInfo.y))
				{
					pico_->makeHappy();
				}
				if(!firstFallen_ && levelState_ == FIRST_LEVEL && subLevelState_ == PLAYING && firstAppleCollisionTest_->testIntersection(camera_, arg.mouseInfo.x, arg.mouseInfo.y))
				{
					if(fruitsInGame_.at(1)->makeItFall())
					{
						firstFallen_ = true;
					}
				}
			}
			break;
		default:
			{
				
			}
			break;
	}
}

void FirstScreenState::notify(GUIButton* notifier, ButtonStruct arg)
{
	if(subLevelState_ == SELECT_POLAROID)
	{
		switch(arg.buttonPurpose)
		{
			case(LOAD_OBJECT):
				{
					soundManager_->playSelection();

					switch(levelState_)
					{
						case FIRST_LEVEL:
							{
								std::string name = arg.buttonInfo;

								std::vector<FruitClass*>::iterator it;
								for(it = fruitsInGame_.begin(); it != fruitsInGame_.end(); it++)
								{
									if((*it)->getName() == name)
									{
										pico_->setTipsColor((*it)->getColorEffect());
									}
								}

								changeLevel(SECOND_LEVEL);
							}
							break;
						case SECOND_LEVEL:
							{
								std::string name = arg.buttonInfo;

								std::vector<FruitClass*>::iterator it;
								for(it = fruitsInGame_.begin(); it != fruitsInGame_.end(); it++)
								{
									if((*it)->getName() == name)
									{
										pico_->setBodyTexture((*it)->getTextureEffect());
										break;
									}
								}

								bird_->setStealFood(true);
								changeLevel(THIRD_LEVEL);
							}
							break;
						case THIRD_LEVEL:
							{
								std::string name = arg.buttonInfo;

								std::vector<FruitClass*>::iterator it;
								for(it = fruitsInGame_.begin(); it != fruitsInGame_.end(); it++)
								{
									if((*it)->getName() == name)
									{
										pico_->setHat((*it)->getHatEffect());
										break;
									}
								}

								changeLevel(FOURTH_LEVEL);
							}
							break;
						case FOURTH_LEVEL:
							{
								std::string name = arg.buttonInfo;

								std::vector<FruitClass*>::iterator it;
								for(it = fruitsInGame_.begin(); it != fruitsInGame_.end(); it++)
								{
									if((*it)->getName() == name)
									{
										pico_->setBody((*it)->getBodyEffect());
										pico_->setTips((*it)->getTipsEffect());
										break;
									}
								}

								changeLevel(ENDING);
								pico_->makeGoodbye();
							}
							break;
					}

					if(levelState_ == SECOND_LEVEL)
					{
						pico_->makePointing();
						pico_->setToRest(false);
					}
					else
					{
						pico_->setToRest(true);
					}
				}
				break;
			default:
				{
				}
				break;
		}
	}
}

void FirstScreenState::notify(KinectClass* notifier, KinectStruct arg)
{
	switch(arg.type)
	{
		case(FIRST_RIGHT_HAND_ROT):
		case(FIRST_LEFT_HAND_ROT):
			{
				//kinectHandPos_ = Point(arg.handPos.x*screenWidth_/320, arg.handPos.y*screenHeight_/240);
				kinectHandPos_ = Point(arg.handPos.x, arg.handPos.y);

				std::stringstream kinectext;
				kinectext << "Kinect: " << kinectHandPos_.x << "x" << kinectHandPos_.y;
				kinectHandText_->setText(kinectext.str(), graphicsManager_->getDeviceContext());

				bool touchedFruits = false;
				int touchedFruit = 0;
				std::vector<FruitClass*>::iterator fruitIt;
				for(fruitIt = fruits_.begin(); fruitIt != fruits_.end(); fruitIt++)
				{
					if((*fruitIt)->getCollisionSphere()->testIntersection(camera_, kinectHandPos_.x, kinectHandPos_.y))
					{
						if(!bird_->getIsTeasing())
						{
							(*fruitIt)->shakeIt();
							soundManager_->playLeaves();
						}
						touchedFruits = true;
					}
					touchedFruit++;
				}

				if(touchedFruits)
				{
					for(fruitIt = fruitsInGame_.begin(); fruitIt != fruitsInGame_.end(); fruitIt++)
					{
						(*fruitIt)->activateAlert(false);
						activatedSignaledFruit_ = false;
					}
				}


				if(arg.type == FIRST_RIGHT_HAND_ROT)
				{
					if(subLevelState_ == PLAYING && pico_->getCollisionSphere()->testIntersection(camera_, kinectHandPos_.x, kinectHandPos_.y))
					{
						if(!picoHeadTouchedRight_)
						{
							LogClass::Instance()->addEntry("Pico_Head", levelState_, 0);
							pico_->makeHappy();
						}
						picoHeadTouchedRight_ = true;
					}
					else
					{
						picoHeadTouchedRight_ = false;
					}
				}
				else
				{
					if(subLevelState_ == PLAYING && pico_->getCollisionSphere()->testIntersection(camera_, kinectHandPos_.x, kinectHandPos_.y))
					{
						if(!picoHeadTouchedLeft_)
						{
							LogClass::Instance()->addEntry("Pico_Head", levelState_, 0);
							pico_->makeHappy();
						}
						picoHeadTouchedLeft_ = true;
					}
					else
					{
						picoHeadTouchedLeft_ = false;
					}
				}

				if(bird_->getCollisionSphere()->testIntersection(camera_, kinectHandPos_.x, kinectHandPos_.y))
				{
					bird_->scared(false, true);
				}

				if(!firstFallen_ && subLevelState_ == PLAYING && levelState_ == FIRST_LEVEL && firstAppleCollisionTest_->testIntersection(camera_, kinectHandPos_.x, kinectHandPos_.y))
				{
					if(fruitsInGame_.at(1)->makeItFall())
					{
						firstFallen_ = true;
					}
				}
			}
			break;
		case(GREETINGS_KINECT):
			{
				if(subLevelState_ == PLAYING)
				{
					/*bool touchedFruits = false;
					std::vector<FruitClass*>::iterator fruitIt;
					for(fruitIt = fruits_.begin(); fruitIt != fruits_.end(); fruitIt++)
					{
						if((*fruitIt)->getCollisionSphere()->testIntersection(camera_, kinectHandPos_.x, kinectHandPos_.y))
						{
							touchedFruits = true;
						}
					}

					if(!touchedFruits)
					{
						LogClass::Instance()->addEntry("User_Hi", levelState_, 0);
						pico_->sayHello();
					}*/
				}
				if(levelState_ == ENDING)
				{
					/*LogClass::Instance()->addEntry("User_Bye", levelState_, 0);
					pico_->makeLeave();*/
				}
			}
		default:
			{

			}
			break;
	}
}

void FirstScreenState::notify(FruitClass* notifier, Point arg)
{
	std::vector<FruitClass*>::iterator fruitIt;
	for(fruitIt = fruitsInGame_.begin(); fruitIt != fruitsInGame_.end(); fruitIt++)
	{
		if((*fruitIt)->getName() != notifier->getName())
		{
			(*fruitIt)->resetSpawningTime();
		}
	}
}

void FirstScreenState::updateLevel()
{
	switch(subLevelState_)
	{
		case PLAYING:
			{
				if(gameClock_->getTime() > playingTime_)
				{
					// check if Pico is idle
					subLevelState_ = FADING;
					soundManager_->playChangeLevel();
					gameClock_->reset();
				}
			}
			break;
		case FADING:
			{
				float diffuseTemp = 1.0f - gameClock_->getTime()/fadeTime_;
				light_->setDiffuseColor(diffuseTemp, diffuseTemp, diffuseTemp, 1.0f);

				float ambientTemp = 0.1f - (gameClock_->getTime()/fadeTime_)*0.075f;
				light_->setAmbientColor(ambientTemp, ambientTemp, ambientTemp, 1.0f);

				if(gameClock_->getTime() > fadeTime_)
				{
					clearPolaroids();
					createPolaroids();
					subLevelState_ = SELECT_POLAROID;
					gameClock_->reset();
				}
			}
			break;
		case SELECT_POLAROID:
			{
					soundManager_->playChangeLevel();
			}
			break;
		default:
			{

			}
			break;
	}
}

void FirstScreenState::setupGUI(GraphicsManager* graphicsManager, InputManager* inputManager, KinectClass* kinectManager)
{
	polaroidGUI_ = new GUIManager;

	if(!polaroidGUI_->setup(graphicsManager))
	{
		MessageBoxA(NULL, "Could not create Polaroid GUI", "FirstScreen - Error", MB_ICONERROR | MB_OK);
	}

	int screenWidth, screenHeight;
	graphicsManager->getScreenSize(screenWidth, screenHeight);

	polaroidFrame_ = new GUIFrame;
	polaroidFrame_->setup(graphicsManager, "Polaroids", Point(0, 0), screenWidth, screenHeight);
	polaroidGUI_->addFrame(polaroidFrame_);

	inputManager->addListener(*polaroidGUI_);
	kinectManager->addListener(*polaroidGUI_);
}

void FirstScreenState::loadConfigurationFromXML()
{
	std::string root = "./Data/configuration/level1/main_configuration.xml";

	//Loading animations XML file
	pugi::xml_document configurationDoc;
	if (!configurationDoc.load_file(root.c_str()))
	{
		MessageBoxA(NULL, "Could not load configuration .xml file!", "FirstScreen - Error", MB_ICONERROR | MB_OK);
	}

	//Searching for the initial node
	pugi::xml_node rootNode;
	if(!(rootNode = configurationDoc.child("configuration")))
	{
		MessageBoxA(NULL, "Could not load configuration node!", "FirstScreen - Error", MB_ICONERROR | MB_OK);
	}

	pugi::xml_node introductionNode;
	if(!(introductionNode = rootNode.child("intro_time")))
	{
		MessageBoxA(NULL, "Could not load intorduction time node!", "FirstScreen - Error", MB_ICONERROR | MB_OK);
		introductionTime_ = 25;
	}
	else
	{
		introductionTime_ = introductionNode.text().as_float();
	}

	pugi::xml_node playTimeNode;
	if(!(playTimeNode = rootNode.child("play_time")))
	{
		MessageBoxA(NULL, "Could not load play time node!", "FirstScreen - Error", MB_ICONERROR | MB_OK);
		playingTime_ = 15;
	}
	else
	{
		playingTime_ = playTimeNode.text().as_float();
	}

	pugi::xml_node fadeTimeNode;
	if(!(fadeTimeNode = rootNode.child("fade_time")))
	{
		MessageBoxA(NULL, "Could not load fade time node!", "FirstScreen - Error", MB_ICONERROR | MB_OK);
		fadeTime_ = 7;
	}
	else
	{
		fadeTime_ = fadeTimeNode.text().as_float();
	}
}

void FirstScreenState::loadScenario(std::string scenario)
{
	HANDLE dir;
    WIN32_FIND_DATAA file_data;

	std::string scenarioToLoad = "./Data/configuration/"+scenario+"/scenario/*";

	// If we can access to that sctructure, then create a loadModel button for each found model
    if ((dir = FindFirstFileA(scenarioToLoad.c_str(), &file_data)) != INVALID_HANDLE_VALUE)
	{
		do {
    		const std::string file_name = file_data.cFileName;
    		const bool is_directory = (file_data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0;

    		if (file_name[0] == '.')
    			continue;

    		if (is_directory)
    			continue;

			const std::string s( file_name.begin(), file_name.end() );

			createScenarioObject(scenario, s);

		} while (FindNextFileA(dir, &file_data));
	}
}

void FirstScreenState::createScenarioObject(std::string scenario, std::string xmlName)
{
	Object3D* objectLoadedTemp = NULL;

	std::string root = "./Data/configuration/"+ scenario + "/scenario/" + xmlName;

	//Loading animations XML file
	pugi::xml_document objectDoc;
	if (!objectDoc.load_file(root.c_str()))
	{
		MessageBoxA(NULL, "Could not load object .xml file!", "FirstScreen - Error", MB_ICONERROR | MB_OK);
	}

	//Searching for the initial node
	pugi::xml_node rootNode;
	if(!(rootNode = objectDoc.child(xmlName.substr(0, xmlName.size()-4).c_str())))
	{
		MessageBoxA(NULL, "Invalid .xml file! Could not find base node (must have object name).", "Visualizer - Error", MB_ICONERROR | MB_OK);
	}

	pugi::xml_node modelNode;
	modelNode = rootNode.child("model");

	pugi::xml_text modelName = modelNode.text();
	objectLoadedTemp = Object3DFactory::Instance()->CreateObject3D("StaticObject3D", graphicsManager_, modelName.as_string());

	if(objectLoadedTemp)
	{
		// Parse transformation data
		pugi::xml_node positionNode;
		positionNode = rootNode.child("position");
		objectLoadedTemp->setPosition(Point(positionNode.attribute("x").as_float(), positionNode.attribute("y").as_float(), positionNode.attribute("z").as_float()));

		pugi::xml_node scaleNode;
		scaleNode = rootNode.child("scale");
		objectLoadedTemp->setScale(Vector(scaleNode.attribute("x").as_float(), scaleNode.attribute("y").as_float(), scaleNode.attribute("z").as_float()));

		pugi::xml_node rotationNode;
		rotationNode = rootNode.child("rotation");
		objectLoadedTemp->setRotationX(rotationNode.attribute("x").as_float());
		objectLoadedTemp->setRotationY(rotationNode.attribute("y").as_float());
		objectLoadedTemp->setRotationZ(rotationNode.attribute("z").as_float());

		scenario_.push_back(objectLoadedTemp);
	}
	else
	{
		MessageBoxA(NULL, "Could not load the selected model.", "Visualizer - Error", MB_ICONERROR | MB_OK);
	}
}

bool FirstScreenState::loadFruits()
{
	for(int i = 1; i < 5; i++)
	{
		std::stringstream root;
		root << "./Data/configuration/level1/fruits/fruits_" << i << ".xml";

		//Loading animations XML file
		pugi::xml_document fruitsDoc;
		if(!fruitsDoc.load_file(root.str().c_str()))
		{
			std::stringstream error;
			error << "Could not load object .xml file!" << root.str();
			MessageBoxA(NULL, error.str().c_str(), "FirstScreen - Fruit - Error", MB_ICONERROR | MB_OK);
			continue;
		}

		pugi::xml_node root_node;
		// Get initial node
		if(!(root_node = fruitsDoc.child("fruits")))
		{
			MessageBoxA(NULL, "Could not find the fruits root node.", "FirstScreen - Fruit - Error", MB_ICONERROR | MB_OK);
			continue;
		}

		for(pugi::xml_node fruitNode = root_node.first_child(); fruitNode; fruitNode = fruitNode.next_sibling())
		{
			std::string node_name = fruitNode.name();
			// Actuamos en consecuencia segun el tipo de nodo
			if(node_name ==  "fruit")
			{
				pugi::xml_node modelNode;
				modelNode = fruitNode.child("model");

				pugi::xml_text modelName = modelNode.text();

				// Parse transformation data
				pugi::xml_node positionNode;
				positionNode = fruitNode.child("position");
				Point pos = Point(positionNode.attribute("x").as_float(), positionNode.attribute("y").as_float(), positionNode.attribute("z").as_float());

				pugi::xml_node scaleNode;
				scaleNode = fruitNode.child("scale");
				Vector scale = Vector(scaleNode.attribute("x").as_float(), scaleNode.attribute("y").as_float(), scaleNode.attribute("z").as_float());

				pugi::xml_node rotationNode;
				rotationNode = fruitNode.child("rotation");
				float rotX = rotationNode.attribute("x").as_float();
				float rotY = rotationNode.attribute("y").as_float();
				float rotZ = rotationNode.attribute("z").as_float();

				FruitClass* fruit = new FruitClass();
				if(!fruit)
				{
					return false;
				}

				if(!fruit->setup(graphicsManager_, soundManager_, modelName.as_string(), pos, terrainHeight_, scale, rotX, rotY, rotZ))
				{
					MessageBoxA(NULL, "Could not initialize fruit.", "Visualizer - Fruit - Error", MB_ICONERROR | MB_OK);
					return false;
				}

				pugi::xml_node effectNode = fruitNode.child("effect");
				std::string effectType = effectNode.attribute("type").as_string();

				if(effectType == "color")
				{
					pugi::xml_node colorNode = effectNode.child("color");
					XMFLOAT4 color = XMFLOAT4(colorNode.attribute("r").as_float(), colorNode.attribute("g").as_float(), colorNode.attribute("b").as_float(), 1.0f);

					fruit->setFruitEffectType(COLOR);
					fruit->setColorEffect(color);
				}

				if(effectType == "texture")
				{
					pugi::xml_node textureNode = effectNode.child("texture");
					TextureClass* temp = new TextureClass;
					if(!temp)
					{
						MessageBoxA(NULL, "Could not create texture!", "Visualizer - Fruit - Error", MB_ICONERROR | MB_OK);
						continue;
					}
					std::string textureName = textureNode.text().as_string();
					std::string filePath = "./Data/models/miniBossCuerpo/" + textureName + ".dds";
					bool result = temp->setup(graphicsManager_->getDevice(), filePath);
					if(!result)
					{
						MessageBoxA(NULL, "Could not load texture!", "Visualizer - Fruit - Error", MB_ICONERROR | MB_OK);
						continue;
					}

					fruit->setFruitEffectType(TEXTURE);
					fruit->setTextureEffect(temp);
				}

				if(effectType == "hat")
				{
					pugi::xml_node modelNode = effectNode.child("model");

					std::string modelName = modelNode.text().as_string();

					Object3D* temp = 0;
					temp = Object3DFactory::Instance()->CreateObject3D("AnimatedObject3D", graphicsManager_, modelName);
					if(!temp)
					{
						MessageBoxA(NULL, "Could not load model!", "Visualizer - Fruit - Error", MB_ICONERROR | MB_OK);
						continue;
					}

					fruit->setFruitEffectType(HAT);
					fruit->setHatEffect(temp);
				}

				if(effectType == "body")
				{
					pugi::xml_node modelNode = effectNode.child("body");

					std::string modelName = modelNode.text().as_string();

					Object3D* tempbody = 0;
					tempbody = Object3DFactory::Instance()->CreateObject3D("AnimatedObject3D", graphicsManager_, modelName);
					if(!tempbody)
					{
						MessageBoxA(NULL, "Could not load model!", "Visualizer - Fruit - Error", MB_ICONERROR | MB_OK);
						continue;
					}

					modelNode = effectNode.child("tips");

					modelName = modelNode.text().as_string();

					Object3D* temptips = 0;
					temptips = Object3DFactory::Instance()->CreateObject3D("AnimatedObject3D", graphicsManager_, modelName);
					if(!temptips)
					{
						MessageBoxA(NULL, "Could not load model!", "Visualizer - Fruit - Error", MB_ICONERROR | MB_OK);
						continue;
					}

					fruit->setFruitEffectType(BODY);
					fruit->setBodyEffect(tempbody, temptips);
				}

				pugi::xml_node collisionNode = fruitNode.child("collision");

				positionNode = collisionNode.child("position");
				pos = Point(positionNode.attribute("x").as_float(), positionNode.attribute("y").as_float()+0.4, positionNode.attribute("z").as_float());

				fruit->getCollisionSphere()->setRelativePosition(pos);

				pugi::xml_node radiusNode;
				radiusNode = collisionNode.child("radius");
				float radius = radiusNode.text().as_float();

				fruit->getCollisionSphere()->setRadius(radius);

				fruits_.push_back(fruit);
			}
		}
	}

	return true;
}

void FirstScreenState::addFruitsToGame()
{
	int logicId = 1;
	if(levelState_ < 4)
	{
		for(int i = (levelState_-1)*4; i < (levelState_-1)*4+4; i++)
		{
			// This ID will be used for Pico Logic
			fruits_.at(i)->setLogicID(logicId);
			fruits_.at(i)->setLevel(levelState_);

			// When we set at the game the first apple
			if(levelState_ == FIRST_LEVEL && logicId == 2)
			{
				fruits_.at(i)->setPosition(Point(-1.25f, 2.5, -3.0));
			}

			fruitsInGame_.push_back(fruits_.at(i));
			fruits_.at(i)->addListener(*pico_);
			fruits_.at(i)->addListener(*bird_);
			fruits_.at(i)->addListener(*this);

			logicId++;
		}
	}
	else
	{
		for(int i = (levelState_-1)*4; i < (levelState_-1)*4+3; i++)
		{
			// This ID will be used for Pico Logic
			fruits_.at(i)->setLogicID(logicId);
			fruits_.at(i)->setLevel(levelState_);

			fruitsInGame_.push_back(fruits_.at(i));
			fruits_.at(i)->addListener(*pico_);
			fruits_.at(i)->addListener(*bird_);

			logicId++;
		}
	}
}

void FirstScreenState::clearFruits()
{
	// First we clean the fruits vector just in case
	std::vector<FruitClass*>::iterator fruitIt;
	for(fruitIt = fruits_.begin(); fruitIt != fruits_.end(); fruitIt++)
	{
		(*fruitIt)->destroy();
	}
	fruits_.clear();
}

void FirstScreenState::clearFruitsInGame()
{
	std::vector<FruitClass*>::iterator fruitIt;
	for(fruitIt = fruitsInGame_.begin(); fruitIt != fruitsInGame_.end(); fruitIt++)
	{
		(*fruitIt)->removeListener(*pico_);
	}
	fruitsInGame_.clear();
}

bool FirstScreenState::createPolaroids()
{
	polaroidsName_.clear();

	std::stringstream root;
	root << "./Data/configuration/level1/polaroids/polaroids_" << levelState_ << ".xml";

	//Loading animations XML file
	pugi::xml_document polaroidsDoc;
	if(!polaroidsDoc.load_file(root.str().c_str()))
	{
		MessageBoxA(NULL, "Could not load polaroid .xml file!", "FirstScreen - Error", MB_ICONERROR | MB_OK);
		return false;
	}

	pugi::xml_node root_node;
	// Get initial node
	if(!(root_node = polaroidsDoc.child("polaroids")))
	{
		MessageBoxA(NULL, "Could not find the polaroids root node.", "FirstScreen - Error", MB_ICONERROR | MB_OK);
		return false;
	}

	// We first check if any has fallen, if none has been thrown, then we display all
	bool noneHasFallen = true;
	/*std::vector<FruitClass*>::iterator it;
	for(it = fruitsInGame_.begin(); it != fruitsInGame_.end(); it++)
	{
		if((*it)->hasFallen())
		{
			noneHasFallen = false;
		}
	}*/

	int screenWidth, screenHeight;
	graphicsManager_->getScreenSize(screenWidth, screenHeight);

	int fruitIndex = 0;
	for(pugi::xml_node polaroidNode = root_node.first_child(); polaroidNode; polaroidNode = polaroidNode.next_sibling())
	{
		std::string node_name = polaroidNode.name();
		// Actuamos en consecuencia segun el tipo de nodo
		if(node_name ==  "polaroid")
		{
			if(fruitsInGame_.at(fruitIndex)->hasFallen() || noneHasFallen)
			{
				pugi::xml_node imageNode;
				imageNode = polaroidNode.child("image");

				pugi::xml_text imageName = imageNode.text();

				// Parse transformation data
				pugi::xml_node positionNode;
				Point pos;
				if(!(positionNode = polaroidNode.child("position")))
				{
					MessageBoxA(NULL, "Could not find the polaroids position.", "FirstScreen - Error", MB_ICONERROR | MB_OK);
					pos = Point(0, 0);
				}
				else{
					pos = Point(positionNode.attribute("x").as_float()*screenWidth, positionNode.attribute("y").as_float()*screenHeight);
				}

				pugi::xml_node sizeNode;
				Point size;
				if(!(sizeNode = polaroidNode.child("size")))
				{
					MessageBoxA(NULL, "Could not find the polaroids size.", "FirstScreen - Error", MB_ICONERROR | MB_OK);
					size = Point(0, 0);
				}
				else{
					size = Point(sizeNode.attribute("x").as_float()*screenWidth, sizeNode.attribute("y").as_float()*screenHeight);
				}

				polaroidFrame_->addButton(graphicsManager_, fruitsInGame_.at(fruitIndex)->getName(), pos, size, imageName.as_string())->addListener(*this);
				polaroidsName_.push_back(fruitsInGame_.at(fruitIndex)->getName());
			}
		}
		fruitIndex++;
	}

	return true;
}

void FirstScreenState::clearPolaroids()
{
	polaroidFrame_->deleteButtons();
}

void FirstScreenState::changeLevel(LevelState level)
{
	// Clear the fruits and polaroids vectors for next level
	clearFruitsInGame();

	pico_->setLevelState(level);

	// Reset light
	light_->setAmbientColor(0.1f, 0.1f, 0.1f, 1.0f);
	light_->setDiffuseColor(1.0f, 1.0f, 1.0f, 1.0f);

	// Set new level
	levelState_ = level;
	subLevelState_ = PLAYING;

	if(levelState_ != ENDING)
	{
		addFruitsToGame();
	}
	else
	{
		pico_->changeAnimation("DanceAss", 0.4f);
		pico_->changeExpression("feliz");
		soundManager_->playCelebratingFile();
	}

	// Reset clock
	gameClock_->reset();
}

bool FirstScreenState::setupSplashScreen()
{
	// load splashscreen and calculate its position
	splashScreen_ = new ImageClass;
	if(!splashScreen_)
	{
		MessageBoxA(NULL, "Could not initialize the splashscreen image instance.", "SecondScreen - Error", MB_OK);
		return false;
	}

	if(!splashScreen_->setup(graphicsManager_->getDevice(), graphicsManager_->getShader2D(), screenWidth_, screenHeight_, "splashscreen1", screenWidth_, screenHeight_))
	{
		MessageBoxA(NULL, "Could not setup the splashscreen image.", "SecondScreen - Error", MB_OK);
		return false;
	}

	return true;
}

void FirstScreenState::drawSplashScreen()
{
	XMFLOAT4X4 viewMatrix;
	camera_->getViewMatrix(viewMatrix);

	XMFLOAT4X4 projectionMatrix, worldMatrix, orthoMatrix;
	graphicsManager_->getWorldMatrix(worldMatrix);
	graphicsManager_->getProjectionMatrix(projectionMatrix);
	graphicsManager_->getOrthoMatrix(orthoMatrix);

	graphicsManager_->beginDraw(1.0f, 0.5f, 0.0f, 1.0f);
		graphicsManager_->turnZBufferOff();
			splashScreen_->draw(graphicsManager_->getDeviceContext(), (screenWidth_/2)*-1, (screenHeight_/2)+2, worldMatrix, viewMatrix, orthoMatrix, XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));
		graphicsManager_->turnZBufferOn();
	graphicsManager_->endDraw();
}