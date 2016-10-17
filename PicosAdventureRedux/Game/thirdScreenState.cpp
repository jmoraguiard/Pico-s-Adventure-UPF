#ifdef _WIN32
#include <windows.h>
#endif

#include <GL/gl.h>
#include <GL/glu.h>

#include "thirdScreenState.h"

ThirdScreenState ThirdScreenState::thirdScreenState_;

ThirdScreenState::ThirdScreenState()
{
	camera_ = 0;
	light_ = 0;
	gameClock_ = 0;

	pico_ = 0;

	spaceShip_ = 0;

	background_ = 0;

	kinectHandRot_[0] = 0.0f;
	kinectHandRot_[1] = 0.0f;

	for(int i = 0; i < 5; i++)
	{
		friendSpacheShips_[0];
	}
}

ThirdScreenState::~ThirdScreenState()
{

}

ThirdScreenState* ThirdScreenState::Instance()
{
	return (&thirdScreenState_);
}

bool ThirdScreenState::setup(ApplicationManager* appManager, GraphicsManager* graphicsManager, InputManager * inputManager, KinectClass* kinectManager)
{
	// We get a pointer to the graphicsManager
	graphicsManager_ = graphicsManager;

	graphicsManager_->getScreenSize(screenWidth_, screenHeight_);

	LogClass::Instance()->setup("third_level_log");

	// Subscribe to keyboard and mouse manager
	inputManager->addListener(*this);

	// Change kinect settings to make user being drawn darker
	kinectClass_ = kinectManager;

	kinectClass_->setUserColor(XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f));
	kinectClass_->addListener(*this);

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

	loadConfigurationFromXML();

	// Create the light object.
	light_ = new LightClass;
	if(!light_)
	{
		return false;
	}

	// Initialize the light object.
	light_->setAmbientColor(0.1f, 0.1f, 0.1f, 1.0f);
	light_->setDiffuseColor(0.8f, 0.8f, 0.8f, 1.0f);
	light_->setDirection(0.0f, -0.5f, 1.0f);

	// SOUND
	soundManager_ = new SoundThirdClass;
	if(!soundManager_)
	{
		return false;
	}
 
	// Initialize the sound object.
	if(!soundManager_->setup(graphicsManager_->getWindowHandler()))
	{
		MessageBoxA(NULL, "Could not initialize Direct Sound.", "ThirdScreen - Error", MB_OK);
		return false;
	}
	soundManager_->playFile("background_music", true);

	// Load the first level scenario
	loadScenario("level3");

	// Create Pico
	pico_ = new PicoThirdClass();
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
	
	if(!setupAllSpaceShips())
	{
		return false;
	}

	if(!setupRays())
	{
		return false;
	}
	if(!setupRayInfo())
	{
		return false;
	}
	distanceThreshold_ = 120;
	drawRays_ = false;
	activeRay_ = GREEN;
	bothTouching_ = false;

	lastOutLeft_ = false;
	subLevel4Freed_ = 0;

	picoDanced_ = false;

	spaceShipIntroWaitTime_ = 2.0f;
	introFallTime_ = 3.0f;
	introGreetTime_ = 3.0f;
	introSpaceTime_ = 3.0f;

	//soundManager_->playFile("background_music", true);

	// CELEBRATIONS
	for(int i = 0; i < 5; i++)
	{
		celebrationParticles_[i] = new ParticleSystem;
		if(!celebrationParticles_[i])
		{
			MessageBoxA(NULL, "Could not create particles instance", "SecondScreen - Error", MB_ICONERROR | MB_OK);
		}
		if(celebrationParticles_[i] && !celebrationParticles_[i]->setup(graphicsManager, "star", Point(-2.0f+(1.0f*i), 4.3f, -5.75f), 4.0f, 200, 200, XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), true))
		{
			MessageBoxA(NULL, "Could not setup particles object", "SecondScreen - Error", MB_ICONERROR | MB_OK);
		}
		celebrationParticles_[i]->setParticlesVelocity(Point(0.0f, 0.8f, 0.0f), Point(1.5f, 0.3f, 0.0f));
	}
	celebrationTime_ = 15.0f;

	// MEDALS
	//Load medals
	medals_[0] = Object3DFactory::Instance()->CreateObject3D("StaticObject3D", graphicsManager, "medalla_oro");
	medals_[0]->setRotationX(XM_PIDIV2);
	medals_[0]->setScale(Vector(0.01f, 0.01f, 0.01f));
	medals_[0]->setPosition(Point(0.0f, 3.0f, -6.0f));
	medals_[1] = Object3DFactory::Instance()->CreateObject3D("StaticObject3D", graphicsManager, "medalla_plata");
	medals_[1]->setRotationX(XM_PIDIV2);
	medals_[1]->setScale(Vector(0.01f, 0.01f, 0.01f));
	medals_[1]->setPosition(Point(0.0f, 3.0f, -6.0f));
	medals_[2] = Object3DFactory::Instance()->CreateObject3D("StaticObject3D", graphicsManager, "medalla_bronce");
	medals_[2]->setRotationX(XM_PIDIV2);
	medals_[2]->setScale(Vector(0.01f, 0.01f, 0.01f));
	medals_[2]->setPosition(Point(0.0f, 3.0f, -6.0f));
	fadeTime_ = 7.0f;
	actualLevelState_ = 0;

	if(!setupDebugOptions())
	{
		return false;
	}
	
	// Setup initial game state
	levelState_ = INTRODUCTION;
	introLevelState_ = ARRIBING;

	// Setup clock at the end so it starts when we run
	gameClock_ = new ClockClass();
	if(!gameClock_)
	{
		return false;
	}
	gameClock_->reset();

	introClock_ = new ClockClass();
	if(!introClock_)
	{
		return false;
	}
	introClock_->reset();

	playClock_ = new ClockClass();
	if(!playClock_)
	{
		return false;
	}
	playClock_->reset();

	pointingClock_ = new ClockClass();
	if(!pointingClock_)
	{
		return false;
	}
	pointingClock_->reset();
	isPointing_ = false;

	noClock_ = new ClockClass();
	if(!noClock_)
	{
		return false;
	}
	noClock_->reset();

	spaceShip_->goToPosition(Point(3.5f, 5.0f, -3.25f));

	return true;
}

void ThirdScreenState::update(float elapsedTime)
{	
	gameClock_->tick();

	LogClass::Instance()->update();

	switch(levelState_)
	{
		case INTRODUCTION:
			{
				updateIntroduction(elapsedTime);

				friendSpacheShips_[0]->update(elapsedTime);
			}
			break;
		case PLAYING:
			{
				updatePlaying(elapsedTime);

				friendSpacheShips_[1]->setRotationZ(0);
				friendSpacheShips_[3]->setRotationZ(0);

				for(int i = 0; i< 2; i++)
				{
					blueRay_[i]->update();
					greenRay_[i]->update();
					redRay_[i]->update();
				}
			}
			break;
		case ENDING:
			{
				pico_->update(elapsedTime);

				if(!picoDanced_ && gameClock_->getTime() > 5.0f)
				{
					pico_->makeRest(true);
					soundManager_->playBye();
					pico_->changeAnimation("hola", 0.5f);

					picoDanced_ = true;
					gameClock_->reset();
				}

				if(picoDanced_ && gameClock_->getTime() > 5.0f)
				{
					pico_->makeLeave();
				}
			}
			break;
		default:
			{
				
			}
			break;
	}

	// Update debug texts
	if(debug_)
	{
		std::stringstream FPSText;
		FPSText << "FPS: " << 1/elapsedTime;
		FPS_->setText(FPSText.str(), graphicsManager_->getDeviceContext());

		updateKinectHands();
		updateRayInfo();
	}
}

void ThirdScreenState::draw()
{
	XMFLOAT4X4 viewMatrix;
	camera_->getViewMatrix(viewMatrix);

	XMFLOAT4X4 projectionMatrix, worldMatrix, orthoMatrix;
	graphicsManager_->getWorldMatrix(worldMatrix);
	graphicsManager_->getProjectionMatrix(projectionMatrix);
	graphicsManager_->getOrthoMatrix(orthoMatrix);

	// Draw background
	graphicsManager_->turnZBufferOff();
		background_->draw(graphicsManager_->getDeviceContext(), backgrounPosition_.x, backgrounPosition_.y, worldMatrix, viewMatrix, orthoMatrix, light_->getDiffuseColor());
	graphicsManager_->turnZBufferOn();

	// We iterate over each loaded Object to call its draw function and draw the scenario
	std::vector<Object3D*>::iterator objectsIt;
	for(objectsIt = scenario_.begin(); objectsIt != scenario_.end(); objectsIt++)
	{
		(*objectsIt)->draw(graphicsManager_->getDevice() ,graphicsManager_->getDeviceContext(), worldMatrix, viewMatrix, projectionMatrix, light_);
	}

	switch(levelState_)
	{
		case INTRODUCTION:
			{
				spaceShip_->draw(graphicsManager_, worldMatrix, viewMatrix, projectionMatrix, light_, debug_);
				pico_->draw(graphicsManager_, worldMatrix, viewMatrix, projectionMatrix, light_, debug_);

				friendSpacheShips_[0]->draw(graphicsManager_, worldMatrix, viewMatrix, projectionMatrix, light_, debug_);
			}
			break;
		case PLAYING:
			{
				for(int i = 0; i < 5; i++)
				{
					friendSpacheShips_[i]->draw(graphicsManager_, worldMatrix, viewMatrix, projectionMatrix, light_, debug_);
				}

				pico_->draw(graphicsManager_, worldMatrix, viewMatrix, projectionMatrix, light_, debug_);

				graphicsManager_->turnOnParticlesAlphaBlending();
				graphicsManager_->turnZBufferOff();
					if(playLevelState_ == CELEBRATING_LEVEL_1 || playLevelState_ == CELEBRATING_LEVEL_2 ||
					   playLevelState_ == CELEBRATING_LEVEL_3 || playLevelState_ == CELEBRATING_LEVEL_4)
					{
						for(int i = 0; i < 5; i++)
						{
							celebrationParticles_[i]->draw(graphicsManager_->getDeviceContext(), worldMatrix, viewMatrix, projectionMatrix, light_);
						}
					}
				graphicsManager_->turnZBufferOn();
				graphicsManager_->turnOffAlphaBlending();

				if(drawRays_)
				{
					graphicsManager_->turnZBufferOff();
					graphicsManager_->turnOnAlphaBlending();

					switch(activeRay_)
					{
						case GREEN:
							{
								greenRay_[0]->draw(graphicsManager_, worldMatrix, viewMatrix, orthoMatrix, kinectHandViewPos_[0], kinectHandRot_[0], debug_);
								greenRay_[1]->draw(graphicsManager_, worldMatrix, viewMatrix, orthoMatrix, kinectHandViewPos_[1], kinectHandRot_[1], debug_);
							}
							break;
						case RED:
							{
								redRay_[0]->draw(graphicsManager_, worldMatrix, viewMatrix, orthoMatrix, kinectHandViewPos_[0], kinectHandRot_[0], debug_);
								redRay_[1]->draw(graphicsManager_, worldMatrix, viewMatrix, orthoMatrix, kinectHandViewPos_[1], kinectHandRot_[1], debug_);
							}
							break;
						case BLUE:
							{
								blueRay_[0]->draw(graphicsManager_, worldMatrix, viewMatrix, orthoMatrix, kinectHandViewPos_[0], kinectHandRot_[0], debug_);
								blueRay_[1]->draw(graphicsManager_, worldMatrix, viewMatrix, orthoMatrix, kinectHandViewPos_[1], kinectHandRot_[1], debug_);
							}
							break;					
					}

					graphicsManager_->turnOffAlphaBlending();
					graphicsManager_->turnZBufferOn();
				}

				if(playLevelState_ == SELECT_MEDAL)
				{
					LightClass tempLight;
					tempLight.setAmbientColor(0.1f, 0.1f, 0.1f, 1.0f);
					tempLight.setDiffuseColor(0.8f, 0.8f, 0.8f, 1.0f);
					tempLight.setDirection(0.0f, -0.5f, 1.0f);

					switch(actualLevelState_)
					{
						case 1:
							medals_[2]->draw(graphicsManager_->getDevice(), graphicsManager_->getDeviceContext(), worldMatrix, viewMatrix, projectionMatrix, &tempLight);
							break;
						case 2:
							medals_[1]->draw(graphicsManager_->getDevice(), graphicsManager_->getDeviceContext(), worldMatrix, viewMatrix, projectionMatrix, &tempLight);
							break;
						case 3:
							medals_[0]->draw(graphicsManager_->getDevice(), graphicsManager_->getDeviceContext(), worldMatrix, viewMatrix, projectionMatrix, &tempLight);
							break;
						case 4:
							medals_[1]->setPosition(Point(1.2f, 3.0f, -6.0f));
							medals_[2]->setPosition(Point(-1.2f, 3.0f, -6.0f));

							medals_[0]->draw(graphicsManager_->getDevice(), graphicsManager_->getDeviceContext(), worldMatrix, viewMatrix, projectionMatrix, &tempLight);
							medals_[1]->draw(graphicsManager_->getDevice(), graphicsManager_->getDeviceContext(), worldMatrix, viewMatrix, projectionMatrix, &tempLight);
							medals_[2]->draw(graphicsManager_->getDevice(), graphicsManager_->getDeviceContext(), worldMatrix, viewMatrix, projectionMatrix, &tempLight);
							break;
					}
				}
			}
			break;
		case ENDING:
			{
				pico_->draw(graphicsManager_, worldMatrix, viewMatrix, projectionMatrix, light_, debug_);
			}
			break;
		default:
			{
				
			}
			break;
	}

	// Draw all debug stuff!
	if(debug_)
	{
		graphicsManager_->turnZBufferOff();
		graphicsManager_->turnOnAlphaBlending();
			FPS_->draw(graphicsManager_->getDeviceContext(), worldMatrix, viewMatrix, orthoMatrix);
			kinectHands_[0]->draw(graphicsManager_->getDeviceContext(), worldMatrix, viewMatrix, orthoMatrix);
			kinectHands_[1]->draw(graphicsManager_->getDeviceContext(), worldMatrix, viewMatrix, orthoMatrix);
			kinectHand_->draw(graphicsManager_->getDeviceContext(), ((screenWidth_/2)*-1)+kinectHandViewPos_[0].x, ((screenHeight_/2))-kinectHandViewPos_[0].y, worldMatrix, viewMatrix, orthoMatrix, XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f));
			kinectHand_->draw(graphicsManager_->getDeviceContext(), ((screenWidth_/2)*-1)+kinectHandViewPos_[1].x, ((screenHeight_/2))-kinectHandViewPos_[1].y, worldMatrix, viewMatrix, orthoMatrix, XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f));
			rayInfo_[0]->draw(graphicsManager_->getDeviceContext(), worldMatrix, viewMatrix, orthoMatrix);
			rayInfo_[1]->draw(graphicsManager_->getDeviceContext(), worldMatrix, viewMatrix, orthoMatrix);
		graphicsManager_->turnOffAlphaBlending();
		graphicsManager_->turnZBufferOn();
	}
}

void ThirdScreenState::destroy()
{
	LogClass::Instance()->setEndTime();

	if(spaceShip_)
	{
		spaceShip_->destroy();
		delete spaceShip_;
		spaceShip_ = 0;
	}

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

	// Destroy scenario models
	std::vector<Object3D*>::iterator objectsIt;
	for(objectsIt = scenario_.begin(); objectsIt != scenario_.end(); objectsIt++)
	{
		(*objectsIt)->destroy();
	}
}

void ThirdScreenState::notify(InputManager* notifier, InputStruct arg)
{
	switch(arg.keyPressed){
		case 49: //1
		case 50: //2
		case 51: //3
		case 52: //4
		case 53: //5
			{
				switch(levelState_)
				{
					case PLAYING:
						{
							switch(playLevelState_)
							{
								case LEVEL_3:
									{
										friendSpacheShips_[arg.keyPressed-49]->makeItFree();
									}
									break;
							}
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
		case 32: // Space
			{
				switch(levelState_)
				{
					case PLAYING:
						{
							switch(playLevelState_)
							{
								case LEVEL_1:
									{
										friendSpacheShips_[actualFriendSpaceShip_]->makeItFree();
									}
									break;
								case LEVEL_2:
									{
										switch(activeRay_)
										{
											case GREEN:
												{
													friendSpacheShips_[0]->makeItFree();
												}
												break;
											case RED:
												{
													friendSpacheShips_[1]->makeItFree();
												}
												break;
										}
									}
									break;
								case LEVEL_4:
									{
										friendSpacheShips_[actualFriendSpaceShip_]->makeItFree();
									}
									break;
							}
						}
						break;
				}
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
				
			}
			break;
		case RIGHT_BUTTON:
			{

			}
			break;
		default:
			{
				
			}
			break;
	}
}

void ThirdScreenState::notify(KinectClass* notifier, KinectStruct arg)
{
	kinectHandPreviousRot_[0] = kinectHandRot_[0];
	kinectHandPreviousRot_[1] = kinectHandRot_[1];

	if(arg.type == FIRST_RIGHT_HAND_ROT)
	{
		// Calculate viewport position
		//kinectHandViewPos_[0] = Point(arg.handPos.x*screenWidth_/320, arg.handPos.y*screenHeight_/240);
		kinectHandViewPos_[0] = Point(arg.handPos.x, arg.handPos.y);

		// Calculate world position
		kinectHandWorldPos_[0].x = ((kinectHandViewPos_[0].x/screenWidth_)*8)-4;
		kinectHandWorldPos_[0].y = (1-(kinectHandViewPos_[0].y/screenHeight_))*5;
		kinectHandWorldPos_[0].z = 0;

		float change = abs(arg.handRot - kinectHandPreviousRot_[0]);
		if(change < XM_PI)
		{
			kinectHandRot_[0] = kinectHandPreviousRot_[0] + 0.15 * (arg.handRot - kinectHandPreviousRot_[0]);
		}
		else
		{
			kinectHandRot_[0] = arg.handRot;
		}
	}
	
	if(arg.type == SECOND_RIGHT_HAND_ROT)
	{
		// Calculate viewport position
		//kinectHandViewPos_[1] = Point(arg.handPos.x*screenWidth_/320, arg.handPos.y*screenHeight_/240);
		kinectHandViewPos_[1] = Point(arg.handPos.x, arg.handPos.y);

		// Calculate world position
		kinectHandWorldPos_[1].x = ((kinectHandViewPos_[1].x/screenWidth_)*8)-4;
		kinectHandWorldPos_[1].y = (1-(kinectHandViewPos_[1].y/screenHeight_))*5;
		kinectHandWorldPos_[1].z = 0;

		float change = abs(arg.handRot - kinectHandPreviousRot_[1]);
		if(change < XM_PI)
		{
			kinectHandRot_[1] = kinectHandPreviousRot_[1] + 0.15 * (arg.handRot - kinectHandPreviousRot_[1]);
		}
		else
		{
			kinectHandRot_[1] = arg.handRot;
		}
	}
}

void ThirdScreenState::notify(FriendSpaceShipClass* notifier, bool arg)
{
	if(playLevelState_ == LEVEL_4 && arg)
	{
		subLevel4Freed_++;
		playClock_->reset();
	}
}

bool ThirdScreenState::setupAllSpaceShips()
{
	// Create the space ship.
	spaceShip_ = new SpaceShipClass;
	if(!spaceShip_)
	{
		return false;
	}
	if(!spaceShip_->setup(graphicsManager_, NULL, 2.0f))
	{
		MessageBoxA(NULL, "Could not initialize the SpaceShip.", "Error", MB_ICONERROR | MB_OK);
		return false;
	}
	Shader3DClass* shaderTemp = Shader3DFactory::Instance()->CreateShader3D("PointlightDiffuseShader3D", graphicsManager_);
	spaceShip_->getObject()->setShader3D(shaderTemp);
	spaceShip_->setPosition(Point(15.0f, 5.0f, -3.25f));
	spaceShip_->setPosition(Point(15.0f, 5.0f, -3.25f));
	spaceShip_->setScale(Vector(0.03f, 0.03f, 0.03f));
	spaceShip_->setRotation(0.0f, 0.0f, XM_PI);
	spaceShip_->setFloorHeight(terrainHeight_);

	setupFriendSpaceShips();

	return setupFriendSpaceShips();
}

bool ThirdScreenState::setupDebugOptions()
{
	debug_ = false;

	FPS_ = new TextClass();
	if(!FPS_)
	{
		return false;
	}

	// Initialize the text object.
	if(!FPS_->setup(graphicsManager_->getDevice(), graphicsManager_->getDeviceContext(), graphicsManager_->getShader2D(), screenWidth_, screenHeight_, 20, 10, "FPS: "))
	{
		MessageBoxA(NULL, "Could not initialize the FPS text object.", "SecondScreen - Error", MB_OK);
		return false;
	}

	if(!setupKinectHands())
	{
		return false;
	}

	return true;
}

bool ThirdScreenState::setupFriendSpaceShips()
{
	for(int i = 0; i < 5; i++)
	{
		friendSpacheShips_[i] = new FriendSpaceShipClass;
		if(!friendSpacheShips_[i])
		{
			MessageBoxA(NULL, "Could not create friend spaceship instance.", "Error", MB_ICONERROR | MB_OK);
			return false;
		}
		if(i == 0 || i == 2 || i == 4)
		{
			if(!friendSpacheShips_[i]->setup(graphicsManager_, soundManager_, "nave"))
			{
				MessageBoxA(NULL, "Could not setup the firend spaceship.", "Error", MB_ICONERROR | MB_OK);
				return false;
			}
		}
		else
		{
			if(!friendSpacheShips_[i]->setup(graphicsManager_, soundManager_, "navepascal"))
			{
				MessageBoxA(NULL, "Could not setup the firend flying saucer.", "Error", MB_ICONERROR | MB_OK);
				return false;
			}
		}

		friendSpacheShips_[i]->setPosition(Point(20.0f, 0.0f, 0.0f));
		friendSpacheShips_[i]->addListener(*pico_);
		friendSpacheShips_[i]->addListener(*this);
	}

	return true;
}

bool ThirdScreenState::setupKinectHands()
{
	kinectHands_[0] = new TextClass();
	if(!kinectHands_[0])
	{
		return false;
	}
	// Initialize the text object.
	if(!kinectHands_[0]->setup(graphicsManager_->getDevice(), graphicsManager_->getDeviceContext(), graphicsManager_->getShader2D(), screenWidth_, screenHeight_, 20, 30, "Kinect1: "))
	{
		MessageBoxA(NULL, "Could not initialize the Kinect text object.", "SecondScreen - Error", MB_OK);
		return false;
	}

	kinectHands_[1] = new TextClass();
	if(!kinectHands_[1])
	{
		return false;
	}
	// Initialize the text object.
	if(!kinectHands_[1]->setup(graphicsManager_->getDevice(), graphicsManager_->getDeviceContext(), graphicsManager_->getShader2D(), screenWidth_, screenHeight_, 20, 50, "Kinect2: "))
	{
		MessageBoxA(NULL, "Could not initialize the Kinect text object.", "SecondScreen - Error", MB_OK);
		return false;
	}

	kinectHand_ = new ImageClass;
	if(!kinectHand_)
	{
		MessageBoxA(NULL, "Could not initialize the kinectHand_ image instance.", "SecondScreen - Error", MB_OK);
		return false;
	}

	if(!kinectHand_->setup(graphicsManager_->getDevice(), graphicsManager_->getShader2D(), screenWidth_, screenHeight_, "hand", 30, 50))
	{
		MessageBoxA(NULL, "Could not setup the kinectHand_ image.", "SecondScreen - Error", MB_OK);
		return false;
	}

	return true;
}

bool ThirdScreenState::setupRayInfo()
{
	rayInfo_[0] = new TextClass();
	if(!rayInfo_[0])
	{
		return false;
	}
	// Initialize the text object.
	if(!rayInfo_[0]->setup(graphicsManager_->getDevice(), graphicsManager_->getDeviceContext(), graphicsManager_->getShader2D(), screenWidth_, screenHeight_, 20, 90, "Ray1: "))
	{
		MessageBoxA(NULL, "Could not initialize the Ray text object.", "SecondScreen - Error", MB_OK);
		return false;
	}

	rayInfo_[1] = new TextClass();
	if(!rayInfo_[1])
	{
		return false;
	}
	// Initialize the text object.
	if(!rayInfo_[1]->setup(graphicsManager_->getDevice(), graphicsManager_->getDeviceContext(), graphicsManager_->getShader2D(), screenWidth_, screenHeight_, 20, 110, "Ray2: "))
	{
		MessageBoxA(NULL, "Could not initialize the Ray text object.", "SecondScreen - Error", MB_OK);
		return false;
	}

	return true;
}

bool ThirdScreenState::setupRays()
{
	for(int i = 0; i < 2; i++)
	{
		blueRay_[i] = new SpriteAnimationClass;
		if(!blueRay_[i])
		{
			MessageBoxA(NULL, "Could not create blue ray instance.", "Error", MB_ICONERROR | MB_OK);
			return false;
		}
		if(!blueRay_[i]->setup(graphicsManager_, "BlueInterf_", 4))
		{
			MessageBoxA(NULL, "Could not setup the blue ray.", "Error", MB_ICONERROR | MB_OK);
			return false;
		}

		greenRay_[i] = new SpriteAnimationClass;
		if(!greenRay_[i])
		{
			MessageBoxA(NULL, "Could not create green ray instance.", "Error", MB_ICONERROR | MB_OK);
			return false;
		}
		if(!greenRay_[i]->setup(graphicsManager_, "GreenWave_", 4))
		{
			MessageBoxA(NULL, "Could not setup the green ray.", "Error", MB_ICONERROR | MB_OK);
			return false;
		}

		redRay_[i] = new SpriteAnimationClass;
		if(!redRay_[i])
		{
			MessageBoxA(NULL, "Could not create green ray instance.", "Error", MB_ICONERROR | MB_OK);
			return false;
		}
		if(!redRay_[i]->setup(graphicsManager_, "redZigZag_", 3))
		{
			MessageBoxA(NULL, "Could not setup the green ray.", "Error", MB_ICONERROR | MB_OK);
			return false;
		}
	}

	return true;
}

bool ThirdScreenState::setupSplashScreen()
{
	// load splashscreen and calculate its position
	splashScreen_ = new ImageClass;
	if(!splashScreen_)
	{
		MessageBoxA(NULL, "Could not initialize the splashscreen image instance.", "SecondScreen - Error", MB_OK);
		return false;
	}

	if(!splashScreen_->setup(graphicsManager_->getDevice(), graphicsManager_->getShader2D(), screenWidth_, screenHeight_, "splashscreen3", screenWidth_, screenHeight_))
	{
		MessageBoxA(NULL, "Could not setup the splashscreen image.", "SecondScreen - Error", MB_OK);
		return false;
	}

	return true;
}

void ThirdScreenState::updateIntroduction(float elapsedTime)
{
	introClock_->tick();

	spaceShip_->update(elapsedTime);
	pico_->update(elapsedTime);

	switch(introLevelState_)
	{
		case ARRIBING:
			{
				if(spaceShip_->checkArrivedObjective())
				{
					spaceShip_->setSpaceShipState(PREPARED);
					introLevelState_ = LANDING;

					introClock_->reset();
				}
			}
			break;
		case LANDING:
			{
				if(introClock_->getTime() > spaceShipIntroWaitTime_)
				{
					pico_->setPosition(Point(0.0f, 5.0f, -3.25f));
					introLevelState_ = PICO_APPEARS;
					
					introClock_->reset();
				}
			}
			break;
		case PICO_APPEARS:
			{
				float dist = (5.0f-terrainHeight_)*(introClock_->getTime()/introFallTime_);
				pico_->setPosition(Point(0.0f, 5.0f-dist, -3.25f));

				if(introClock_->getTime() > introFallTime_)
				{
					pico_->setPosition(Point(0.0f, terrainHeight_, -3.25f));
					pico_->makeGreeting();
					spaceShip_->goToPosition(Point(-10.0f, 5.0f, -3.25f));
					kinectClass_->setUserColor(XMFLOAT4(1.0f, 1.0f, 1.0f, 0.5f));
					
					introLevelState_ = LEAVING;

					introClock_->reset();
				}
			}
			break;
		case LEAVING:
			{
				if(spaceShip_->checkArrivedObjective())
				{
					spaceShip_->setSpaceShipState(PREPARED);
					introLevelState_ = GREETING;

					introClock_->reset();
				}
			}
			break;
		case GREETING:
			{
				if(introClock_->getTime() > introGreetTime_)
				{
					pico_->makePointing(false);
					friendSpacheShips_[0]->setPosition(Point(-8.75f, 5.0f, 0.0f));
					friendSpacheShips_[0]->goToPosition(Point(0.0f, 5.0f, 0.0f));
					introLevelState_ = TRANSITION;

					introClock_->reset();
				}
			}
		case TRANSITION:
			{
				if(introClock_->getTime() > introSpaceTime_)
				{
					pico_->makeRest(false);

					levelState_ = PLAYING;
					playLevelState_ = LEVEL_1;
					//playLevelState_ = LEVEL_4;
					//activeRay_ = RED;
					actualFriendSpaceShip_ = 0;

					drawRays_ = true;

					gameClock_->reset();
				}
			}
			break;
		default:
			{

			}
			break;
	}
}

void ThirdScreenState::updatePlaying(float elapsedTime)
{	
	playClock_->tick();
	noClock_->tick();

	spaceShip_->update(elapsedTime);

	if(playLevelState_ != FADING && playLevelState_ != SELECT_MEDAL)
	{
		pico_->update(elapsedTime);
	}

	switch(playLevelState_)
	{
		case LEVEL_1:
			{
				updatePlayingSublevel1(elapsedTime);
			}
			break;
		case CELEBRATING_LEVEL_1:
			{
				updateCelebration(elapsedTime);
			}
			break;
		case LEVEL_2:
			{
				updatePlayingSublevel2(elapsedTime);
			}
			break;
		case CELEBRATING_LEVEL_2:
			{
				updateCelebration(elapsedTime);
			}
			break;
		case LEVEL_3:
			{
				updatePlayingSublevel3(elapsedTime);
			}
			break;
		case CELEBRATING_LEVEL_3:
			{
				updateCelebration(elapsedTime);
			}
			break;
		case LEVEL_4:
			{
				updatePlayingSublevel4(elapsedTime);
			}
			break;
		case CELEBRATING_LEVEL_4:
			{
				updateCelebration(elapsedTime);
			}
			break;
		case FADING:
			{
				float diffuseTemp = 1.0f - playClock_->getTime()/fadeTime_;
				light_->setDiffuseColor(diffuseTemp, diffuseTemp, diffuseTemp, 1.0f);

				float ambientTemp = 0.1f - (playClock_->getTime()/fadeTime_)*0.075f;
				light_->setAmbientColor(ambientTemp, ambientTemp, ambientTemp, 1.0f);

				soundManager_->playFile("change_level", false);

				if(playClock_->getTime() > fadeTime_)
				{
					playLevelState_ = SELECT_MEDAL;
					playClock_->reset();
				}
			}
			break;
		case SELECT_MEDAL:
			{
				soundManager_->playFile("change_level", false);

				if(playClock_->getTime() > 15.0f)
				{
					light_->setAmbientColor(0.1f, 0.1f, 0.1f, 1.0f);
					light_->setDiffuseColor(0.8f, 0.8f, 0.8f, 1.0f);
					light_->setDirection(0.0f, -0.5f, 1.0f);

					switch(actualLevelState_)
					{
						case 1:
							playLevelState_ = LEVEL_2;
							break;
						case 2:
							playLevelState_ = LEVEL_3;
							break;
						case 3:
							playLevelState_ = LEVEL_4;
							break;
						case 4:
							levelState_ = ENDING;
							break;
					}

					playClock_->reset();
				}
			}
			break;
		default:
			{

			}
			break;
	}
}

void ThirdScreenState::updatePlayingSublevel1(float elapsedTime)
{
	// Updtae ships
	for(int i = 0; i < 3; i++)
	{
		friendSpacheShips_[i]->update(elapsedTime);
	}

	if(isPointing_)
	{
		pointingClock_->tick();

		if(pointingClock_->getTime() > 2.0f)
		{
			pico_->makeRest(false);
			isPointing_ = false;
		}
	}
				
	// If first ship is waiting release it and say to draw the rays
	/*if(!friendSpacheShips_[0]->isWaiting())
	{
		drawRays_ = true;
	}*/

	// When first ship is out but has not celebrated
	if(friendSpacheShips_[0]->isOut() && friendSpacheShips_[1]->isWaiting())
	{
		// Launch second spaceship
		friendSpacheShips_[0]->setPosition(Point(10.75f, 5.0f, 0.0f));
		friendSpacheShips_[0]->celebrateToPosition(Point(-10.75f, 5.0f, 0.0f));
		pico_->changeExpression("feliz");
		pico_->changeAnimation("celebration", 0.2f);
		drawRays_ = false;
	}

	// When first ship has celebrated but second is still waiting, make second start
	if(friendSpacheShips_[0]->hasCelebrated() && friendSpacheShips_[1]->isWaiting())
	{
		pico_->makePointing(true);
		pointingClock_->reset();
		isPointing_ = true;
		drawRays_ = true;

		// Launch second spaceship
		friendSpacheShips_[1]->setPosition(Point(8.75f, 5.0f, 0.0f));
		friendSpacheShips_[1]->goToPosition(Point(-2.0f, 5.0f, 0.0f));
		actualFriendSpaceShip_ = 1;
	}
	// When second ship is out but has not celebrated
	if(friendSpacheShips_[1]->isOut() && friendSpacheShips_[2]->isWaiting())
	{
		// Launch second spaceship
		friendSpacheShips_[1]->setPosition(Point(-10.75f, 5.0f, 0.0f));
		friendSpacheShips_[1]->celebrateToPosition(Point(10.75f, 5.0f, 0.0f));
		pico_->changeExpression("feliz");
		pico_->changeAnimation("celebration", 0.2f);
		drawRays_ = false;
	}

	// Same as above but for second and third
	if(friendSpacheShips_[1]->hasCelebrated() && friendSpacheShips_[2]->isWaiting())
	{
		pico_->makePointing(false);
		pointingClock_->reset();
		isPointing_ = true;
		drawRays_ = true;

		// launch third spaceship
		friendSpacheShips_[2]->setPosition(Point(-8.75f, 5.0f, 0.0f));
		friendSpacheShips_[2]->goToPosition(Point(3.0f, 5.0f, 0.0f));
		actualFriendSpaceShip_ = 2;
	}
	// When third ship is out but has not celebrated
	if(friendSpacheShips_[2]->isOut())
	{
		// Launch second spaceship
		friendSpacheShips_[2]->setPosition(Point(10.75f, 5.0f, 0.0f));
		friendSpacheShips_[2]->celebrateToPosition(Point(-10.75f, 5.0f, 0.0f));
		pico_->changeExpression("feliz");
		pico_->changeAnimation("celebration", 0.2f);
		drawRays_ = false;
	}

	// Check whenever rays are overlaping the sips
	if(greenRay_[0]->getPointDistance(kinectHandViewPos_[0], friendSpacheShips_[actualFriendSpaceShip_]->getViewportPosition()) < distanceThreshold_ &&
		greenRay_[1]->getPointDistance(kinectHandViewPos_[1], friendSpacheShips_[actualFriendSpaceShip_]->getViewportPosition()) < distanceThreshold_)
	{
		bothTouching_ = true;

		if(friendSpacheShips_[actualFriendSpaceShip_]->isStuck())
		{
			LogClass::Instance()->addEntry("BOTH_TOUCHING_SPACESHIP", 1, actualFriendSpaceShip_);
			soundManager_->playFile("laser_continu_bo", false);
		}

		noClock_->reset();
		noClock_->stop();

		friendSpacheShips_[actualFriendSpaceShip_]->freeIt(elapsedTime);
	}
	else
	{
		if(bothTouching_ && friendSpacheShips_[actualFriendSpaceShip_]->isStuck())
		{
			noClock_->reset();
		}
		if(noClock_->getTime() > 1.0f)
		{
			pico_->makeNo();
			LogClass::Instance()->addEntry("BOTH_NOT_TOUCHING_SPACESHIP", 1, actualFriendSpaceShip_);

			noClock_->reset();
			noClock_->stop();
		}
		bothTouching_ = false;
	}

	// When the three ships have celebrated (thus, have breen set free)
	if(friendSpacheShips_[0]->hasCelebrated() && friendSpacheShips_[1]->hasCelebrated() && friendSpacheShips_[2]->hasCelebrated())
	{
		//pico_->makeCelebrate();
		pico_->changeExpression("feliz");
		pico_->changeAnimation("DanceAss", 0.2f);
		soundManager_->playFile("fanfare", false);
		LogClass::Instance()->addEntry("FIRST_LEVEL_FINISHED", 0, 0);

		friendSpacheShips_[0]->setPosition(Point(8.75f, 5.5f, 0.0f));
		friendSpacheShips_[1]->setPosition(Point(-8.75f, 4.5f, 0.0f));

		playClock_->reset();
		playLevelState_ = CELEBRATING_LEVEL_1;
		activeRay_ = RED;
		secondLevelIteration_ = 0;
	}
}

void ThirdScreenState::updatePlayingSublevel2(float elapsedTime)
{
	// Updtae ships
	for(int i = 0; i < 2; i++)
	{
		friendSpacheShips_[i]->update(elapsedTime);
	}

	if(isPointing_)
	{
		pointingClock_->tick();

		if(pointingClock_->getTime() > 1.0f)
		{
			pico_->makeRest(false);
			isPointing_ = false;
		}
	}

	// When 9 seconds pass, launch the spaceships for the sublevel
	if(playClock_->getTime() > 3.0f)
	{
		switch(secondLevelIteration_)
		{
			case 0:
				{
					pico_->makePointing(true);
					pointingClock_->reset();
					isPointing_ = true;
					drawRays_ = true;

					friendSpacheShips_[0]->goToPosition(Point(3.0f, 5.5f, 0.0f));
					friendSpacheShips_[0]->setType(GREEN);

					friendSpacheShips_[1]->goToPosition(Point(-1.0f, 4.5f, 0.0f));
					friendSpacheShips_[1]->setType(RED);

					playClock_->reset();
					playClock_->stop();
				}
				break;
			case 1:
				{
					pico_->makePointing(true);
					pointingClock_->reset();
					isPointing_ = true;
					drawRays_ = true;

					friendSpacheShips_[0]->goToPosition(Point(0.0f, 5.5f, 0.0f));
					friendSpacheShips_[0]->setType(GREEN);

					friendSpacheShips_[1]->goToPosition(Point(-3.0f, 4.5f, 0.0f));
					friendSpacheShips_[1]->setType(RED);

					playClock_->reset();
					playClock_->stop();
				}
				break;
			case 2:
				{
					pico_->makePointing(true);
					pointingClock_->reset();
					isPointing_ = true;
					drawRays_ = true;

					friendSpacheShips_[0]->goToPosition(Point(-1.0f, 5.5f, 0.0f));
					friendSpacheShips_[0]->setType(GREEN);

					friendSpacheShips_[1]->goToPosition(Point(3.0f, 4.5f, 0.0f));
					friendSpacheShips_[1]->setType(RED);

					playClock_->reset();
					playClock_->stop();
				}
				break;
		}
	}

	// Check whenever rays are overlaping the sips depending on color
	switch(activeRay_)
	{
		case RED:
			{
				// If it is the green, we work with friendSpaceShip 1
				if(redRay_[0]->getPointDistance(kinectHandViewPos_[0], friendSpacheShips_[1]->getViewportPosition()) < distanceThreshold_ &&
					redRay_[1]->getPointDistance(kinectHandViewPos_[1], friendSpacheShips_[1]->getViewportPosition()) < distanceThreshold_)
				{
					bothTouching_ = true;

					if(friendSpacheShips_[1]->isStuck())
					{
						LogClass::Instance()->addEntry("BOTH_TOUCHING_SPACESHIP", 2, 0);
						soundManager_->playFile("laser_continu_bo", false);
					}

					noClock_->reset();
					noClock_->stop();

					friendSpacheShips_[1]->freeIt(elapsedTime);
				}
				else
				{
					if(bothTouching_ && friendSpacheShips_[1]->isStuck())
					{
						noClock_->reset();
					}
					if(noClock_->getTime() > 1.0f)
					{
						pico_->makeNo();
						LogClass::Instance()->addEntry("BOTH_NOT_TOUCHING_SPACESHIP", 2, 0);

						noClock_->reset();
						noClock_->stop();
					}
					bothTouching_ = false;
				}

				if(friendSpacheShips_[1]->isOut())
				{
					activeRay_ = GREEN;
				}
			}
			break;
		case GREEN:
			{
				// If it is the green, we work with friendSpaceShip 0
				if(greenRay_[0]->getPointDistance(kinectHandViewPos_[0], friendSpacheShips_[0]->getViewportPosition()) < distanceThreshold_ &&
					greenRay_[1]->getPointDistance(kinectHandViewPos_[1], friendSpacheShips_[0]->getViewportPosition()) < distanceThreshold_)
				{
					bothTouching_ = true;

					if(friendSpacheShips_[0]->isStuck())
					{
						LogClass::Instance()->addEntry("BOTH_TOUCHING_SPACESHIP", 2, 1);
						soundManager_->playFile("laser_continu_bo", false);
					}

					noClock_->reset();
					noClock_->stop();

					friendSpacheShips_[0]->freeIt(elapsedTime);
				}
				else
				{
					if(bothTouching_ && friendSpacheShips_[0]->isStuck())
					{
						noClock_->reset();
					}
					if(noClock_->getTime() > 1.0f)
					{
						pico_->makeNo();
						LogClass::Instance()->addEntry("BOTH_NOT_TOUCHING_SPACESHIP", 2, 1);

						noClock_->reset();
						noClock_->stop();
					}
					bothTouching_ = false;
				}

				// When Green SpaceShip leaves means that the sublevel or level has finished
				if(friendSpacheShips_[0]->hasCelebrated() && friendSpacheShips_[1]->hasCelebrated())
				{
					// If we are in the last sublevel iteration, we advance to the third part of the playing leve
					if(secondLevelIteration_ == 2)
					{
						pico_->makeCelebrate();
						soundManager_->playFile("fanfare", false);

						playClock_->reset();
						LogClass::Instance()->addEntry("SECOND_LEVEL_FINISHED", 0, 0);
						playLevelState_ = CELEBRATING_LEVEL_2;
						thirdLevelTimes_ = 0;
						activeRay_ = RED;

						friendSpacheShips_[0]->setPosition(Point(8.75f, 5.5f, 0.0f));
						friendSpacheShips_[1]->setPosition(Point(-8.75f, 4.5f, 0.0f));
						friendSpacheShips_[2]->setPosition(Point(8.75f, 4.5f, 0.0f));
						friendSpacheShips_[3]->setPosition(Point(8.75f, 4.5f, 0.0f));
						friendSpacheShips_[4]->setPosition(Point(-8.75f, 4.5f, 0.0f));
					}
					// Otherwise we advance one sublevel
					else
					{
						// Reset to red ray
						activeRay_ = RED;
						secondLevelIteration_++;

						// Set spaceship positions depending on sublevel state
						if(secondLevelIteration_ == 1)
						{
							friendSpacheShips_[0]->setPosition(Point(8.75f, 5.5f, 0.0f));
							friendSpacheShips_[1]->setPosition(Point(-8.75f, 4.5f, 0.0f));
						}
						else
						{
							friendSpacheShips_[0]->setPosition(Point(8.75f, 5.5f, 0.0f));
							friendSpacheShips_[1]->setPosition(Point(-8.75f, 4.5f, 0.0f));
						}
						playClock_->reset();
					}
				}

				if(friendSpacheShips_[0]->isOut() && friendSpacheShips_[1]->isOut())
				{
					friendSpacheShips_[0]->setPosition(Point(10.75f, 5.5f, 0.0f));
					friendSpacheShips_[1]->setPosition(Point(-10.75f, 4.5f, 0.0f));
					friendSpacheShips_[0]->celebrateToPosition(Point(-10.75f, 5.5f, 0.0f));
					friendSpacheShips_[1]->celebrateToPosition(Point(10.75f, 4.5f, 0.0f));

					drawRays_ = false;

					// Keep Pico celebrating
					soundManager_->playCelebrate();
					pico_->changeExpression("feliz");
					pico_->changeAnimation("celebration", 0.2f);
				}
			}
			break;
	}
}

void ThirdScreenState::updatePlayingSublevel3(float elapsedTime)
{
	// Updtae ships
	for(int i = 0; i < 5; i++)
	{
		friendSpacheShips_[i]->update(elapsedTime);
	}

	// Control pointing behaviour from Pico
	if(isPointing_)
	{
		pointingClock_->tick();

		if(pointingClock_->getTime() > 2.0f)
		{
			pico_->makeRest(false);
			isPointing_ = false;
		}
	}

	// Set FriendSpaceShips to appear after 9 seconds
	if(playClock_->getTime() > 1.0f)
	{
		pico_->makePointing(true);
		pointingClock_->reset();
		isPointing_ = true;
		drawRays_ = true;

		friendSpacheShips_[0]->goToPosition(Point(-5.0f, 5.5f, 0.0f));
		friendSpacheShips_[0]->setType(RED);
		friendSpacheShips_[1]->goToPosition(Point(-1.0f, 5.5f, 0.0f));
		friendSpacheShips_[1]->setType(RED);
		friendSpacheShips_[2]->goToPosition(Point(-2.0f, 4.5f, 0.0f));
		friendSpacheShips_[2]->setType(GREEN);
		friendSpacheShips_[3]->goToPosition(Point(3.0f, 4.5f, 0.0f));
		friendSpacheShips_[3]->setType(GREEN);
		friendSpacheShips_[4]->goToPosition(Point(5.0f, 5.5f, 0.0f));
		friendSpacheShips_[4]->setType(BLUE);

		playClock_->reset();
		playClock_->stop();
	}

	// Check whenever rays are overlaping the sips depending on color
	switch(activeRay_)
	{
		case RED:
			{
				// If it is the green, we work with friendSpaceShips 0 and 1
				if(redRay_[0]->getPointDistance(kinectHandViewPos_[0], friendSpacheShips_[0]->getViewportPosition()) < distanceThreshold_ &&
				   redRay_[1]->getPointDistance(kinectHandViewPos_[1], friendSpacheShips_[0]->getViewportPosition()) < distanceThreshold_)
				{
					bothTouching_ = true;

					if(friendSpacheShips_[0]->isStuck())
					{
						LogClass::Instance()->addEntry("BOTH_TOUCHING_SPACESHIP", 3, 0);
						soundManager_->playFile("laser_continu_bo", false);
					}
					
					noClock_->reset();
					noClock_->stop();

					friendSpacheShips_[0]->freeIt(elapsedTime);
				}
				else
				{
					if(bothTouching_ && friendSpacheShips_[0]->isStuck())
					{
						noClock_->reset();
					}
					if(noClock_->getTime() > 1.0f)
					{
						pico_->makeNo();

						noClock_->reset();
						noClock_->stop();
					}
					bothTouching_ = false;
				}
				if(redRay_[0]->getPointDistance(kinectHandViewPos_[0], friendSpacheShips_[1]->getViewportPosition()) < distanceThreshold_ &&
				   redRay_[1]->getPointDistance(kinectHandViewPos_[1], friendSpacheShips_[1]->getViewportPosition()) < distanceThreshold_)
				{
					bothTouching_ = true;
					if(friendSpacheShips_[1]->isStuck())
					{
						LogClass::Instance()->addEntry("BOTH_TOUCHING_SPACESHIP", 3, 1);
						soundManager_->playFile("laser_continu_bo", false);
					}
					
					noClock_->reset();
					noClock_->stop();

					friendSpacheShips_[1]->freeIt(elapsedTime);
				}
				else
				{
					if(bothTouching_ && friendSpacheShips_[1]->isStuck())
					{
						noClock_->reset();
					}
					if(noClock_->getTime() > 1.0f)
					{
						pico_->makeNo();

						noClock_->reset();
						noClock_->stop();
					}
					bothTouching_ = false;
				}

				if(friendSpacheShips_[0]->isOut() && friendSpacheShips_[1]->isOut())
				{
					activeRay_ = GREEN;
				}
			}
			break;
		case GREEN:
			{
				// If it is the green, we work with friendSpaceShips 2 and 3
				if(greenRay_[0]->getPointDistance(kinectHandViewPos_[0], friendSpacheShips_[2]->getViewportPosition()) < distanceThreshold_ &&
				   greenRay_[1]->getPointDistance(kinectHandViewPos_[1], friendSpacheShips_[2]->getViewportPosition()) < distanceThreshold_)
				{
					bothTouching_ = true;
					if(friendSpacheShips_[2]->isStuck())
					{
						LogClass::Instance()->addEntry("BOTH_TOUCHING_SPACESHIP", 3, 2);
						soundManager_->playFile("laser_continu_bo", false);
					}
					
					noClock_->reset();
					noClock_->stop();

					friendSpacheShips_[2]->freeIt(elapsedTime);
				}
				else
				{
					if(bothTouching_ && friendSpacheShips_[2]->isStuck())
					{
						noClock_->reset();
					}
					if(noClock_->getTime() > 1.0f)
					{
						pico_->makeNo();

						noClock_->reset();
						noClock_->stop();
					}
					bothTouching_ = false;
				}
				if(greenRay_[0]->getPointDistance(kinectHandViewPos_[0], friendSpacheShips_[3]->getViewportPosition()) < distanceThreshold_ &&
				   greenRay_[1]->getPointDistance(kinectHandViewPos_[1], friendSpacheShips_[3]->getViewportPosition()) < distanceThreshold_)
				{
					bothTouching_ = true;
					if(friendSpacheShips_[3]->isStuck())
					{
						LogClass::Instance()->addEntry("BOTH_TOUCHING_SPACESHIP", 3, 3);
						soundManager_->playFile("laser_continu_bo", false);
					}
					
					noClock_->reset();
					noClock_->stop();

					friendSpacheShips_[3]->freeIt(elapsedTime);
				}
				else
				{
					if(bothTouching_ && friendSpacheShips_[3]->isStuck())
					{
						noClock_->reset();
					}
					if(noClock_->getTime() > 1.0f)
					{
						pico_->makeNo();

						noClock_->reset();
						noClock_->stop();
					}
					bothTouching_ = false;
				}

				if(friendSpacheShips_[2]->isOut() && friendSpacheShips_[3]->isOut())
				{
					activeRay_ = BLUE;
				}
			}
			break;
		case BLUE:
			{
				// If it is the green, we work with friendSpaceShip 1
				if(blueRay_[0]->getPointDistance(kinectHandViewPos_[0], friendSpacheShips_[4]->getViewportPosition()) < distanceThreshold_ &&
				   blueRay_[1]->getPointDistance(kinectHandViewPos_[1], friendSpacheShips_[4]->getViewportPosition()) < distanceThreshold_)
				{
					bothTouching_ = true;
					if(friendSpacheShips_[4]->isStuck())
					{
						LogClass::Instance()->addEntry("BOTH_TOUCHING_SPACESHIP", 3, 4);
						soundManager_->playFile("laser_continu_bo", false);
					}
					
					noClock_->reset();
					noClock_->stop();

					friendSpacheShips_[4]->freeIt(elapsedTime);
				}
				else
				{
					if(bothTouching_ && friendSpacheShips_[4]->isStuck())
					{
						noClock_->reset();
					}
					if(noClock_->getTime() > 1.0f)
					{
						pico_->makeNo();

						noClock_->reset();
						noClock_->stop();
					}
					bothTouching_ = false;
				}

				if(friendSpacheShips_[0]->hasCelebrated()&&friendSpacheShips_[1]->hasCelebrated()&&friendSpacheShips_[2]->hasCelebrated()&&
				   friendSpacheShips_[3]->hasCelebrated()&&friendSpacheShips_[4]->hasCelebrated())
				{
					if(thirdLevelTimes_ < 1)
					{
						playClock_->reset();
						playLevelState_ = LEVEL_3;
						activeRay_ = RED;

						drawRays_ = true;

						friendSpacheShips_[0]->setPosition(Point(8.75f, 5.5f, 0.0f));
						friendSpacheShips_[1]->setPosition(Point(-8.75f, 4.5f, 0.0f));
						friendSpacheShips_[2]->setPosition(Point(8.75f, 4.5f, 0.0f));
						friendSpacheShips_[3]->setPosition(Point(8.75f, 4.5f, 0.0f));
						friendSpacheShips_[4]->setPosition(Point(-8.75f, 4.5f, 0.0f));

						thirdLevelTimes_++;
					}
					else
					{
						pico_->makeCelebrate();
						soundManager_->playFile("fanfare", false);

						playClock_->reset();
						LogClass::Instance()->addEntry("THIRD_LEVEL_FINISHED", 0, 0);
						playLevelState_ = CELEBRATING_LEVEL_3;
						activeRay_ = RED;

						friendSpacheShips_[0]->setPosition(Point(8.75f, 5.5f, 0.0f));
						friendSpacheShips_[1]->setPosition(Point(-8.75f, 4.5f, 0.0f));
						friendSpacheShips_[2]->setPosition(Point(8.75f, 4.5f, 0.0f));
						friendSpacheShips_[3]->setPosition(Point(8.75f, 4.5f, 0.0f));
						friendSpacheShips_[4]->setPosition(Point(-8.75f, 4.5f, 0.0f));
					}
				}

				if(friendSpacheShips_[0]->isOut()&&friendSpacheShips_[1]->isOut()&&friendSpacheShips_[2]->isOut()&&
				   friendSpacheShips_[3]->isOut()&&friendSpacheShips_[4]->isOut())
				{
					pico_->changeExpression("feliz");
					pico_->changeAnimation("celebration", 0.2f);
					drawRays_ = false;

					if(thirdLevelTimes_ < 1)
					{
						friendSpacheShips_[0]->setPosition(Point(-10.75f, 6.75f, 0.0f));
						friendSpacheShips_[1]->setPosition(Point(-10.75f, 5.75f, 0.0f));
						friendSpacheShips_[2]->setPosition(Point(-10.75f, 4.75f, 0.0f));
						friendSpacheShips_[3]->setPosition(Point(-10.75f, 3.75f, 0.0f));
						friendSpacheShips_[4]->setPosition(Point(-10.75f, 2.75f, 0.0f));
						friendSpacheShips_[0]->celebrateToPosition(Point(10.75f, 6.75f, 0.0f));
						friendSpacheShips_[1]->celebrateToPosition(Point(10.75f, 5.75f, 0.0f));
						friendSpacheShips_[2]->celebrateToPosition(Point(10.75f, 4.75f, 0.0f));
						friendSpacheShips_[3]->celebrateToPosition(Point(10.75f, 3.75f, 0.0f));
						friendSpacheShips_[4]->celebrateToPosition(Point(10.75f, 2.75f, 0.0f));
					}
					else
					{
						friendSpacheShips_[0]->setPosition(Point(10.75f, 6.75f, 0.0f));
						friendSpacheShips_[1]->setPosition(Point(10.75f, 5.75f, 0.0f));
						friendSpacheShips_[2]->setPosition(Point(10.75f, 4.75f, 0.0f));
						friendSpacheShips_[3]->setPosition(Point(10.75f, 3.75f, 0.0f));
						friendSpacheShips_[4]->setPosition(Point(10.75f, 2.75f, 0.0f));
						friendSpacheShips_[0]->celebrateToPosition(Point(-10.75f, 6.75f, 0.0f));
						friendSpacheShips_[1]->celebrateToPosition(Point(-10.75f, 5.75f, 0.0f));
						friendSpacheShips_[2]->celebrateToPosition(Point(-10.75f, 4.75f, 0.0f));
						friendSpacheShips_[3]->celebrateToPosition(Point(-10.75f, 3.75f, 0.0f));
						friendSpacheShips_[4]->celebrateToPosition(Point(-10.75f, 2.75f, 0.0f));
					}
				}
			}
			break;
	}
}

void ThirdScreenState::updatePlayingSublevel4(float elapsedTime)
{
	// Updtae ships
	for(int i = 0; i < 5; i++)
	{
		friendSpacheShips_[i]->update(elapsedTime);
	}

	// Control pointing behaviour from Pico
	if(isPointing_)
	{
		pointingClock_->tick();

		if(pointingClock_->getTime() > 2.0f)
		{
			pico_->makeRest(false);
			isPointing_ = false;
		}
	}

	// Set FriendSpaceShips to appear after 9 seconds
	if(playClock_->getTime() > 9.0f)
	{
		launchRandomFriendSpaceShip();

		playClock_->reset();
		playClock_->stop();
	}

	if(subLevel4Freed_ > 5 && friendSpacheShips_[actualFriendSpaceShip_]->isOut())
	{
		pico_->makeCelebrate();
		LogClass::Instance()->addEntry("FOURTH_LEVEL_FINISHED", 0, 0);
		soundManager_->playFile("happy", false);
		pico_->changeAnimation("DanceAss", 0.6f);

		playClock_->reset();
		//playClock_->stop();

		gameClock_->reset();

		//levelState_ = ENDING;
		playLevelState_ = CELEBRATING_LEVEL_4;
	}

	// Check whenever rays are overlaping the sips depending on color
	switch(activeRay_)
	{
		case RED:
			{
				// If it is the green, we work with friendSpaceShips 0
				if(redRay_[0]->getPointDistance(kinectHandViewPos_[0], friendSpacheShips_[actualFriendSpaceShip_]->getViewportPosition()) < distanceThreshold_ &&
				   redRay_[1]->getPointDistance(kinectHandViewPos_[1], friendSpacheShips_[actualFriendSpaceShip_]->getViewportPosition()) < distanceThreshold_)
				{
					bothTouching_ = true;
					if(friendSpacheShips_[actualFriendSpaceShip_]->isStuck())
					{
						LogClass::Instance()->addEntry("BOTH_TOUCHING_SPACESHIP", 4, 0);
						soundManager_->playFile("laser_continu_bo", false);
					}
					
					noClock_->reset();
					noClock_->stop();

					friendSpacheShips_[actualFriendSpaceShip_]->freeIt(elapsedTime);
				}
				else
				{
					if(bothTouching_ && friendSpacheShips_[actualFriendSpaceShip_]->isStuck())
					{
						noClock_->reset();
					}
					if(noClock_->getTime() > 1.0f)
					{
						pico_->makeNo();

						noClock_->reset();
						noClock_->stop();
					}
					bothTouching_ = false;
				}
				// When first ship is out but second is still waiting, make second start
				if(friendSpacheShips_[actualFriendSpaceShip_]->isOut())
				{
					launchRandomFriendSpaceShip();
					//playClock_->reset();
					//playClock_->stop();
				}
			}
			break;
		case GREEN:
			{
				// If it is the green, we work with friendSpaceShips 0
				if(greenRay_[0]->getPointDistance(kinectHandViewPos_[0], friendSpacheShips_[actualFriendSpaceShip_]->getViewportPosition()) < distanceThreshold_ &&
				   greenRay_[1]->getPointDistance(kinectHandViewPos_[1], friendSpacheShips_[actualFriendSpaceShip_]->getViewportPosition()) < distanceThreshold_)
				{
					bothTouching_ = true;
					if(friendSpacheShips_[actualFriendSpaceShip_]->isStuck())
					{
						soundManager_->playFile("laser_continu_bo", false);
					}
					
					noClock_->reset();
					noClock_->stop();

					friendSpacheShips_[actualFriendSpaceShip_]->freeIt(elapsedTime);
				}
				else
				{
					if(bothTouching_ && friendSpacheShips_[actualFriendSpaceShip_]->isStuck())
					{
						noClock_->reset();
					}
					if(noClock_->getTime() > 1.0f)
					{
						pico_->makeNo();

						noClock_->reset();
						noClock_->stop();
					}
					bothTouching_ = false;
				}
				// When first ship is out but second is still waiting, make second start
				if(friendSpacheShips_[actualFriendSpaceShip_]->isOut())
				{
					launchRandomFriendSpaceShip();
					//playClock_->reset();
					//playClock_->stop();
				}
			}
			break;
		case BLUE:
			{
				// If it is the green, we work with friendSpaceShips 0
				if(blueRay_[0]->getPointDistance(kinectHandViewPos_[0], friendSpacheShips_[actualFriendSpaceShip_]->getViewportPosition()) < distanceThreshold_ &&
				   blueRay_[1]->getPointDistance(kinectHandViewPos_[1], friendSpacheShips_[actualFriendSpaceShip_]->getViewportPosition()) < distanceThreshold_)
				{
					bothTouching_ = true;
					if(friendSpacheShips_[actualFriendSpaceShip_]->isStuck())
					{
						soundManager_->playFile("laser_continu_bo", false);
					}
					
					noClock_->reset();
					noClock_->stop();

					friendSpacheShips_[actualFriendSpaceShip_]->freeIt(elapsedTime);
				}
				else
				{
					if(bothTouching_ && friendSpacheShips_[actualFriendSpaceShip_]->isStuck())
					{
						noClock_->reset();
					}
					if(noClock_->getTime() > 1.0f)
					{
						pico_->makeNo();

						noClock_->reset();
						noClock_->stop();
					}
					bothTouching_ = false;
				}
				// When first ship is out but second is still waiting, make second start
				if(friendSpacheShips_[actualFriendSpaceShip_]->isOut())
				{
					launchRandomFriendSpaceShip();
					//playClock_->reset();
					//playClock_->stop();
				}
			}
			break;
	}
}

void ThirdScreenState::updateCelebration(float elapsedTime)
{
	for(int i = 0; i < 5; i++)
	{
		if(playClock_->getTime() < celebrationTime_-10.0f)
		{
			celebrationParticles_[i]->update(elapsedTime, true);
		}
		else
		{
			celebrationParticles_[i]->update(elapsedTime, false);
		}
	}

	if(playClock_->getTime() > 10.0f)
	{
		playClock_->reset();

		switch(playLevelState_)
		{
			case CELEBRATING_LEVEL_1:
				playLevelState_ = FADING;
				actualLevelState_ = 1;
				break;
			case CELEBRATING_LEVEL_2:
				playLevelState_ = FADING;
				actualLevelState_ = 2;
				break;
			case CELEBRATING_LEVEL_3:
				playLevelState_ = FADING;
				actualLevelState_ = 3;
				break;
			case CELEBRATING_LEVEL_4:
				playLevelState_ = FADING;
				actualLevelState_ = 4;
				break;
		}
	}
}

void ThirdScreenState::updateKinectHands()
{
	for(int i = 0; i < 2; i++)
	{
		std::stringstream kinectext;
		kinectext << "Kinect" << i+1 << ":  " << kinectHandViewPos_[i].x << "x" << kinectHandViewPos_[i].y << "  rot: " << kinectHandRot_[i];
		kinectHands_[i]->setText(kinectext.str(), graphicsManager_->getDeviceContext());
	}
}

void ThirdScreenState::updateRayInfo()
{
	for(int i = 0; i < 2; i++)
	{
		Point endHand;
		endHand.x = greenRay_[i]->getEndRayPosition().x+kinectHandViewPos_[i].x;
		endHand.y = kinectHandViewPos_[i].y-greenRay_[i]->getEndRayPosition().y;

		Point spaceShip1 = friendSpacheShips_[0]->getViewportPosition();

		int dist = sqrt(pow(endHand.x -spaceShip1.x ,2)+pow(endHand.y -spaceShip1.y ,2));

		std::stringstream raytext;
		raytext << "Ray" << i+1 << ":  " << greenRay_[i]->getPointDistance(kinectHandViewPos_[i], spaceShip1) << "  " << endHand.x  << "x" << endHand.y << "  " << spaceShip1.x << "x" << spaceShip1.y;
		rayInfo_[i]->setText(raytext.str(), graphicsManager_->getDeviceContext());
	}
}

void ThirdScreenState::drawSplashScreen()
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

void ThirdScreenState::launchRandomFriendSpaceShip()
{
	drawRays_ = true;

	if(subLevel4Freed_ < 6)
	{
		if(lastOutLeft_)
		{
			pico_->makePointing(true);
			pointingClock_->reset();
			isPointing_ = true;

			friendSpacheShips_[0]->setPosition(Point(8.5f, 5.0f, 0.0f));
			friendSpacheShips_[0]->flyToPosition(Point(0.0f, 0.0f, 0.0f));

			lastOutLeft_ = false;
		}
		else
		{
			pico_->makePointing(false);
			pointingClock_->reset();
			isPointing_ = true;

			friendSpacheShips_[0]->setPosition(Point(-8.5f, 5.0f, 0.0f));
			friendSpacheShips_[0]->flyToPosition(Point(-8.5f, 5.0f, 0.0f));

			lastOutLeft_ = true;
		}

		switch(activeRay_)
		{
		case RED:
			{
				friendSpacheShips_[0]->setType(GREEN);
				activeRay_ = GREEN;
				actualFriendSpaceShip_ = 0;
			}
			break;
		case GREEN:
			{
				friendSpacheShips_[0]->setType(BLUE);
				activeRay_ = BLUE;
				actualFriendSpaceShip_ = 0;
			}
			break;
		case BLUE:
			{
				friendSpacheShips_[0]->setType(RED);
				activeRay_ = RED;
				actualFriendSpaceShip_ = 0;
			}
			break;
		}
	}
}

void ThirdScreenState::loadConfigurationFromXML()
{
	std::string root = "./Data/configuration/level3/main_configuration.xml";

	//Loading animations XML file
	pugi::xml_document configurationDoc;
	if (!configurationDoc.load_file(root.c_str()))
	{
		MessageBoxA(NULL, "Could not load configuration .xml file!", "SecondScreen - Error", MB_ICONERROR | MB_OK);
	}

	//Searching for the initial node
	pugi::xml_node rootNode;
	if(!(rootNode = configurationDoc.child("configuration")))
	{
		MessageBoxA(NULL, "Could not load configuration node!", "SecondScreen - Error", MB_ICONERROR | MB_OK);
	}

	// Time Pico greeting
	/*pugi::xml_node greetingNode;
	if(!(greetingNode = rootNode.child("greeting_time")))
	{
		MessageBoxA(NULL, "Could not load greeting time node!", "SecondScreen - Error", MB_ICONERROR | MB_OK);
		timeGreeting_ = 5;
	}
	else
	{
		timeGreeting_ = greetingNode.text().as_float();
	}*/
}

bool ThirdScreenState::loadScenario(std::string scenario)
{
	// load background and calculate its position
	background_ = new ImageClass;
	if(!background_)
	{
		MessageBoxA(NULL, "Could not initialize the background image instance.", "SecondScreen - Error", MB_OK);
		return false;
	}

	if(!background_->setup(graphicsManager_->getDevice(), graphicsManager_->getShader2D(), screenWidth_, screenHeight_, "planet_background", screenWidth_, screenHeight_))
	{
		MessageBoxA(NULL, "Could not setup the background image.", "SecondScreen - Error", MB_OK);
		return false;
	}

	backgrounPosition_.x = (screenWidth_/2)*-1;
	backgrounPosition_.y = (screenHeight_/2)+2;

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

	// Get terrain height for posterior setup of different objects
	terrainHeight_ = 0;
	std::vector<Object3D*>::iterator it;
	for(it = scenario_.begin(); it != scenario_.end(); it++)
	{
		if((*it)->getName().find("suelociudad") != std::string::npos)
		{
			terrainHeight_ = (*it)->getPosition().y+0.1f;
		}
	}

	return true;
}

void ThirdScreenState::createScenarioObject(std::string scenario, std::string xmlName)
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

void ThirdScreenState::changeLevel(LevelState level)
{
	// Set new level
	levelState_ = level;

	// Reset clock
	gameClock_->reset();
}
