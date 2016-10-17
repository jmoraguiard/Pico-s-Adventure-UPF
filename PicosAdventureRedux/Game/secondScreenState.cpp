#ifdef _WIN32
#include <windows.h>
#endif

#include <GL/gl.h>
#include <GL/glu.h>

#include "secondScreenState.h"

SecondScreenState SecondScreenState::secondScreenState_;

SecondScreenState::SecondScreenState()
{
	camera_ = 0;
	light_ = 0;
	gameClock_ = 0;

	spaceShip_ = 0;
	pico_ = 0;

	background_ = 0;

	for(int i = 0; i < 3; i++)
	{
		initialPositions_[i].x = 0.0f;
		initialPositions_[i].y = 0.0f;
		initialPositions_[i].z = 0.0f;

		finalPositions_[i].x = 0.0f;
		finalPositions_[i].y = 0.0f;
		finalPositions_[i].z = 0.0f;
	}
}

SecondScreenState::~SecondScreenState()
{

}

SecondScreenState* SecondScreenState::Instance()
{
	return (&secondScreenState_);
}

bool SecondScreenState::setup(ApplicationManager* appManager, GraphicsManager* graphicsManager, InputManager * inputManager, KinectClass* kinectManager)
{
	// We get a pointer to the graphicsManager
	graphicsManager_ = graphicsManager;

	graphicsManager_->getScreenSize(screenWidth_, screenHeight_);

	LogClass::Instance()->setup("second_level_log");

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
	light_->setAmbientColor(0.32f, 0.32f, 0.32f, 1.0f);
	light_->setDiffuseColor(0.0f, 0.0f, 0.0f, 1.0f);
	light_->setDirection(0.0f, -0.5f, 1.0f);

	// Create the light object.
	piecesLight_ = new LightClass;
	if(!piecesLight_)
	{
		return false;
	}

	// Initialize the light object.
	piecesLight_->setAmbientColor(0.2f, 0.2f, 0.3f, 1.0f);
	piecesLight_->setDiffuseColor(0.2f, 0.2f, 0.3f, 1.0f);
	piecesLight_->setDirection(0.0f, -1.0f, 1.0f);

	// SOUND
	soundManager_ = new SoundSecondClass;
	if(!soundManager_)
	{
		return false;
	}
 
	// Initialize the sound object.
	if(!soundManager_->setup(graphicsManager_->getWindowHandler()))
	{
		MessageBoxA(NULL, "Could not initialize Direct Sound.", "SecondScreen - Error", MB_OK);
		return false;
	}

	// load background and calculate its position
	background_ = new ImageClass;
	if(!background_)
	{
		MessageBoxA(NULL, "Could not initialize the background image instance.", "SecondScreen - Error", MB_OK);
		return false;
	}

	if(!background_->setup(graphicsManager_->getDevice(), graphicsManager_->getShader2D(), screenWidth_, screenHeight_, "night_background", screenWidth_, screenHeight_))
	{
		MessageBoxA(NULL, "Could not setup the background image.", "SecondScreen - Error", MB_OK);
		return false;
	}

	background2_ = new ImageClass;
	if(!background2_)
	{
		MessageBoxA(NULL, "Could not initialize the background image instance.", "SecondScreen - Error", MB_OK);
		return false;
	}

	if(!background2_->setup(graphicsManager_->getDevice(), graphicsManager_->getShader2D(), screenWidth_, screenHeight_, "sky_background", screenWidth_, screenHeight_))
	{
		MessageBoxA(NULL, "Could not setup the background image.", "SecondScreen - Error", MB_OK);
		return false;
	}

	backgrounPosition_.x = (screenWidth_/2)*-1;
	backgrounPosition_.y = (screenHeight_/2)+2;

	// Lights
	lightPos_[0] = XMFLOAT4(-3.5f, 2.0f, -4.0f, 0.0f);
	lightPos_[1] = XMFLOAT4(-30.5f, 20.0f, -4.0f, 0.0f);
	lightPos_[2] = XMFLOAT4(-30.5f, 20.0f, -4.0f, 0.0f);
	lightPos_[3] = XMFLOAT4(-30.5f, 20.0f, -4.0f, 0.0f);
	lightPos_[4] = XMFLOAT4(-30.5f, 20.0f, -4.0f, 0.0f);

	lightBody_ = Object3DFactory::Instance()->CreateObject3D("StaticObject3D", graphicsManager_, "sphere");
	lightBody_->setPosition(Point(-4.0f, 0.0f, -3.25f));
	lightBody_->setScale(Vector(0.02f, 0.02f, 0.02f));

	lightParticles_[0] = new ParticleSystem;
	if(!lightParticles_[0])
	{
		MessageBoxA(NULL, "Could not create light1 particles instance", "SecondScreen - Error", MB_ICONERROR | MB_OK);
	}
	if(lightParticles_[0] && !lightParticles_[0]->setup(graphicsManager, "star", Point(lightPos_[1].x, lightPos_[1].y, lightPos_[1].z) , 2.8, 30, 60, XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f), true))
	{
		MessageBoxA(NULL, "Could not setup light1 particles object", "SecondScreen - Error", MB_ICONERROR | MB_OK);
	}
	lightParticles_[0]->setParticleSize(0.08f);

	lightParticles_[1] = new ParticleSystem;
	if(!lightParticles_[1])
	{
		MessageBoxA(NULL, "Could not create light2 particles instance", "SecondScreen - Error", MB_ICONERROR | MB_OK);
	}
	if(lightParticles_[1] && !lightParticles_[1]->setup(graphicsManager, "star", Point(lightPos_[2].x, lightPos_[2].y, lightPos_[2].z) , 2.8, 30, 60, XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f), true))
	{
		MessageBoxA(NULL, "Could not setup light2 particles object", "SecondScreen - Error", MB_ICONERROR | MB_OK);
	}
	lightParticles_[1]->setParticleSize(0.08f);

	lightParticles_[2] = new ParticleSystem;
	if(!lightParticles_[2])
	{
		MessageBoxA(NULL, "Could not create light3 particles instance", "SecondScreen - Error", MB_ICONERROR | MB_OK);
	}
	if(lightParticles_[2] && !lightParticles_[2]->setup(graphicsManager, "star", Point(lightPos_[3].x, lightPos_[3].y, lightPos_[3].z) , 2.8, 30, 60, XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f), true))
	{
		MessageBoxA(NULL, "Could not setup light3 particles object", "SecondScreen - Error", MB_ICONERROR | MB_OK);
	}
	lightParticles_[2]->setParticleSize(0.08f);

	lightParticles_[3] = new ParticleSystem;
	if(!lightParticles_[3])
	{
		MessageBoxA(NULL, "Could not create light4 particles instance", "SecondScreen - Error", MB_ICONERROR | MB_OK);
	}
	if(lightParticles_[3] && !lightParticles_[3]->setup(graphicsManager, "star", Point(lightPos_[4].x, lightPos_[4].y, lightPos_[4].z) , 2.8, 30, 60, XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f), true))
	{
		MessageBoxA(NULL, "Could not setup light4 particles object", "SecondScreen - Error", MB_ICONERROR | MB_OK);
	}
	lightParticles_[3]->setParticleSize(0.08f);

	// Load the first level scenario
	loadScenario("level2");

	// Get terrain height for posterior setup of different objects
	terrainHeight_ = 0;
	std::vector<Object3D*>::iterator it;
	for(it = scenario_.begin(); it != scenario_.end(); it++)
	{
		if((*it)->getName().find("terreno") != std::string::npos)
		{
			terrainHeight_ = (*it)->getPosition().y+0.1f;
		}
	}
	
	// Create the space ship.
	spaceShip_ = new SpaceShipClass;
	if(!spaceShip_)
	{
		return false;
	}
	if(!spaceShip_->setup(graphicsManager_, soundManager_, 2.0f))
	{
		MessageBoxA(NULL, "Could not initialize the SpaceShip.", "Error", MB_ICONERROR | MB_OK);
		return false;
	}
	Shader3DClass* shaderTemp = Shader3DFactory::Instance()->CreateShader3D("PointlightDiffuseShader3D", graphicsManager_);
	spaceShip_->getObject()->setShader3D(shaderTemp);
	PointlightDiffuseShader3DClass* pointlightShader = dynamic_cast<PointlightDiffuseShader3DClass*>(spaceShip_->getObject()->getShader3D());
	pointlightShader->setPositions(lightPos_[0], lightPos_[1], lightPos_[2], lightPos_[3], lightPos_[4]);
	spaceShip_->setInitialPosition(Point(-4.0f, terrainHeight_+0.001f, -3.25f));
	spaceShip_->setPosition(Point(-4.0f, terrainHeight_+0.001f, -3.25f));
	spaceShip_->setScale(Vector(0.0115476f, 0.0110476f, 0.0115476f));
	spaceShip_->setRotation(0.0f, 0.0f, XM_PI/2);
	spaceShip_->setFloorHeight(terrainHeight_);

	spaceShipLevel_ = 1;

	// Create Pico
	pico_ = new PicoSecondClass();
	if(!pico_)
	{
		return false;
	}
	if(!pico_->setup(graphicsManager_, camera_, soundManager_))
	{
		MessageBoxA(NULL, "Could not initialize Pico :(.", "Error", MB_ICONERROR | MB_OK);
		return false;
	}
	pico_->setSpaceShipPosition(spaceShip_->getPosition());
	inputManager->addListener(*pico_);

	// Create the light object for Pico.
	picoLight_ = new LightClass;
	if(!picoLight_)
	{
		return false;
	}

	// Initialize the light object for Pico.
	picoLight_->setAmbientColor(0.1f, 0.1f, 0.1f, 1.0f);
	picoLight_->setDiffuseColor(0.8f, 0.8f, 0.8f, 1.0f);
	picoLight_->setDirection(0.0f, -0.5f, 1.0f);

	// Load spaceship pieces to collect
	loadPieces();

	childSaysHi_ = false;

	fadeTime_ = 5.0f;

	// Stars
	loadStars();
	timeToWaitFirstStar_ = 8.0f;

	starLevel_ = 1;
	starsFalling_ = false;
	starFallTime_ = 8.0f;
	betweenStarsTime_ = 10.0f;

	initialPositions_[0].x = -2.1f;
	initialPositions_[0].y = 8.2f;
	initialPositions_[0].z = -3.0f;

	initialPositions_[1].x = 0.0f;
	initialPositions_[1].y = 8.2f;
	initialPositions_[1].z = -3.0f;

	initialPositions_[2].x = 2.1f;
	initialPositions_[2].y = 8.2f;
	initialPositions_[2].z = -3.0f;

	finalPositions_[0].x = -2.1f;
	finalPositions_[0].y = terrainHeight_;
	finalPositions_[0].z = -3.5f;

	finalPositions_[1].x = 0.0f;
	finalPositions_[1].y = terrainHeight_;
	finalPositions_[1].z = -3.5f;

	finalPositions_[2].x = 2.1f;
	finalPositions_[2].y = terrainHeight_;
	finalPositions_[2].z = -3.5f;

	setupGUI(graphicsManager, inputManager, kinectManager);

	starTouched_ = false;

	starTouchedClock_ = new ClockClass();
	if(!starTouchedClock_)
	{
		return false;
	}
	starTouchedClock_->reset();

	// Set the level state to the introduction, and it to greetings
	levelState_ = INTRODUCTION;
	introLevelState_ = WAIT_INIT_MOVEMENT;
	starsIntroLevelState_ = FIRST_STAR;

	kinectTracksSmbdy_ = false;

	// Setup Pico at the initial state
	hasPicoGreeted_ = false;
	
	loadConfigurationFromXML();

	soundManager_->playFile("background_music", true);

	// Set up game clock and debug
	introClock_ = new ClockClass();
	if(!introClock_)
	{
		return false;
	}
	introClock_->reset();

	debug_ = false;

	FPS_ = new TextClass();
	if(!FPS_)
	{
		return false;
	}

	// Initialize the text object.
	if(!FPS_->setup(graphicsManager->getDevice(), graphicsManager->getDeviceContext(), graphicsManager->getShader2D(), screenWidth_, screenHeight_, 20, 20, "FPS: "))
	{
		MessageBoxA(NULL, "Could not initialize the FPS text object.", "SecondScreen - Error", MB_OK);
		return false;
	}

	kinectHands_ = new TextClass();
	if(!kinectHands_)
	{
		return false;
	}

	// Initialize the text object.
	if(!kinectHands_->setup(graphicsManager->getDevice(), graphicsManager->getDeviceContext(), graphicsManager->getShader2D(), screenWidth_, screenHeight_, 20, 60, "Kinect: "))
	{
		MessageBoxA(NULL, "Could not initialize the Kinect text object.", "SecondScreen - Error", MB_OK);
		return false;
	}

	lightPositions_ = new TextClass();
	if(!lightPositions_)
	{
		return false;
	}

	// Initialize the text object.
	if(!lightPositions_->setup(graphicsManager->getDevice(), graphicsManager->getDeviceContext(), graphicsManager->getShader2D(), screenWidth_, screenHeight_, 20, 80, "FPS: "))
	{
		MessageBoxA(NULL, "Could not initialize the lights text object.", "SecondScreen - Error", MB_OK);
		return false;
	}

	// Setup clock at the end so it starts when we run
	gameClock_ = new ClockClass();
	if(!gameClock_)
	{
		return false;
	}
	gameClock_->reset();

	inputManager->addListener(*this);

	// Change kinect settings to make user being drawn darker
	kinectClass_ = kinectManager;

	kinectHand_ = new ImageClass;
	if(!kinectHand_)
	{
		MessageBoxA(NULL, "Could not initialize the kinectHand_ image instance.", "SecondScreen - Error", MB_OK);
		return false;
	}

	if(!kinectHand_->setup(graphicsManager_->getDevice(), graphicsManager_->getShader2D(), screenWidth_, screenHeight_, "hand", 30, 30))
	{
		MessageBoxA(NULL, "Could not setup the kinectHand_ image.", "SecondScreen - Error", MB_OK);
		return false;
	}

	kinectClass_->addListener(*this);
	kinectClass_->setUserColor(XMFLOAT4(0.25f, 0.25f, 0.35f, 0.5f));

	return true;
}

void SecondScreenState::update(float elapsedTime)
{	
	gameClock_->tick();

	LogClass::Instance()->update();

	// Update light positions
	std::vector<Object3D*>::iterator objectsIt;
	for(objectsIt = scenario_.begin(); objectsIt != scenario_.end(); objectsIt++)
	{
		PointlightDiffuseShader3DClass* pointlightShader = dynamic_cast<PointlightDiffuseShader3DClass*>((*objectsIt)->getShader3D());
		pointlightShader->setPositions(lightPos_[0], lightPos_[1], lightPos_[2], lightPos_[3], lightPos_[4]);
	}
	
	if(levelState_ != SELECT_POLAROID)
	{
		pico_->update(elapsedTime);
	}

	switch(levelState_)
	{
		case INTRODUCTION:
			{
				PointlightDiffuseShader3DClass* pointlightShader = dynamic_cast<PointlightDiffuseShader3DClass*>(spaceShip_->getObject()->getShader3D());
				pointlightShader->setPositions(lightPos_[0], lightPos_[1], lightPos_[2], lightPos_[3], lightPos_[4]);

				if(introLevelState_ != WAIT_INIT_MOVEMENT && introLevelState_ != TO_NIGHT)
				{
					updatePieces(elapsedTime);
				}

				updateIntroduction(elapsedTime);
			}
			break;
		case MOUNTING:
			{
				updateLightPositions();
				updateParticlesPositions();

				PointlightDiffuseShader3DClass* pointlightShader = dynamic_cast<PointlightDiffuseShader3DClass*>(spaceShip_->getObject()->getShader3D());
				pointlightShader->setPositions(lightPos_[0], lightPos_[1], lightPos_[2], lightPos_[3], lightPos_[4]);

				lightParticles_[0]->update(elapsedTime, true);
				lightParticles_[1]->update(elapsedTime, true);
				lightParticles_[2]->update(elapsedTime, true);
				lightParticles_[3]->update(elapsedTime, true);

				updatePieces(elapsedTime);

				// Check if the spaceship has been mounted
				bool allPiecesInPlace = true;
				std::vector<PieceClass*>::iterator pieceIt;
				for(pieceIt = pieces_.begin(); pieceIt != pieces_.end(); pieceIt++)
				{
					if(!(*pieceIt)->getInFinalPosition())
					{
						allPiecesInPlace = false;
					}
				}

				// When all pieces are in place and pico is done
				if(allPiecesInPlace && pico_->isPicoWaiting())
				{
					// Make Pico celebrate it!
					pico_->makeRest(false);
					pico_->makeDanceAss();
					soundManager_->playFile("spaceship_finished", false);

					LogClass::Instance()->addEntry("SPACESHIP_FINNISHED", 0, 0);

					Shader3DClass *diffuseShader3DTemp = Shader3DFactory::Instance()->CreateShader3D("DiffuseShader3D", graphicsManager_);
					spaceShip_->getObject()->setShader3D(diffuseShader3DTemp);

					levelState_ = PRE_FADING;
					gameClock_->reset();
				}
			}
			break;
		case PRE_FADING:
			{
				if(gameClock_->getTime() > 10.0f)
				{
					levelState_ = FADING;
					gameClock_->reset();
				}
			}
			break;
		case FADING:
			{
				/*float diffuseTemp = 1.0f - gameClock_->getTime()/fadeTime_;
				light_->setDiffuseColor(diffuseTemp, diffuseTemp, diffuseTemp, 1.0f);*/

				soundManager_->playFile("change_level", false);

				float ambientTemp = 0.04f - (gameClock_->getTime()/fadeTime_)*0.035f;
				light_->setAmbientColor(ambientTemp, ambientTemp, ambientTemp, 1.0f);

				if(gameClock_->getTime() > fadeTime_)
				{
					clearPolaroids();
					createPolaroids();
					levelState_ = SELECT_POLAROID;
					gameClock_->stop();
				}
			}
			break;
		case SELECT_POLAROID:
			{
				soundManager_->playFile("change_level", false);

				polaroidGUI_->update(elapsedTime);
			}
			break;
		case TRANSITION:
			{
				updateLightPositions();

				//lightParticles_[0]->update(elapsedTime, false);

				if(gameClock_->getTime() > timeToWaitFirstStar_)
				{
					makeFirstStarFall();
					pico_->makePointing();

					levelState_ = INTRO_COLLECTING;
				}
			}
			break;
		case INTRO_COLLECTING:
			{
				updateLightPositions();

				//lightParticles_[0]->update(elapsedTime, false);

				star_->update(elapsedTime);
				spaceShip_->update(elapsedTime);

				updateIntroCollecting(elapsedTime);
			}
			break;
		case COLLECTING:
			{
				updateLightPositions();

				lightParticles_[0]->setPosition(Point(kinectHoldWorldPos_.x, kinectHoldWorldPos_.y, kinectHoldWorldPos_.z));
				lightParticles_[0]->update(elapsedTime, true);
				lightParticles_[1]->setPosition(Point(kinectHoldWorldPos_.x, kinectHoldWorldPos_.y, kinectHoldWorldPos_.z));
				lightParticles_[1]->update(elapsedTime, true);

				starTouchedClock_->tick();
				spaceShip_->update(elapsedTime);

				star_->update(elapsedTime);

				if(!starsFalling_)
				{
					if(gameClock_->getTime() > starFallTime_)
					{
						origins_.clear();
						endings_.clear();

						LogClass::Instance()->addEntry("MAKE_STARS_FALL", starLevel_, 0);

						makeStarFall();

						gameClock_->reset();
					}
				}
				else
				{
					if(gameClock_->getTime() > betweenStarsTime_)
					{
						
						gameClock_->reset();
					}
				}

				if(starTouched_ && starTouchedClock_->getTime() > 8.0f)
				{
					spaceShip_->makeLaunch(spaceShipLevel_);
					pico_->makeCelebrate();
					spaceShipLevel_++;
					starLevel_++;

					if(starLevel_ > 3)
					{
						starLevel_ = 3;
					}

					starTouched_ = false;
					starTouchedClock_->reset();

					if(spaceShipLevel_ > 5)
					{
						spaceShip_->makeBig(Point(-4.5f, terrainHeight_+0.001f, -3.25f), Vector(0.03f, 0.03f, 0.03f));
						levelState_ = ENDING;
					}
				}
			}
			break;
		case ENDING:
			{
				spaceShip_->update(elapsedTime);

				if(spaceShip_->isPrepared())
				{
					pico_->goToPosition(Point(spaceShip_->getPosition().x-1.0f, terrainHeight_, spaceShip_->getPosition().z));
					kinectClass_->setUserColor(XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f));
				}
			}
			break;
		default:
			{
				
			}
			break;
	}

	std::stringstream FPSText;
	FPSText << "FPS: " << 1/elapsedTime;
	FPS_->setText(FPSText.str(), graphicsManager_->getDeviceContext());

	std::stringstream kinectext;
	kinectext << "Kinect: " << kinectHandWorldPos_[0].x << "x" << kinectHandWorldPos_[0].y << " " << kinectHandWorldPos_[1].x << "x" << kinectHandWorldPos_[1].y;
	kinectHands_->setText(kinectext.str(), graphicsManager_->getDeviceContext());

	std::stringstream lightstext;
	lightstext << "Light: " << lightPos_[0].x << "x" << lightPos_[0].y << " " << lightPos_[1].x << "x" << lightPos_[1].y << " " << lightPos_[2].x << "x" << lightPos_[2].y<< " " << lightPos_[3].x << "x" << lightPos_[3].y << " " << lightPos_[4].x << "x" << lightPos_[4].y;
	lightPositions_->setText(lightstext.str(), graphicsManager_->getDeviceContext());
}

void SecondScreenState::draw()
{
	XMFLOAT4X4 viewMatrix;
	camera_->getViewMatrix(viewMatrix);

	XMFLOAT4X4 projectionMatrix, worldMatrix, orthoMatrix;
	graphicsManager_->getWorldMatrix(worldMatrix);
	graphicsManager_->getProjectionMatrix(projectionMatrix);
	graphicsManager_->getOrthoMatrix(orthoMatrix);

	// Despite not being a drawing method, we call it here to reuse matrices
	updatePicoScreenposition(worldMatrix, viewMatrix, projectionMatrix);

	if(introLevelState_ == TO_NIGHT || introLevelState_ == WAIT_INIT_MOVEMENT)
	{
		graphicsManager_->turnOnAlphaBlending();
		graphicsManager_->turnZBufferOff();
			XMFLOAT4 temp = XMFLOAT4(light_->getAmbientColor().x+0.2, light_->getAmbientColor().y+0.2, light_->getAmbientColor().z+0.2, 1.0f);
			background2_->draw(graphicsManager_->getDeviceContext(), backgrounPosition_.x, backgrounPosition_.y, worldMatrix, viewMatrix, orthoMatrix, temp);
		graphicsManager_->turnZBufferOn();
		graphicsManager_->turnOffAlphaBlending();
	}
	else
	{
		graphicsManager_->turnZBufferOff();
			background_->draw(graphicsManager_->getDeviceContext(), backgrounPosition_.x, backgrounPosition_.y, worldMatrix, viewMatrix, orthoMatrix, XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));
		graphicsManager_->turnZBufferOn();
	}

	// We iterate over each loaded Object to call its draw function and draw the scenario
	std::vector<Object3D*>::iterator objectsIt;
	for(objectsIt = scenario_.begin(); objectsIt != scenario_.end(); objectsIt++)
	{
		(*objectsIt)->draw(graphicsManager_->getDevice() ,graphicsManager_->getDeviceContext(), worldMatrix, viewMatrix, projectionMatrix, light_);
	}

	switch(levelState_)
	{
		case INTRODUCTION:
		case MOUNTING:
			{
				graphicsManager_->turnOnWireframeRasterizer();
					spaceShip_->draw(graphicsManager_, worldMatrix, viewMatrix, projectionMatrix, picoLight_, debug_);
				graphicsManager_->turnOnSolidRasterizer();

				// Draw the loaded pieces
				std::vector<PieceClass*>::iterator pieceIt;
				for(pieceIt = pieces_.begin(); pieceIt != pieces_.end(); pieceIt++)
				{
					(*pieceIt)->draw(graphicsManager_, worldMatrix, viewMatrix, projectionMatrix, piecesLight_, debug_);
				}

				if(introLevelState_ == GIVE_POWER)
				{
					lightBody_->draw(graphicsManager_->getDevice() ,graphicsManager_->getDeviceContext(), worldMatrix, viewMatrix, projectionMatrix, picoLight_);
				}

				// Draw light Particles
				graphicsManager_->turnOnParticlesAlphaBlending();
				graphicsManager_->turnZBufferOff();
					lightParticles_[0]->draw(graphicsManager_->getDeviceContext(), worldMatrix, viewMatrix, projectionMatrix, picoLight_);
					lightParticles_[1]->draw(graphicsManager_->getDeviceContext(), worldMatrix, viewMatrix, projectionMatrix, picoLight_);
					lightParticles_[2]->draw(graphicsManager_->getDeviceContext(), worldMatrix, viewMatrix, projectionMatrix, picoLight_);
					lightParticles_[3]->draw(graphicsManager_->getDeviceContext(), worldMatrix, viewMatrix, projectionMatrix, picoLight_);
				graphicsManager_->turnZBufferOn();
				graphicsManager_->turnOffAlphaBlending();
			}
			break;
		case PRE_FADING:
		case FADING:
			{
				graphicsManager_->turnOnWireframeRasterizer();
					spaceShip_->draw(graphicsManager_, worldMatrix, viewMatrix, projectionMatrix, picoLight_, debug_);
				graphicsManager_->turnOnSolidRasterizer();
			}
			break;
		case SELECT_POLAROID:
			{
				graphicsManager_->turnOnWireframeRasterizer();
					spaceShip_->draw(graphicsManager_, worldMatrix, viewMatrix, projectionMatrix, picoLight_, debug_);
				graphicsManager_->turnOnSolidRasterizer();

				graphicsManager_->turnZBufferOff();
				graphicsManager_->turnOnAlphaBlending();
					polaroidGUI_->draw(graphicsManager_->getDeviceContext(), worldMatrix, viewMatrix, orthoMatrix);
				graphicsManager_->turnOffAlphaBlending();
				graphicsManager_->turnZBufferOn();
			}
			break;
		case COLLECTING:
			{
				graphicsManager_->turnOnParticlesAlphaBlending();
				graphicsManager_->turnZBufferOff();
					lightParticles_[0]->draw(graphicsManager_->getDeviceContext(), worldMatrix, viewMatrix, projectionMatrix, picoLight_);
				graphicsManager_->turnZBufferOn();
				graphicsManager_->turnOffAlphaBlending();
			}
		case TRANSITION:
		case INTRO_COLLECTING:
			{
				star_->draw(graphicsManager_, worldMatrix, viewMatrix, projectionMatrix, picoLight_, debug_);

				spaceShip_->draw(graphicsManager_, worldMatrix, viewMatrix, projectionMatrix, picoLight_, debug_);
			}
			break;
		case ENDING:
			{
				spaceShip_->draw(graphicsManager_, worldMatrix, viewMatrix, projectionMatrix, picoLight_, debug_);
			}
			break;
		default:
			{
				
			}
			break;
	}

	// Draw pico
	pico_->draw(graphicsManager_, worldMatrix, viewMatrix, projectionMatrix, picoLight_, debug_);

	if(debug_)
	{
		graphicsManager_->turnZBufferOff();
		graphicsManager_->turnOnAlphaBlending();
			FPS_->draw(graphicsManager_->getDeviceContext(), worldMatrix, viewMatrix, orthoMatrix);
			kinectHands_->draw(graphicsManager_->getDeviceContext(), worldMatrix, viewMatrix, orthoMatrix);
			lightPositions_->draw(graphicsManager_->getDeviceContext(), worldMatrix, viewMatrix, orthoMatrix);
			kinectHand_->draw(graphicsManager_->getDeviceContext(), ((screenWidth_/2)*-1)+kinectHandViewPos_[0].x, ((screenHeight_/2))-kinectHandViewPos_[0].y, worldMatrix, viewMatrix, orthoMatrix, XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f));
			kinectHand_->draw(graphicsManager_->getDeviceContext(), ((screenWidth_/2)*-1)+kinectHandViewPos_[1].x, ((screenHeight_/2))-kinectHandViewPos_[1].y, worldMatrix, viewMatrix, orthoMatrix, XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f));
			kinectHand_->draw(graphicsManager_->getDeviceContext(), ((screenWidth_/2)*-1)+kinectHoldViewPos_.x, ((screenHeight_/2))-kinectHoldViewPos_.y, worldMatrix, viewMatrix, orthoMatrix, XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));
		graphicsManager_->turnOffAlphaBlending();
		graphicsManager_->turnZBufferOn();
	}
}

void SecondScreenState::destroy()
{
	LogClass::Instance()->setEndTime();

	if(spaceShip_)
	{
		spaceShip_->destroy();
		delete spaceShip_;
		spaceShip_ = 0;
	}

	if(lightBody_)
	{
		lightBody_->destroy();
		delete lightBody_;
		lightBody_ = 0;
	}

	// Release the background image
	if(background_)
	{
		background_->destroy();
		delete background_;
		background_ = 0;
	}
	
	if(star_)
	{
		star_->destroy();
		delete star_;
		star_ = 0;
	}

	std::vector<Object3D*>::iterator objectsIt;
	for(objectsIt = scenario_.begin(); objectsIt != scenario_.end(); objectsIt++)
	{
		(*objectsIt)->destroy();
	}

	std::vector<PieceClass*>::iterator pieceIt;
	for(pieceIt = pieces_.begin(); pieceIt != pieces_.end(); pieceIt++)
	{
		(*pieceIt)->destroy();
	}
}

void SecondScreenState::updatePicoScreenposition(XMFLOAT4X4 worldMatrix, XMFLOAT4X4 viewMatrix, XMFLOAT4X4 projectionMatrix)
{
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

	picoScreenPos_.x = resultPos.x;
	picoScreenPos_.y = resultPos.y;
}

void SecondScreenState::updateIntroduction(float elapsedTime)
{
	introClock_->tick();

	if(introLevelState_ != GIVE_POWER)
	{
		lightPos_[0].x = pico_->getPosition().x;
		lightPos_[0].y = pico_->getPosition().y+1.5;
		lightPos_[0].z = pico_->getPosition().z-1.0f;
	}

	switch(introLevelState_)
	{
		case WAIT_INIT_MOVEMENT:
			{
				pico_->makeRest(false);

				if(kinectTracksSmbdy_)
				{
					introLevelState_ = TO_NIGHT;
					introClock_->reset();
				}
			}
			break;
		case TO_NIGHT:
			{
				float lightToNight = 0.32f-0.28f*(introClock_->getTime()/timeToNight_);
				light_->setAmbientColor(lightToNight, lightToNight, lightToNight, 1.0f);

				pico_->makeRest(false);

				// When greeting time has passed we make Pico go next to the piece with a little margin
				if(introClock_->getTime() > timeToNight_)
				{
					light_->setAmbientColor(0.04f, 0.04f, 0.04f, 1.0f);

					pico_->goToPosition(Point(4.0f, 0.0f, -3.0f));

					introLevelState_ = PICO_ENTERING;
					introClock_->reset();
				}
			}
			break;
		case PICO_ENTERING:
			{
				// Check whether Pico arrived or not to the piece
				if(pico_->checkPicoArrivedObjective())
				{
					pico_->makeRest(false);

					if(!hasPicoGreeted_)
					{
						pico_->makeGreeting();
						hasPicoGreeted_ = true;
						introClock_->reset();
					}

					introLevelState_ = GREETING;
					introClock_->reset();
				}
			}
			break;
		case GREETING:
			{
				// When greeting time has passed we make Pico go next to the piece with a little margin
				if(introClock_->getTime() > 2.0f)
				{
					introLevelState_ = WAIT_HI;
				}
			}
			break;
		case WAIT_HI:
			{
				if(childSaysHi_)
				{
					Point positionFirstPiece = pieces_.at(0)->getPosition();
					positionFirstPiece.x += 0.7f;
					pico_->goToPosition(positionFirstPiece);

					introLevelState_ = TO_PIECE;
					childSaysHi_ = false;
					introClock_->reset();
				}
				else
				{
					if(introClock_->getTime() > timeGreeting_)
					{
						pico_->makeGreeting();
						introClock_->reset();
					}
				}
			}
			break;
		case TO_PIECE:
			{
				// Check whether Pico arrived or not to the piece
				if(pico_->checkPicoArrivedObjective())
				{
					// When arrive we make him surprised and light the piece
					pico_->makeRest(false);
					pico_->changeExpression("sorpresa");
					soundManager_->playSurprise();
					introLevelState_ = LIGHT_IT;
				}
			}
			break;
		case LIGHT_IT:
			{
				pieces_.at(0)->lightIt();
				pieces_.at(0)->update(elapsedTime);

				// When lighted we make Pico drag it until the spaceship
				if(pieces_.at(0)->hasFallen())
				{
					Point positionSpaceship = spaceShip_->getPosition(); //spaceShipObject_->getPosition();
					positionSpaceship.x += 1.2f;
					pico_->goToPosition(positionSpaceship);
					introLevelState_ = DRAG_IT;
				}
			}
			break;
		case DRAG_IT:
			{
				// Put the piece in front of Pico to simulate that he is dragging it
				Point newPiecePosition = pico_->getPosition();
				newPiecePosition.x -= 0.7f;
				pieces_.at(0)->setPosition(newPiecePosition);

				// When arrived to the SpaceShip, place piece on final position and make Pico work
				if(pico_->checkPicoArrivedObjective())
				{
					pieces_.at(0)->setFinalPosition();
					pico_->makeWork();
					introLevelState_ = WORK;
				}
			}
			break;
		case WORK:
			{
				if(pico_->isPicoWaiting())
				{
					// Not until we want users to start playing with the pieces we activate the kinect notifier
					introLevelState_ = GIVE_POWER;

					lightPos_[1] = lightPos_[0];
					/*lightPos_[1] = lightPos_[0];
					lightPos_[1] = lightPos_[0];*/
				}
			}
			break;
		case GIVE_POWER:
			{
				pico_->makeRest(false);
				if(powerToUser(elapsedTime))
				{
					introLevelState_ = POWER_GIVEN;
					levelState_ = MOUNTING;
					kinectClass_->setUserColor(XMFLOAT4(0.75f, 0.75f, 0.85f, 0.5f));
					pico_->makeCelebrate();
				}
			}
			break;
		default:
			{

			}
			break;
	}
}

void SecondScreenState::updateIntroCollecting(float elapsedTime)
{
	switch(starsIntroLevelState_)
	{
		case FIRST_STAR:
			{
				if(star_->isInTheFloor() || star_->isInTheSky())
				{
					//make the spaceship simulate some kind of reaction
					spaceShip_->makeLaunch(1);

					starsIntroLevelState_ = FIRST_REACTION;
				}
			}
			break;
		case FIRST_REACTION:
			{
				if(star_->isInTheFloor() || star_->isInTheSky())
				{
					pico_->makeRest(false);
					pico_->makeCelebrate();

					starsIntroLevelState_ = PICO_GUIDING;
				}
			}
			break;
		case PICO_GUIDING:
			{
				levelState_ = COLLECTING;
				gameClock_->reset();
			}
			break;
		default:
			{

			}
			break;
	}
}

void SecondScreenState::updatePieces(float elapsedTime)
{
	// Update pieces
	std::vector<PieceClass*>::iterator pieceIt;
	for(pieceIt = pieces_.begin(); pieceIt != pieces_.end(); pieceIt++)
	{
		(*pieceIt)->update(elapsedTime);
	}
}

void SecondScreenState::updateLightPositions()
{
	lightPos_[0].x = pico_->getPosition().x;
	lightPos_[0].y = pico_->getPosition().y+1.5;
	lightPos_[0].z = pico_->getPosition().z-1.0f;

	lightPos_[1].x = kinectHandWorldPos_[0].x;
	lightPos_[1].y = kinectHandWorldPos_[0].y;

	lightPos_[2].x = kinectHandWorldPos_[1].x;
	lightPos_[2].y = kinectHandWorldPos_[1].y;

	/*lightPos_[3].x = kinectHandWorldPos_[1].x;
	lightPos_[3].y = kinectHandWorldPos_[1].y;

	lightPos_[4].x = kinectHandWorldPos_[1].x;
	lightPos_[4].y = kinectHandWorldPos_[1].y;*/
}

void SecondScreenState::updateParticlesPositions()
{
	lightParticles_[0]->setPosition(Point(lightPos_[1].x, lightPos_[1].y, lightPos_[1].z));
	lightParticles_[1]->setPosition(Point(lightPos_[2].x, lightPos_[2].y, lightPos_[2].z));
	lightParticles_[2]->setPosition(Point(lightPos_[3].x, lightPos_[3].y, lightPos_[3].z));
	lightParticles_[3]->setPosition(Point(lightPos_[4].x, lightPos_[4].y, lightPos_[4].z));
}

void SecondScreenState::makeFirstStarFall()
{
	star_->setInitialPosition(Point(-2.5f, 8.2f, -1.0f));
	star_->setFinalPosition(spaceShip_->getPosition());
	star_->makeItFall(true);
}

void SecondScreenState::makeStarFall()
{
	int pos = rand() % 3;
	bool keepSearching = true;
	while(keepSearching)
	{
		bool notIn = true;
		std::vector<int>::iterator originIt;
		for(originIt = origins_.begin(); originIt != origins_.end(); originIt++)
		{
			if((*originIt) == pos)
			{
				notIn = false;
			}
		}

		if(notIn)
		{
			keepSearching = false;
			origins_.push_back(pos);
		}
		else
		{
			pos = rand() % 3;
		}
	}
	star_->setInitialPosition(initialPositions_[pos]);
	star_->setFinalPosition(finalPositions_[pos]);

	if(starsOrder_[starIndex_])
	{
		star_->makeItFall(true);
	}
	else
	{
		star_->makeItFall(false);
	}

	starIndex_++;

	if(starIndex_ > 19)
	{
		starIndex_ = 0;
	}
}

void SecondScreenState::makeShipIgnite(int level)
{

}

void SecondScreenState::notify(InputManager* notifier, InputStruct arg)
{
	switch(arg.keyPressed){
		case 49:
			{
				LogClass::Instance()->addEntry("KEYBOARD_FOUND_PIECE", 8, 0);
				pieces_.at(8)->makeItFall();
				if(levelState_ == SELECT_POLAROID)
				{
					if(arg.keyPressed-49 < polaroidFrame_->getNumberButtons())
					{
						polaroidFrame_->selectButton(polaroidsName_.at(arg.keyPressed-49));
					}
				}
			}
			break;
		case 50:
			{
				LogClass::Instance()->addEntry("KEYBOARD_FOUND_PIECE", 4, 0);
				pieces_.at(4)->makeItFall();
				if(levelState_ == SELECT_POLAROID)
				{
					if(arg.keyPressed-49 < polaroidFrame_->getNumberButtons())
					{
						polaroidFrame_->selectButton(polaroidsName_.at(arg.keyPressed-49));
					}
				}
			}
			break;
		case 51:
			{
				LogClass::Instance()->addEntry("KEYBOARD_FOUND_PIECE", 2, 0);
				pieces_.at(2)->makeItFall();
				if(levelState_ == SELECT_POLAROID)
				{
					if(arg.keyPressed-49 < polaroidFrame_->getNumberButtons())
					{
						polaroidFrame_->selectButton(polaroidsName_.at(arg.keyPressed-49));
					}
				}
			}
			break;
		case 52:
			{
				LogClass::Instance()->addEntry("KEYBOARD_FOUND_PIECE", 7, 0);
				pieces_.at(7)->makeItFall();
				if(levelState_ == SELECT_POLAROID)
				{
					if(arg.keyPressed-49 < polaroidFrame_->getNumberButtons())
					{
						polaroidFrame_->selectButton(polaroidsName_.at(arg.keyPressed-49));
					}
				}
			}
			break;
		case 53:
			{
				LogClass::Instance()->addEntry("KEYBOARD_FOUND_PIECE", 6, 0);
				pieces_.at(6)->makeItFall();
			}
			break;
		case 54:
			{
				LogClass::Instance()->addEntry("KEYBOARD_FOUND_PIECE", 3, 0);
				pieces_.at(3)->makeItFall();
			}
			break;
		case 55:
			{
				LogClass::Instance()->addEntry("KEYBOARD_FOUND_PIECE", 9, 0);
				pieces_.at(9)->makeItFall();
			}
			break;
		case 56:
			{
				LogClass::Instance()->addEntry("KEYBOARD_FOUND_PIECE", 1, 0);
				pieces_.at(1)->makeItFall();
			}
			break;
		case 57:
			{
				LogClass::Instance()->addEntry("KEYBOARD_FOUND_PIECE", 5, 0);
				pieces_.at(5)->makeItFall();
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
				switch(levelState_ )
				{
					case INTRODUCTION:
						{
							kinectTracksSmbdy_ = true;
						}
						break;
					case MOUNTING:
						{
							LogClass::Instance()->addEntry("KEYBOARD_SPACESHIP_MOUNTED", 1, 0);

							std::vector<PieceClass*>::iterator pieceIt;
							for(pieceIt = pieces_.begin(); pieceIt != pieces_.end(); pieceIt++)
							{
								(*pieceIt)->setFinalPosition();
							}
						}
						break;
					case COLLECTING:
						{
							if(star_->isFalling()){
								if(star_->isGood())
								{
									LogClass::Instance()->addEntry("KEYBOARD_GOOD_STAR_CATCHED", 1, 0);
									star_->goToSpaceShip();
									starTouched_ = true;
								}
								else
								{
									LogClass::Instance()->addEntry("KEYBOARD_BAD_STAR_CATCHED", 1, 0);
									star_->reset();
									soundManager_->playSad();
									pico_->makeNo();
								}
							}
						}
						break;
				}
			}
			break;
		case 83: //S
		case 115: //s
			{
				childSaysHi_ = true;
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
				//if(levelState_ != IN
				std::vector<PieceClass*>::iterator pieceIt;
				for(pieceIt = pieces_.begin(); pieceIt != pieces_.end(); pieceIt++)
				{
					if((*pieceIt)->getCollisionSphere()->testIntersection(camera_, arg.mouseInfo.x, arg.mouseInfo.y))
					{
						(*pieceIt)->makeItFall();
					}
				}
			}
			break;
		case RIGHT_BUTTON:
			{
				//lightPos_[1].x = ((arg.mouseInfo.x/screenWidth_)*8)-4;
				//lightPos_[1].y = (1-(arg.mouseInfo.y/screenHeight_))*5;
			}
			break;
		default:
			{
				
			}
			break;
	}
}

void SecondScreenState::notify(GUIButton* notifier, ButtonStruct arg)
{
	if(levelState_ == SELECT_POLAROID)
	{
		switch(arg.buttonPurpose)
		{
			case(LOAD_OBJECT):
				{
					soundManager_->playFile("selection", FALSE);

					std::string name = arg.buttonInfo;
					spaceShip_->setTexture(name);

					levelState_ = TRANSITION;
					gameClock_->reset();
				}
				break;
			default:
				{
				}
				break;
		}
	}
}

void SecondScreenState::notify(KinectClass* notifier, KinectStruct arg)
{
	kinectTracksSmbdy_ = true;

	if(arg.type == FIRST_RIGHT_HAND_ROT)
	{
		// Calculate viewport position
		//kinectHandViewPos_[0] = Point(arg.handPos.x*screenWidth_/320, arg.handPos.y*screenHeight_/240);
		kinectHandViewPos_[0] = Point(arg.handPos.x, arg.handPos.y);

		// Calculate world position
		kinectHandWorldPos_[0].x = ((kinectHandViewPos_[0].x/screenWidth_)*8)-4;
		kinectHandWorldPos_[0].y = (1-(kinectHandViewPos_[0].y/screenHeight_))*5;
		kinectHandWorldPos_[0].z = 0;

		if(levelState_ == MOUNTING)
		{
			std::vector<PieceClass*>::iterator pieceIt;
			for(pieceIt = pieces_.begin(); pieceIt != pieces_.end(); pieceIt++)
			{
				if((*pieceIt)->getCollisionSphere()->testIntersection(camera_, kinectHandViewPos_[0].x, kinectHandViewPos_[0].y))
				{
					(*pieceIt)->lightIt();
				}
			}
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

		if(levelState_ == MOUNTING)
		{
			std::vector<PieceClass*>::iterator pieceIt;
			for(pieceIt = pieces_.begin(); pieceIt != pieces_.end(); pieceIt++)
			{
				if((*pieceIt)->getCollisionSphere()->testIntersection(camera_, kinectHandViewPos_[1].x, kinectHandViewPos_[1].y))
				{
					(*pieceIt)->lightIt();
				}
			}
		}
	}

	if(levelState_ == COLLECTING && arg.type == HOLD_HANDS)
	{
		if(arg.boolean == TRUE)
		{
			//kinectHoldViewPos_ = Point(arg.handPos.x*screenWidth_/320, arg.handPos.y*screenHeight_/240);
			kinectHoldViewPos_ = Point(arg.handPos.x, arg.handPos.y);

			// Calculate world position
			kinectHoldWorldPos_.x = ((kinectHoldViewPos_.x/screenWidth_)*8)-4;
			kinectHoldWorldPos_.y = (1-(kinectHoldViewPos_.y/screenHeight_))*5;
			kinectHoldWorldPos_.z = 0;

			for(int i = 0; i < starLevel_; i++)
			{
				if(star_->getCollisionSphere()->testIntersection(camera_, kinectHoldViewPos_.x, kinectHoldViewPos_.y))
				{
					if(star_->isGood())
					{
						LogClass::Instance()->addEntry("GOOD_STAR_CATCHED", i+1, 0);
						star_->goToSpaceShip();
						starTouched_ = true;
					}
					else
					{
						LogClass::Instance()->addEntry("BAD_STAR_CATCHED", i+1, 0);
						star_->reset();
						soundManager_->playSad();
						pico_->makeNo();
					}
				}
			}
		}
		else
		{
			kinectHoldViewPos_ = Point(-100/320, -100/240);

			// Calculate world position
			kinectHoldWorldPos_.x = ((kinectHoldViewPos_.x/screenWidth_)*8)-4;
			kinectHoldWorldPos_.y = (1-(kinectHoldViewPos_.y/screenHeight_))*5;
			kinectHoldWorldPos_.z = 0;
		}
	}
}

void SecondScreenState::loadConfigurationFromXML()
{
	std::string root = "./Data/configuration/level2/main_configuration.xml";

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
	pugi::xml_node greetingNode;
	if(!(greetingNode = rootNode.child("greeting_time")))
	{
		MessageBoxA(NULL, "Could not load greeting time node!", "SecondScreen - Error", MB_ICONERROR | MB_OK);
		timeGreeting_ = 5;
	}
	else
	{
		timeGreeting_ = greetingNode.text().as_float();
	}

	// Time night fading
	pugi::xml_node fadingNode;
	if(!(fadingNode = rootNode.child("fading_time")))
	{
		MessageBoxA(NULL, "Could not load fading time node!", "SecondScreen - Error", MB_ICONERROR | MB_OK);
		timeToNight_ = 5;
	}
	else
	{
		timeToNight_ = fadingNode.text().as_float();
	}
}

void SecondScreenState::loadScenario(std::string scenario)
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

void SecondScreenState::createScenarioObject(std::string scenario, std::string xmlName)
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

	// Add pointlight shader
	// Set specific multitexture shader for tips and increment textures array
	Shader3DClass* shaderTemp = Shader3DFactory::Instance()->CreateShader3D("PointlightDiffuseShader3D", graphicsManager_);
	objectLoadedTemp->setShader3D(shaderTemp);

	PointlightDiffuseShader3DClass* pointlightShader = dynamic_cast<PointlightDiffuseShader3DClass*>(objectLoadedTemp->getShader3D());
	pointlightShader->setPositions(lightPos_[0], lightPos_[1], lightPos_[2], lightPos_[3], lightPos_[4]);

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

bool SecondScreenState::loadPieces()
{
	std::stringstream root;
	root << "./Data/configuration/level2/pieces/pieces.xml";

	//Loading animations XML file
	pugi::xml_document fruitsDoc;
	if(!fruitsDoc.load_file(root.str().c_str()))
	{
		std::stringstream error;
		error << "Could not load object .xml file!" << root.str();
		MessageBoxA(NULL, error.str().c_str(), "SecondScreen - Piece - Error", MB_ICONERROR | MB_OK);
		return false;
	}

	pugi::xml_node root_node;
	// Get initial node
	if(!(root_node = fruitsDoc.child("pieces")))
	{
		MessageBoxA(NULL, "Could not find the fruits root node.", "SecondScreen - Piece - Error", MB_ICONERROR | MB_OK);
		return false;
	}

	int pieces = 0;
	for(pugi::xml_node fruitNode = root_node.first_child(); fruitNode; fruitNode = fruitNode.next_sibling())
	{
		std::string node_name = fruitNode.name();

		if(node_name ==  "piece")
		{
			pugi::xml_node modelNode;
			modelNode = fruitNode.child("model");

			pugi::xml_text modelName = modelNode.text();

			// Parse transformation data
			pugi::xml_node finalPositionNode;
			finalPositionNode = fruitNode.child("final_position");
			Point finalPos = Point(finalPositionNode.attribute("x").as_float(), finalPositionNode.attribute("y").as_float(), finalPositionNode.attribute("z").as_float());

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

			PieceClass* piece = new PieceClass();
			if(!piece)
			{
				return false;
			}

			if(!piece->setup(graphicsManager_, soundManager_, modelName.as_string(), finalPos, pos, terrainHeight_, scale, rotX, rotY, rotZ, pieces))
			{
				MessageBoxA(NULL, "Could not initialize fruit.", "SecondScreen - Piece - Error", MB_ICONERROR | MB_OK);
				return false;
			}				

			pugi::xml_node collisionNode = fruitNode.child("collision");

			positionNode = collisionNode.child("position");
			pos = Point(positionNode.attribute("x").as_float(), positionNode.attribute("y").as_float(), positionNode.attribute("z").as_float());

			piece->getCollisionSphere()->setRelativePosition(pos);

			pugi::xml_node radiusNode;
			radiusNode = collisionNode.child("radius");
			float radius = radiusNode.text().as_float();

			piece->getCollisionSphere()->setRadius(radius);

			piece->addListener(*pico_);

			pieces_.push_back(piece);
			pieces++;
		}
	}

	return true;
}

bool SecondScreenState::loadStars()
{
	star_ = new StarClass;
	if(!star_)
	{
		MessageBoxA(NULL, "Could not create star instance.", "SecondScreen - Error", MB_ICONERROR | MB_OK);
		return false;
	}
	if(!star_->setup(graphicsManager_, soundManager_, terrainHeight_))
	{
		MessageBoxA(NULL, "Could not create star object.", "SecondScreen - Error", MB_ICONERROR | MB_OK);
		return false;
	}

	starIndex_ = 0;

	starsOrder_[0] = true;
	starsOrder_[1] = false;
	starsOrder_[2] = true;
	starsOrder_[3] = false;
	starsOrder_[4] = false;
	starsOrder_[5] = true;
	starsOrder_[6] = false;
	starsOrder_[7] = true;
	starsOrder_[8] = false;
	starsOrder_[9] = false;
	starsOrder_[10] = true;
	starsOrder_[11] = false;
	starsOrder_[12] = false;
	starsOrder_[13] = false;
	starsOrder_[14] = true;
	starsOrder_[15] = false;
	starsOrder_[16] = false;
	starsOrder_[17] = true;
	starsOrder_[18] = false;
	starsOrder_[19] = false;

	return true;
}

void SecondScreenState::setupGUI(GraphicsManager* graphicsManager, InputManager* inputManager, KinectClass* kinectManager)
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

bool SecondScreenState::createPolaroids()
{
	polaroidsName_.clear();

	std::stringstream root;
	root << "./Data/configuration/level2/polaroids/polaroids.xml";

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

	int screenWidth, screenHeight;
	graphicsManager_->getScreenSize(screenWidth, screenHeight);

	int fruitIndex = 0;
	for(pugi::xml_node polaroidNode = root_node.first_child(); polaroidNode; polaroidNode = polaroidNode.next_sibling())
	{
		std::string node_name = polaroidNode.name();
		// Actuamos en consecuencia segun el tipo de nodo
		if(node_name ==  "polaroid")
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

			polaroidFrame_->addButton(graphicsManager_, imageName.as_string(), pos, size, imageName.as_string())->addListener(*this);
			polaroidsName_.push_back(imageName.as_string());
		}
		fruitIndex++;
	}

	return true;
}

void SecondScreenState::clearPolaroids()
{
	polaroidFrame_->deleteButtons();
}

bool SecondScreenState::powerToUser(float elapsedTime)
{
	Vector velocity;
	velocity.x = kinectHandWorldPos_[0].x - lightPos_[1].x;
	velocity.y = kinectHandWorldPos_[0].y - lightPos_[1].y;
	velocity.z = kinectHandWorldPos_[0].z - lightPos_[1].z;

	Vector normalizedVelocity = velocity.normalize();
	velocity = normalizedVelocity * 4.0f;

	lightPos_[1].x += velocity.x*elapsedTime;
	lightPos_[1].y += velocity.y*elapsedTime;

	// Update light body model position
	lightBody_->setPosition(Point(lightPos_[1].x, lightPos_[1].y, lightPos_[1].z));

	// Add particles effect
	lightParticles_[0]->setPosition(Point(lightPos_[1].x, lightPos_[1].y, lightPos_[1].z));
	lightParticles_[0]->update(elapsedTime, true);

	if(lightPos_[1].x < kinectHandWorldPos_[0].x+0.15 && lightPos_[1].x > kinectHandWorldPos_[0].x-0.15 && lightPos_[1].y < kinectHandWorldPos_[0].y+0.15 && lightPos_[1].y > kinectHandWorldPos_[0].y-0.15)
	{
		return true;
	}

	return false;
}

void SecondScreenState::changeLevel(LevelState level)
{
	// Set new level
	levelState_ = level;

	// Reset clock
	gameClock_->reset();
}

bool SecondScreenState::setupSplashScreen()
{
	// load splashscreen and calculate its position
	splashScreen_ = new ImageClass;
	if(!splashScreen_)
	{
		MessageBoxA(NULL, "Could not initialize the splashscreen image instance.", "SecondScreen - Error", MB_OK);
		return false;
	}

	if(!splashScreen_->setup(graphicsManager_->getDevice(), graphicsManager_->getShader2D(), screenWidth_, screenHeight_, "splashscreen2", screenWidth_, screenHeight_))
	{
		MessageBoxA(NULL, "Could not setup the splashscreen image.", "SecondScreen - Error", MB_OK);
		return false;
	}

	return true;
}

void SecondScreenState::drawSplashScreen()
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