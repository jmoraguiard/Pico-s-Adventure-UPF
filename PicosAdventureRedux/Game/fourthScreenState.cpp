#ifdef _WIN32
#include <windows.h>
#endif

#include <GL/gl.h>
#include <GL/glu.h>

#include "fourthScreenState.h"

FourthScreenState FourthScreenState::fourthScreenState_;

FourthScreenState::FourthScreenState()
{
	camera_ = 0;
	light_ = 0;
	gameClock_ = 0;

	background_ = 0;

	kinectHandRot_[0] = 0.0f;
	kinectHandRot_[1] = 0.0f;

	platform_ = 0;

	for(int i = 0; i < 2; i++)
	{
		picos_[i] = 0;
	}

	for(int i = 0; i < 3; i++)
	{
		presents_[i] = 0;
	}

}

FourthScreenState::~FourthScreenState()
{

}

FourthScreenState* FourthScreenState::Instance()
{
	return (&fourthScreenState_);
}

bool FourthScreenState::setup(ApplicationManager* appManager, GraphicsManager* graphicsManager, InputManager * inputManager, KinectClass* kinectManager)
{
	// We get a pointer to the graphicsManager
	graphicsManager_ = graphicsManager;

	graphicsManager_->getScreenSize(screenWidth_, screenHeight_);

	LogClass::Instance()->setup("fourth_level_log");

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
	soundManager_ = new SoundFourthClass;
	if(!soundManager_)
	{
		return false;
	}
 
	// Initialize the sound object.
	if(!soundManager_->setup(graphicsManager_->getWindowHandler()))
	{
		MessageBox(NULL, L"Could not initialize Direct Sound.", L"SecondScreen - Error", MB_OK);
		return false;
	}
	soundManager_->playFile("background_music", true);

	// Load the first level scenario
	loadScenario("level4");

	soundManager_->playFile("background_music", true);

	// Create Pico
	picos_[0] = new PicoFourthClass();
	if(!picos_[0])
	{
		return false;
	}
	if(!picos_[0]->setup(graphicsManager_, camera_, soundManager_, 1))
	{
		MessageBoxA(NULL, "Could not initialize Pico 1 :(.", "Error", MB_ICONERROR | MB_OK);
		return false;
	}
	picos_[0]->setPosition(Point(-8.0f, terrainHeight_, -3.25f));
	inputManager->addListener(*picos_[0]);
	picos_[1] = new PicoFourthClass();
	if(!picos_[1])
	{
		return false;
	}
	if(!picos_[1]->setup(graphicsManager_, camera_, soundManager_, 2))
	{
		MessageBoxA(NULL, "Could not initialize Pico 2 :(.", "Error", MB_ICONERROR | MB_OK);
		return false;
	}
	picos_[1]->setPosition(Point(8.0f, terrainHeight_, -3.25f));
	inputManager->addListener(*picos_[1]);

	if(!setupPresents())
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

	if(!setupDebugOptions())
	{
		return false;
	}

	platform_ = new PlatformClass;
	if(!platform_)
	{
	MessageBoxA(NULL, "Could not create platform instance.", "Error", MB_ICONERROR | MB_OK);
		return false;
	}

	if(!platform_->setup(graphicsManager_, soundManager_))
	{
		MessageBoxA(NULL, "Could not setup the platform.", "Error", MB_ICONERROR | MB_OK);
		return false;
	}

	loadPresents();

	// CELEBRATIONS
	for(int i = 0; i < 5; i++)
	{
		celebrationParticles_[i] = new ParticleSystem;
		if(!celebrationParticles_[i])
		{
			MessageBoxA(NULL, "Could not create particles instance", "FourthScreen - Error", MB_ICONERROR | MB_OK);
		}
		if(celebrationParticles_[i] && !celebrationParticles_[i]->setup(graphicsManager, "star", Point(-2.0f+(1.0f*i), 4.3f, -5.75f), 4.0f, 200, 200, XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), true))
		{
			MessageBoxA(NULL, "Could not setup particles object", "FourthScreen - Error", MB_ICONERROR | MB_OK);
		}
		celebrationParticles_[i]->setParticlesVelocity(Point(0.0f, 0.8f, 0.0f), Point(1.5f, 0.3f, 0.0f));
	}
	celebrationTime_ = 15.0f;
	
	// Setup initial game state
	levelState_ = INTRODUCTION;
	introLevelState_ = ARRIBING;

	bothTouching_ = false;
	prepareNewPresent_ = false;
	waitOtherPresent_ = false;
	timeToPoint_ = 10.0f;
	picosPointing_ = false;
	picosDanced_ = false;

	goToPresent_[0] = false;
	goToPresent_[1] = false;
	goToPresent_[2] = false;

	toysFound_ = 0;

	picos_[0]->goToPosition(Point(platform_->getPosition().x-2.3f, terrainHeight_, -3.25f));
	picos_[1]->goToPosition(Point(platform_->getPosition().x+2.3f, terrainHeight_, -3.25f));

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

	noClock_ = new ClockClass();
	if(!noClock_)
	{
		return false;
	}
	noClock_->reset();
	noClock_->stop();

	return true;
}

void FourthScreenState::update(float elapsedTime)
{	
	gameClock_->tick();

	LogClass::Instance()->update();

	switch(levelState_)
	{
		case INTRODUCTION:
			{
				updateIntroduction(elapsedTime);
			}
			break;
		case PLAYING:
			{
				for(int i = 0; i< 2; i++)
				{
					blueRay_[i]->update();
					greenRay_[i]->update();
					redRay_[i]->update();
				}

				updatePlaying(elapsedTime);
			}
			break;
		case ENDING:
			{
				picos_[0]->update(elapsedTime);
				picos_[1]->update(elapsedTime);

				for(int i = 0; i < 5; i++)
				{
					celebrationParticles_[i]->update(elapsedTime, true);
				}

				if(!picosDanced_ && gameClock_->getTime() > 5.0f)
				{
					picos_[0]->makeRest(false);
					picos_[1]->makeRest(false);
					soundManager_->playBye();
					picos_[0]->changeAnimation("hola", 0.5f);
					picos_[1]->changeAnimation("hola", 0.5f);

					picosDanced_ = true;
					gameClock_->reset();
				}

				if(picosDanced_ && gameClock_->getTime() > 5.0f)
				{
					picos_[0]->makeLeave();
					picos_[1]->makeLeave();
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

void FourthScreenState::draw()
{
	XMFLOAT4X4 viewMatrix;
	camera_->getViewMatrix(viewMatrix);

	XMFLOAT4X4 projectionMatrix, worldMatrix, orthoMatrix;
	graphicsManager_->getWorldMatrix(worldMatrix);
	graphicsManager_->getProjectionMatrix(projectionMatrix);
	graphicsManager_->getOrthoMatrix(orthoMatrix);

	// Draw background
	graphicsManager_->turnZBufferOff();
		background_->draw(graphicsManager_->getDeviceContext(), backgrounPosition_.x, backgrounPosition_.y, worldMatrix, viewMatrix, orthoMatrix, XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));
	graphicsManager_->turnZBufferOn();

	// We iterate over each loaded Object to call its draw function and draw the scenario
	std::vector<Object3D*>::iterator objectsIt;
	for(objectsIt = scenario_.begin(); objectsIt != scenario_.end(); objectsIt++)
	{
		(*objectsIt)->draw(graphicsManager_->getDevice() ,graphicsManager_->getDeviceContext(), worldMatrix, viewMatrix, projectionMatrix, light_);
	}

	picos_[0]->draw(graphicsManager_, worldMatrix, viewMatrix, projectionMatrix, light_, debug_);
	picos_[1]->draw(graphicsManager_, worldMatrix, viewMatrix, projectionMatrix, light_, debug_);

	switch(levelState_)
	{
		case INTRODUCTION:
			{
				platform_->draw(graphicsManager_, worldMatrix, viewMatrix, projectionMatrix, light_, debug_);
				if(introLevelState_ == POINTING || introLevelState_ == TRANSITION)
				{
					presents_[0]->draw(graphicsManager_, worldMatrix, viewMatrix, projectionMatrix, light_, debug_);
				}
			}
			break;
		case PLAYING:
			{
				platform_->draw(graphicsManager_, worldMatrix, viewMatrix, projectionMatrix, light_, debug_);

				for(int i = 0; i < 3; i++)
				{
					presents_[i]->draw(graphicsManager_, worldMatrix, viewMatrix, projectionMatrix, light_, debug_);
				}

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

				for(int i = 0; i < toysFound_; i++)
				{
					toys_[0][i]->draw(graphicsManager_, worldMatrix, viewMatrix, projectionMatrix, light_, debug_);
					toys_[1][i]->draw(graphicsManager_, worldMatrix, viewMatrix, projectionMatrix, light_, debug_);
				}
			}
			break;
		case ENDING:
			{
				platform_->draw(graphicsManager_, worldMatrix, viewMatrix, projectionMatrix, light_, debug_);

				graphicsManager_->turnOnParticlesAlphaBlending();
				graphicsManager_->turnZBufferOff();
					for(int i = 0; i < 5; i++)
					{
						celebrationParticles_[i]->draw(graphicsManager_->getDeviceContext(), worldMatrix, viewMatrix, projectionMatrix, light_);
					}
				graphicsManager_->turnZBufferOn();
				graphicsManager_->turnOffAlphaBlending();

				for(int i = 0; i < toysFound_; i++)
				{
					toys_[0][i]->draw(graphicsManager_, worldMatrix, viewMatrix, projectionMatrix, light_, debug_);
					toys_[1][i]->draw(graphicsManager_, worldMatrix, viewMatrix, projectionMatrix, light_, debug_);
				}
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
			raysCrossingImage_->draw(graphicsManager_->getDeviceContext(), raysCrossing_.x, raysCrossing_.y, worldMatrix, viewMatrix, orthoMatrix, XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f));
		graphicsManager_->turnOffAlphaBlending();
		graphicsManager_->turnZBufferOn();
	}
}

void FourthScreenState::destroy()
{
	LogClass::Instance()->setEndTime();

	// Release the background image
	if(background_)
	{
		background_->destroy();
		delete background_;
		background_ = 0;
	}

	if(kinectHand_)
	{
		kinectHand_->destroy();
		delete kinectHand_;
		kinectHand_ = 0;
	}

	if(picos_[0])
	{
		picos_[0]->destroy();
		delete picos_[0];
		picos_[0] = 0;
	}

	if(picos_[1])
	{
		picos_[1]->destroy();
		delete picos_[1];
		picos_[1] = 0;
	}

	for(int i = 0; i < 2; i++)
	{
		if(blueRay_[i])
		{
			blueRay_[i]->destroy();
			delete blueRay_[i];
			blueRay_[i] = 0;
		}
		if(redRay_[i])
		{
			redRay_[i]->destroy();
			delete redRay_[i];
			redRay_[i] = 0;
		}
		if(greenRay_[i])
		{
			greenRay_[i]->destroy();
			delete greenRay_[i];
			greenRay_[i] = 0;
		}
	}

	for(int i = 0; i < 3; i++)
	{
		if(presents_[i])
		{
			presents_[i]->destroy();
			delete presents_[i];
			presents_[i] = 0;
		}
	}

	if(platform_)
	{
		platform_->destroy();
		delete platform_;
		platform_ = 0;
	}

	// Destroy scenario models
	std::vector<Object3D*>::iterator objectsIt;
	for(objectsIt = scenario_.begin(); objectsIt != scenario_.end(); objectsIt++)
	{
		(*objectsIt)->destroy();
	}

	// Destroy scenario models
	std::vector<ToyClass*>::iterator toys1It;
	for(toys1It = toys_[0].begin(); toys1It != toys_[0].end(); toys1It++)
	{
		(*toys1It)->destroy();
	}

	// Destroy scenario models
	std::vector<ToyClass*>::iterator toys2It;
	for(toys2It = toys_[1].begin(); toys2It != toys_[1].end(); toys2It++)
	{
		(*toys2It)->destroy();
	}
}

void FourthScreenState::notify(InputManager* notifier, InputStruct arg)
{
	switch(arg.keyPressed){
		case 49: //1
		case 50: //2
		case 51: //3
			{
				goToPresent_[arg.keyPressed-49] = !goToPresent_[arg.keyPressed-49];
			}
			break;
		case 68: //D
		case 100: //d
			{
				debug_ = !debug_;
			}
			break;
		case 83: //S
		case 115: //s
			{
				if(introLevelState_ == GREETING_CHILDREN)
				{
					introLevelState_ = GREETING;
				}
			}
			break;
		case 32: // Space
			{
				switch(levelState_)
				{
					case PLAYING:
						{
							for(int i = 0; i < 3; i++)
							{
								if(presents_[i]->isInPlatform())
								{
									openPresent(i);
								}
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

void FourthScreenState::notify(KinectClass* notifier, KinectStruct arg)
{
	kinectHandPreviousRot_[0] = kinectHandRot_[0];
	kinectHandPreviousRot_[1] = kinectHandRot_[1];

	if(arg.type == FIRST_RIGHT_HAND_ROT)
	{
		// Calculate viewport position
		//kinectHandViewPos_[0] = Point(arg.handPos.x*screenWidth_/320, arg.handPos.y*screenHeight_/240);
		kinectHandViewPos_[0] = Point(arg.handPos.x, arg.handPos.y);

		// Calculate the last position of the ray
		rayEndViewPos_[0] = Point(1024*cos(arg.handRot), 1024*sin(arg.handRot));

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

		// Calculate the last position of the ray
		rayEndViewPos_[1] = Point(1024*cos(arg.handRot), 1024*sin(arg.handRot));

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

bool FourthScreenState::setupDebugOptions()
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

bool FourthScreenState::setupKinectHands()
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

bool FourthScreenState::setupPresents()
{
	for(int i = 0; i < 3; i++)
	{
		presents_[i] = new PresentClass;
		if(!presents_[i])
		{
			MessageBoxA(NULL, "Could not create friend spaceship instance.", "Error", MB_ICONERROR | MB_OK);
			return false;
		}

		if(!presents_[i]->setup(graphicsManager_, soundManager_, "regalo"))
		{
			MessageBoxA(NULL, "Could not setup the firend spaceship.", "Error", MB_ICONERROR | MB_OK);
			return false;
		}

		presents_[i]->setPosition(Point(0.0f, 0.0f, 0.0f));
	}
}

bool FourthScreenState::setupRayInfo()
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

bool FourthScreenState::setupRays()
{
	raysCrossing_ = Point(-1, -1);
	distanceThreshold_ = 120.0f;

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

	raysCrossingImage_ = new ImageClass;
	if(!raysCrossingImage_)
	{
		MessageBoxA(NULL, "Could not initialize the kinectHand_ image instance.", "FreindSpaceShip - Error", MB_OK);
		return false;
	}

	if(!raysCrossingImage_->setup(graphicsManager_->getDevice(), graphicsManager_->getShader2D(), screenWidth_, screenHeight_, "star", 40, 40))
	{
		MessageBoxA(NULL, "Could not setup the kinectHand_ image.", "FreindSpaceShip - Error", MB_OK);
		return false;
	}

	return true;
}

bool FourthScreenState::setupSplashScreen()
{
	// load splashscreen and calculate its position
	splashScreen_ = new ImageClass;
	if(!splashScreen_)
	{
		MessageBoxA(NULL, "Could not initialize the splashscreen image instance.", "SecondScreen - Error", MB_OK);
		return false;
	}

	if(!splashScreen_->setup(graphicsManager_->getDevice(), graphicsManager_->getShader2D(), screenWidth_, screenHeight_, "splashscreen4", screenWidth_, screenHeight_))
	{
		MessageBoxA(NULL, "Could not setup the splashscreen image.", "SecondScreen - Error", MB_OK);
		return false;
	}

	return true;
}

void FourthScreenState::updateIntroduction(float elapsedTime)
{
	introClock_->tick();

	picos_[0]->update(elapsedTime+elapsedTime*0.015);
	picos_[1]->update(elapsedTime-elapsedTime*0.015);

	switch(introLevelState_)
	{
		case ARRIBING:
			{
				// We check whether or not Picos are next to the platform
				if(picos_[0]->checkPicoArrivedObjective() && picos_[1]->checkPicoArrivedObjective())
				{
					picos_[0]->makeRest(false);
					picos_[1]->makeRest(false);

					picos_[0]->makeGreeting();
					picos_[1]->makeGreeting();

					introLevelState_ = PICO_NEXT_PLATFORM;
					gameClock_->reset();
				}
			}
			break;
		case PICO_NEXT_PLATFORM:
			{
				//if(picos_[0]->checkPicoArrivedObjective() && picos_[1]->checkPicoArrivedObjective())
				//{
				// If next to the platform, greet between them and start turning to face the user
				
				if(gameClock_->getTime() > 2.0f)
				{
					picos_[0]->makeRest(false);
					picos_[1]->makeRest(false);

					introLevelState_ = TURNING;
					introClock_->reset();
				}
				//}
			}
			break;
		case TURNING:
			{
				
				if(picos_[0]->lookAtCamera(true) || picos_[1]->lookAtCamera(true))
				{
					picos_[0]->makeRest(true);
					picos_[1]->makeRest(true);

					kinectClass_->setUserColor(XMFLOAT4(1.0f, 1.0f, 1.0f, 0.5f));

					picos_[0]->makeGreeting();
					picos_[1]->makeGreeting();

					introLevelState_ = GREETING_CHILDREN;
					introClock_->reset();
				}
			}
			break;
		case GREETING_CHILDREN:
			{
				
			}
			break;
		case GREETING:
			{
				if(introClock_->getTime() > 1.0f)
				{
					picos_[0]->goToPosition(Point(platform_->getPosition().x-0.95f, platform_->getPosition().y+0.36f, -3.25f));
					picos_[1]->goToPosition(Point(platform_->getPosition().x+0.95f, platform_->getPosition().y+0.36f, -3.25f));

					introLevelState_ = PICO_ON_PLATFORM;
				}
			}
			break;
		case PICO_ON_PLATFORM:
			{
				if(picos_[0]->checkPicoArrivedObjective() && picos_[1]->checkPicoArrivedObjective())
				{
					if(picos_[0]->lookAtCamera(true) && picos_[1]->lookAtCamera(true))
					{
						picos_[0]->makeRest(false);
						picos_[1]->makeRest(false);

						introLevelState_ = PRESENT_APPEARING;
					}
				}
			}
			break;
		case PRESENT_APPEARING:
			{
				preparePresent(presents_[0], Point(7.0f, 4.5f, 4.5f), RED);
				drawRays_ = false;

				picos_[0]->makePointing(true);
				picos_[1]->makePointing(true);

				introLevelState_ = POINTING;
				introClock_->reset();
			}
			break;
		case POINTING:
			{
				presents_[0]->update(elapsedTime);

				if(introClock_->getTime() > 2.0f)
				{
					picos_[0]->makeRest(false);
					picos_[1]->makeRest(false);

					introLevelState_ = TRANSITION;
				}
			}
			break;
		case TRANSITION:
			{
				presents_[0]->update(elapsedTime);

				drawRays_ = true;
				levelState_ = PLAYING;
				playLevelState_ = LEVEL_1;
			}
			break;
		default:
			{

			}
			break;
	}
}

void FourthScreenState::updateKinectHands()
{
	for(int i = 0; i < 2; i++)
	{
		std::stringstream kinectext;
		kinectext << "Kinect" << i+1 << ":  " << kinectHandViewPos_[i].x << "x" << kinectHandViewPos_[i].y << "  rot: " << kinectHandRot_[i];
		kinectHands_[i]->setText(kinectext.str(), graphicsManager_->getDeviceContext());
	}
}

void FourthScreenState::updatePlaying(float elapsedTime)
{	
	playClock_->tick();
	noClock_->tick();

	// Update rays crossing position
	updateRayCrossing();

	picos_[0]->setPosition(Point(platform_->getPosition().x-0.95f, platform_->getPosition().y+0.36f, platform_->getPosition().z));
	picos_[1]->setPosition(Point(platform_->getPosition().x+0.95f, platform_->getPosition().y+0.36f, platform_->getPosition().z));

	picos_[0]->update(elapsedTime+elapsedTime*0.015);
	picos_[1]->update(elapsedTime-elapsedTime*0.015);

	for(int i = 0; i < toysFound_; i++)
	{
		toys_[0][i]->update(elapsedTime);
		toys_[1][i]->update(elapsedTime);
	}

	platform_->update(elapsedTime);

	switch(playLevelState_)
	{
		case LEVEL_1:
			{
				updatePlayingSublevel1(elapsedTime);
			}
			break;
		case TRANSITION_1_2:
			{
				if(playClock_->getTime() > 9.0f)
				{
					preparePresent(presents_[0], Point(-1.25f, 3.3f, 5.0f), RED);
					preparePresent(presents_[1], Point(-6.0f, 4.5f, 4.75f), GREEN);

					picos_[0]->makePointing(false);
					picos_[1]->makePointing(false);

					activeRay_ = RED;
					drawRays_ = true;

					playLevelState_ = LEVEL_2;
				}
			}
			break;
		case LEVEL_2:
			{
				updatePlayingSublevel2(elapsedTime);
			}
			break;
		case TRANSITION_2_3:
			{
				if(playClock_->getTime() > 9.0f)
				{
					preparePresent(presents_[0], Point(-5.0f, 4.4f, 5.0f), RED);
					preparePresent(presents_[1], Point(-2.5f, 3.4f, 5.25f), GREEN);
					preparePresent(presents_[2], Point(5.5f, 4.4f, 5.25f), BLUE);

					picos_[0]->makePointing(true);
					picos_[1]->makePointing(false);

					activeRay_ = RED;
					drawRays_ = true;

					playLevelState_ = LEVEL_3;
				}
			}
			break;
		case LEVEL_3:
			{
				updatePlayingSublevel3(elapsedTime);
			}
			break;
		default:
			{

			}
			break;
	}

	for(int i = 0; i < 3; i++)
	{
		if(goToPresent_[i] && drawRays_)
		{
			presentTouched(i);
		}
	}
}

void FourthScreenState::updatePlayingSublevel1(float elapsedTime)
{
	for(int i = 0; i < 1; i++)
	{
		presents_[i]->update(elapsedTime);

		if(!presents_[i]->isInPlatform())
		{
			if(drawRays_ && (presents_[i]->getType() == activeRay_))
			{
				// If rays are active, check which color we should do
				switch(presents_[i]->getType())
				{
					case RED:
						{
							if(redRay_[0]->getPointDistance(kinectHandViewPos_[0], presents_[i]->getViewportPosition()) < distanceThreshold_ &&
							   redRay_[1]->getPointDistance(kinectHandViewPos_[1], presents_[i]->getViewportPosition()) < distanceThreshold_)
							{
								presentTouched(i);
							}
							else
							{
								if(bothTouching_ && presents_[i]->isInBuilding())
								{
									noClock_->reset();
								}
								bothTouching_ = false;
							}
						}
						break;
				}
			}
		}
		else
		{
			presents_[i]->setPosition(Point(platform_->getPosition().x, platform_->getPosition().y+0.3, platform_->getPosition().z));

			presents_[i]->setRotationY(presents_[i]->getRotationY() + 0.1f);
			if(presents_[i]->getRotationY() > XM_PI)
			{
				presents_[i]->setRotationY(presents_[i]->getRotationY() - XM_2PI);
			}

			playClock_->reset();

			// If present is in platform, we wait for the platfmor to arrive
			if(drawRays_)
			{
				if(platform_->isWaiting())
				{
					drawRays_ = false;
				}
			}
			// If we are not drawing rays is that the platform has arrive
			else
			{
				// Check whether players are touching the present or not
				if(presents_[i]->getCollisionSphere()->testIntersection(camera_, kinectHandViewPos_[0].x, kinectHandViewPos_[0].y) &&
				   presents_[i]->getCollisionSphere()->testIntersection(camera_, kinectHandViewPos_[1].x, kinectHandViewPos_[1].y))
				{
					if(presents_[i]->isInPlatform() && platform_->isWaiting())
					{
						openPresent(i);
					}
				}
			}
		}
	}

	// When the two presents have been found, we can prepare the new present
	if(toysFound_ > 0 && (toys_[0][toysFound_-1]->isInPile() || toys_[1][toysFound_-1]->isInPile()))
	{
		switch(toysFound_)
		{
			case 1:
				{
					if(prepareNewPresent_)
					{
						picos_[0]->makePointing(false);
						picos_[1]->makePointing(false);

						preparePresent(presents_[0], Point(-2.25f, 3.3f, 4.75f), RED);
					}
				}
				break;
			case 2:
				{
					if(prepareNewPresent_)
					{
						picos_[0]->makePointing(false);
						picos_[1]->makePointing(false);

						preparePresent(presents_[0], Point(-5.0f, 2.9f, 5.0f), RED);
					}
				}
				break;
			case 3:
				{
					soundManager_->playFile("fanfare", false);

					drawRays_ = false;

					playLevelState_ = TRANSITION_1_2;
					playClock_->reset();
				}
				break;
		}
	}

	// When the present has finally appeared
	if(presents_[0]->isInBuilding())
	{
		if(!makeRestAfterPresentAppeared_)
		{
			picos_[0]->makeRest(false);
			picos_[1]->makeRest(false);
			makeRestAfterPresentAppeared_ = true;
			playClock_->reset();
		}

		// If inactivity time is higher than the threshold, point or not
		if(playClock_->getTime() > timeToPoint_)
		{
			if(!picosPointing_)
			{
				picos_[0]->makePointing(false);
				picos_[1]->makePointing(true);
				picosPointing_ = true;
				playClock_->reset();
			}
			else
			{
				picos_[0]->makeRest(false);
				picos_[1]->makeRest(false);
				picosPointing_ = false;
				playClock_->reset();
			}
		}
	}

	// If both users stop touching the present, Picos get sad
	if(!bothTouching_)
	{
		if(noClock_->getTime() > 1.0f)
		{
			picos_[0]->makeNo();
			picos_[1]->makeNo();

			LogClass::Instance()->addEntry("BOTH_NOT_TOUCHING_PRESENT", 0, toysFound_);

			noClock_->reset();
			noClock_->stop();
		}
	}
}

void FourthScreenState::updatePlayingSublevel2(float elapsedTime)
{
	for(int i = 0; i < 2; i++)
	{
		presents_[i]->update(elapsedTime);

		if(!presents_[i]->isInPlatform())
		{
			if(drawRays_ && (activeRay_ == presents_[i]->getType()))
			{
				// If rays are active, check which color we should do
				switch(presents_[i]->getType())
				{
					case RED:
						{
							if(redRay_[0]->getPointDistance(kinectHandViewPos_[0], presents_[i]->getViewportPosition()) < distanceThreshold_ &&
							   redRay_[1]->getPointDistance(kinectHandViewPos_[1], presents_[i]->getViewportPosition()) < distanceThreshold_)
							{
								presentTouched(i);
							}
							else
							{
								if(bothTouching_ && presents_[i]->isInBuilding())
								{
									noClock_->reset();
								}
								bothTouching_ = false;
							}
						}
						break;
					case GREEN:
						{
							if(greenRay_[0]->getPointDistance(kinectHandViewPos_[0], presents_[i]->getViewportPosition()) < distanceThreshold_ &&
							   greenRay_[1]->getPointDistance(kinectHandViewPos_[1], presents_[i]->getViewportPosition()) < distanceThreshold_)
							{
								presentTouched(i);
							}
							else
							{
								if(bothTouching_ && presents_[i]->isInBuilding())
								{
									noClock_->reset();
								}
								bothTouching_ = false;
							}
						}
						break;
				}
			}
		}
		else
		{
			presents_[i]->setPosition(Point(platform_->getPosition().x, platform_->getPosition().y+0.3, platform_->getPosition().z));

			presents_[i]->setRotationY(presents_[i]->getRotationY() + 0.1f);
			if(presents_[i]->getRotationY() > XM_PI)
			{
				presents_[i]->setRotationY(presents_[i]->getRotationY() - XM_2PI);
			}

			playClock_->reset();

			// If present is in platform, we wait for the platfmor to arrive
			if(drawRays_)
			{
				if(platform_->isWaiting())
				{
					drawRays_ = false;
				}
			}
			// If we are not drawing rays is that the platform has arrive
			else
			{
				// Check whether players are touching the present or not
				if(presents_[i]->getCollisionSphere()->testIntersection(camera_, kinectHandViewPos_[0].x, kinectHandViewPos_[0].y) &&
				   presents_[i]->getCollisionSphere()->testIntersection(camera_, kinectHandViewPos_[1].x, kinectHandViewPos_[1].y))
				{
					if(presents_[i]->isInPlatform() && platform_->isWaiting())
					{
						openPresent(i);
					}
				}
			}
		}
	}

	// When the two presents have been found, we can prepare the new present
	if(toysFound_ > 0 && (toys_[0][toysFound_-1]->isInPile() || toys_[1][toysFound_-1]->isInPile()))
	{
		switch(toysFound_)
		{
			case 4:
				{
					if(prepareNewPresent_)
					{
						activeRay_ = GREEN;
						drawRays_ = true;
						prepareNewPresent_ = false;
					}
				}
				break;
			case 5:
				{
					if(prepareNewPresent_)
					{
						picos_[0]->makePointing(false);
						picos_[1]->makePointing(true);

						preparePresent(presents_[0], Point(7.0f, 4.4f, 4.5f), RED);
						preparePresent(presents_[1], Point(-5.0f, 3.0f, 5.0f), GREEN);
					}
				}
				break;
			case 6:
				{
					if(prepareNewPresent_)
					{
						activeRay_ = RED;
						drawRays_ = true;
						prepareNewPresent_ = false;
					}
				}
				break;
			case 7:
				{
					if(prepareNewPresent_)
					{
						picos_[0]->makePointing(true);
						picos_[1]->makePointing(false);

						preparePresent(presents_[1], Point(-1.25f, 3.3f, 5.0f), GREEN);
						preparePresent(presents_[0], Point(5.5f, 2.8f, 5.25f), RED);
					}
				}
				break;
			case 8:
				{
					if(prepareNewPresent_)
					{
						activeRay_ = GREEN;
						drawRays_ = true;
						prepareNewPresent_ = false;
					}
				}
				break;
			case 9:
				{
					soundManager_->playFile("fanfare", false);

					drawRays_ = false;

					playLevelState_ = TRANSITION_2_3;
					playClock_->reset();
				}
				break;
		}
	}

	// When the present has finally appeared
	if(presents_[0]->isInBuilding() || presents_[1]->isInBuilding())
	{
		if(!makeRestAfterPresentAppeared_)
		{
			picos_[0]->makeRest(false);
			picos_[1]->makeRest(false);
			makeRestAfterPresentAppeared_ = true;
			playClock_->reset();
		}

		// If inactivity time is higher than the threshold, point or not
		if(playClock_->getTime() > timeToPoint_)
		{
			if(!picosPointing_)
			{
				picos_[0]->makePointing(false);
				picos_[1]->makePointing(true);
				picosPointing_ = true;
				playClock_->reset();
			}
			else
			{
				picos_[0]->makeRest(false);
				picos_[1]->makeRest(false);
				picosPointing_ = false;
				playClock_->reset();
			}
		}
	}

	// If both users stop touching the present, Picos get sad
	if(!bothTouching_)
	{
		if(noClock_->getTime() > 1.0f)
		{
			picos_[0]->makeNo();
			picos_[1]->makeNo();

			LogClass::Instance()->addEntry("BOTH_NOT_TOUCHING_PRESENT", 1, toysFound_);

			noClock_->reset();
			noClock_->stop();
		}
	}
}

void FourthScreenState::updatePlayingSublevel3(float elapsedTime)
{
	for(int i = 0; i < 3; i++)
	{
		presents_[i]->update(elapsedTime);

		if(!presents_[i]->isInPlatform())
		{
			if(drawRays_ && (activeRay_ == presents_[i]->getType()))
			{
				// If rays are active, check which color we should do
				switch(presents_[i]->getType())
				{
					case RED:
						{
							if(redRay_[0]->getPointDistance(kinectHandViewPos_[0], presents_[i]->getViewportPosition()) < distanceThreshold_ &&
							   redRay_[1]->getPointDistance(kinectHandViewPos_[1], presents_[i]->getViewportPosition()) < distanceThreshold_)
							{
								presentTouched(i);
							}
							else
							{
								if(bothTouching_ && presents_[i]->isInBuilding())
								{
									noClock_->reset();
								}
								bothTouching_ = false;
							}
						}
						break;
					case GREEN:
						{
							if(greenRay_[0]->getPointDistance(kinectHandViewPos_[0], presents_[i]->getViewportPosition()) < distanceThreshold_ &&
							   greenRay_[1]->getPointDistance(kinectHandViewPos_[1], presents_[i]->getViewportPosition()) < distanceThreshold_)
							{
								presentTouched(i);
							}
							else
							{
								if(bothTouching_ && presents_[i]->isInBuilding())
								{
									noClock_->reset();
								}
								bothTouching_ = false;
							}
						}
						break;
					case BLUE:
						{
							if(blueRay_[0]->getPointDistance(kinectHandViewPos_[0], presents_[i]->getViewportPosition()) < distanceThreshold_ &&
							   blueRay_[1]->getPointDistance(kinectHandViewPos_[1], presents_[i]->getViewportPosition()) < distanceThreshold_)
							{
								presentTouched(i);
							}
							else
							{
								if(bothTouching_ && presents_[i]->isInBuilding())
								{
									noClock_->reset();
								}
								bothTouching_ = false;
							}
						}
						break;
				}
			}
		}
		else
		{
			presents_[i]->setPosition(Point(platform_->getPosition().x, platform_->getPosition().y+0.3, platform_->getPosition().z));

			presents_[i]->setRotationY(presents_[i]->getRotationY() + 0.1f);
			if(presents_[i]->getRotationY() > XM_PI)
			{
				presents_[i]->setRotationY(presents_[i]->getRotationY() - XM_2PI);
			}

			playClock_->reset();

			// If present is in platform, we wait for the platfmor to arrive
			if(drawRays_)
			{
				if(platform_->isWaiting())
				{
					drawRays_ = false;
				}
			}
			// If we are not drawing rays is that the platform has arrive
			else
			{
				// Check whether players are touching the present or not
				if(presents_[i]->getCollisionSphere()->testIntersection(camera_, kinectHandViewPos_[0].x, kinectHandViewPos_[0].y) &&
				   presents_[i]->getCollisionSphere()->testIntersection(camera_, kinectHandViewPos_[1].x, kinectHandViewPos_[1].y))
				{
					if(presents_[i]->isInPlatform() && platform_->isWaiting())
					{
						openPresent(i);
					}
				}
			}
		}
	}

	// When the two presents have been found, we can prepare the new present
	if(toysFound_ > 0 && (toys_[0][toysFound_-1]->isInPile() || toys_[1][toysFound_-1]->isInPile()))
	{
		switch(toysFound_)
		{
			case 10:
				{
					if(prepareNewPresent_)
					{
						activeRay_ = BLUE;
						drawRays_ = true;
						prepareNewPresent_ = false;
					}
				}
				break;
			case 11:
				{
					if(prepareNewPresent_)
					{
						activeRay_ = GREEN;
						drawRays_ = true;
						prepareNewPresent_ = false;
					}
				}
				break;
			case 12:
				{
					if(prepareNewPresent_)
					{
						picos_[0]->makePointing(true);
						picos_[1]->makePointing(false);

						preparePresent(presents_[0], Point(7.0f, 4.5f, 4.5f), RED);
						preparePresent(presents_[1], Point(-3.25f, 3.3f, 5.25f), GREEN);
						preparePresent(presents_[2], Point(-7.0f, 4.3f, 5.0f), BLUE);
					}
				}
				break;
			case 13:
				{
					if(prepareNewPresent_)
					{
						activeRay_ = RED;
						drawRays_ = true;
						prepareNewPresent_ = false;
					}
				}
				break;
			case 14:
				{
					if(prepareNewPresent_)
					{
						activeRay_ = GREEN;
						drawRays_ = true;
						prepareNewPresent_ = false;
					}
				}
				break;
			case 15:
				{
					soundManager_->playFile("happy", false);
					picos_[0]->changeAnimation("DanceAss", 0.6f);
					picos_[1]->changeAnimation("DanceAss", 0.6f);

					drawRays_ = false;

					levelState_ = ENDING;
					gameClock_->reset();
				}
				break;
		}
	}

	// When the present has finally appeared
	if(presents_[0]->isInBuilding() || presents_[1]->isInBuilding() || presents_[2]->isInBuilding())
	{
		if(!makeRestAfterPresentAppeared_)
		{
			picos_[0]->makeRest(false);
			picos_[1]->makeRest(false);
			makeRestAfterPresentAppeared_ = true;
			playClock_->reset();
		}

		// If inactivity time is higher than the threshold, point or not
		if(playClock_->getTime() > timeToPoint_)
		{
			if(!picosPointing_)
			{
				picos_[0]->makePointing(false);
				picos_[1]->makePointing(true);
				picosPointing_ = true;
				playClock_->reset();
			}
			else
			{
				picos_[0]->makeRest(false);
				picos_[1]->makeRest(false);
				picosPointing_ = false;
				playClock_->reset();
			}
		}
	}

	// If both users stop touching the present, Picos get sad
	if(!bothTouching_)
	{
		if(noClock_->getTime() > 1.0f)
		{
			picos_[0]->makeNo();
			picos_[1]->makeNo();

			LogClass::Instance()->addEntry("BOTH_NOT_TOUCHING_PRESENT", 2, toysFound_);

			noClock_->reset();
			noClock_->stop();
		}
	}
}

void FourthScreenState::updateRayCrossing()
{
	raysCrossing_ = intersection(kinectHandViewPos_[0], rayEndViewPos_[0],
								 kinectHandViewPos_[1], rayEndViewPos_[1]);
}

void FourthScreenState::updateRayInfo()
{
	for(int i = 0; i < 2; i++)
	{
		Point endHand;
		endHand.x = greenRay_[i]->getEndRayPosition().x+kinectHandViewPos_[i].x;
		endHand.y = kinectHandViewPos_[i].y-greenRay_[i]->getEndRayPosition().y;
	}
}

void FourthScreenState::drawSplashScreen()
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

void FourthScreenState::openPresent(int i)
{
	presents_[i]->open();

	playClock_->reset();

	bothTouching_ = false;
	noClock_->reset();
	noClock_->stop();

	picos_[0]->makeCelebrate();
	picos_[1]->makeCelebrate();

	toys_[0][toysFound_]->setInitialPosition(Point(presents_[i]->getPosition().x, presents_[i]->getPosition().y+0.6f, presents_[i]->getPosition().z-1.0f));
	toys_[1][toysFound_]->setInitialPosition(Point(presents_[i]->getPosition().x, presents_[i]->getPosition().y+0.6f, presents_[i]->getPosition().z-1.0f));

	toys_[0][toysFound_]->makeItAppear();
	toys_[1][toysFound_]->makeItAppear();

	toysFound_++;
	prepareNewPresent_ = true;

	switch(playLevelState_)
	{
		case LEVEL_1:
			{
				LogClass::Instance()->addEntry("BOTH_OPEN_PRESENT", 0, toysFound_);
			}
			break;
		case LEVEL_2:
			{
				LogClass::Instance()->addEntry("BOTH_OPEN_PRESENT", 1, toysFound_);
			}
			break;
		case LEVEL_3:
			{
				LogClass::Instance()->addEntry("BOTH_OPEN_PRESENT", 2, toysFound_);
			}
			break;
	}
}

void FourthScreenState::presentTouched(int i)
{
	if(presents_[i]->isInBuilding())
	{
		playClock_->reset();

		bothTouching_ = true;

		picos_[0]->makeRest(false);
		picos_[1]->makeRest(false);

		presents_[i]->setTouched();
		platform_->goToPosition(presents_[i]->getPosition());

		if(platform_->isGettingPresent())
		{
			presents_[i]->setInPlatform();
			platform_->bringBackPresent();

			picos_[0]->makeCelebrate();
			picos_[1]->makeCelebrate();

			drawRays_ = false;
		}

		noClock_->reset();
		noClock_->stop();

		switch(playLevelState_)
		{
			case LEVEL_1:
				{
					LogClass::Instance()->addEntry("BOTH_TOUCHING_PRESENT", 0, toysFound_);
				}
				break;
			case LEVEL_2:
				{
					LogClass::Instance()->addEntry("BOTH_TOUCHING_PRESENT", 1, toysFound_);
				}
				break;
			case LEVEL_3:
				{
					LogClass::Instance()->addEntry("BOTH_TOUCHING_PRESENT", 2, toysFound_);
				}
				break;
		}
	}
}

void FourthScreenState::preparePresent(PresentClass* present, Point position, DirtColor type)
{
	present->setInitialPosition(position);
	present->setType(type);

	activeRay_ = type;
	drawRays_ = true;

	prepareNewPresent_ = false;
	makeRestAfterPresentAppeared_ = false;
}

Point FourthScreenState::intersection(Point p1, Point p2, Point p3, Point p4)
{
	// Store the values for fast access and easy
	// equations-to-code conversion
	float x1 = p1.x, x2 = p2.x, x3 = p3.x, x4 = p4.x;
	float y1 = p1.y, y2 = p2.y, y3 = p3.y, y4 = p4.y;
 
	float d = (x1 - x2) * (y3 - y4) - (y1 - y2) * (x3 - x4);
	// If d is zero, there is no intersection
	if (d == 0)
	{
		return Point(-1, -1);
	}
 
	// Get the x and y
	float pre = (x1*y2 - y1*x2), post = (x3*y4 - y3*x4);
	float x = ( pre * (x3 - x4) - (x1 - x2) * post ) / d;
	float y = ( pre * (y3 - y4) - (y1 - y2) * post ) / d;
 
	// Check if the x and y coordinates are within both lines
	if ( x < min(x1, x2) || x > max(x1, x2) || x < min(x3, x4) || x > max(x3, x4) ) 
	{
		return Point(-1, -1);
	}
	if ( y < min(y1, y2) || y > max(y1, y2) || y < min(y3, y4) || y > max(y3, y4) )
	{
		return Point(-1, -1);
	}
 
	// Return the point of intersection
	Point ret;
	ret.x = x;
	ret.y = y;
	return ret;
}

void FourthScreenState::loadConfigurationFromXML()
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

bool FourthScreenState::loadScenario(std::string scenario)
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

void FourthScreenState::createScenarioObject(std::string scenario, std::string xmlName)
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

bool FourthScreenState::loadPresents()
{
	
	std::stringstream root;
	root << "./Data/configuration/level4/presents/presents.xml";

	//Loading animations XML file
	pugi::xml_document fruitsDoc;
	if(!fruitsDoc.load_file(root.str().c_str()))
	{
		std::stringstream error;
		error << "Could not load object .xml file!" << root.str();
		MessageBoxA(NULL, error.str().c_str(), "FourthScreen - Presents - Error", MB_ICONERROR | MB_OK);
		return false;
	}

	pugi::xml_node root_node;
	// Get initial node
	if(!(root_node = fruitsDoc.child("presents")))
	{
		MessageBoxA(NULL, "Could not find the fruits root node.", "FourthScreen - Presents - Error", MB_ICONERROR | MB_OK);
		return false;
	}

	for(pugi::xml_node fruitNode = root_node.first_child(); fruitNode; fruitNode = fruitNode.next_sibling())
	{
		std::string node_name = fruitNode.name();
		// Actuamos en consecuencia segun el tipo de nodo
		if(node_name ==  "present")
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

			//std::stringstream toLoad;
			//toLoad << "Loading... " << modelName.as_string();
			//MessageBoxA(NULL, toLoad.str().c_str(), "ToLoad", MB_OK);

			ToyClass* toy1 = new ToyClass();
			if(!toy1)
			{
				return false;
			}
			if(!toy1->setup(graphicsManager_, soundManager_, modelName.as_string(), pos, scale, rotX, rotY, rotZ))
			{
				MessageBoxA(NULL, "Could not initialize fruit.", "Visualizer - Fruit - Error", MB_ICONERROR | MB_OK);
				return false;
			}
			toys_[0].push_back(toy1);

			pos.x = -pos.x;

			ToyClass* toy2 = new ToyClass();
			if(!toy2)
			{
				return false;
			}
			if(!toy2->setup(graphicsManager_, soundManager_, modelName.as_string(), pos, scale, rotX, rotY, rotZ))
			{
				MessageBoxA(NULL, "Could not initialize fruit.", "Visualizer - Fruit - Error", MB_ICONERROR | MB_OK);
				return false;
			}
			toys_[1].push_back(toy2);
		}
	}

	return true;
}

void FourthScreenState::changeLevel(LevelState level)
{
	// Set new level
	levelState_ = level;

	// Reset clock
	gameClock_->reset();
}
