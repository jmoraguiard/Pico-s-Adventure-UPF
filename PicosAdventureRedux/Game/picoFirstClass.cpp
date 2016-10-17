#include "picoFirstClass.h"

PicoFirstClass::PicoFirstClass()
{
	for(int i = 0; i < 3; i++)
	{
		body_[i] = 0;
		tips_[i] = 0;
	}

	eyes_ = 0;
	
	for(int i = 0; i < 4; i++)
	{
		hats_[i] = 0;
	}

	waitedTime_ = 0.0f;
	eatingWaitTime_ = 0.0f;
	celebratingWaitTime_ = 0.0f;
	inactivityTime1_ = 0.0f;
	inactivityTime2_ = 0.0f;
	inactivityTime3_ = 0.0f;
	pointingTime_ =0.0f;

	behindFruit_ = 0.0f;

	position_.x = 0.0f;
	position_.y = 0.0f;
	position_.z = 0.0f;

	velocity_.x = 0.0f;
	velocity_.y = 0.0f;
	velocity_.z = 0.0f;

	objective_.x = 0.0f;
	objective_.y = 0.0f;
	objective_.z = 0.0f;
	
	lookAt_.x = 0.0f;
	lookAt_.y = 0.0f;
	lookAt_.z = 0.0f;

	scaling_.x = 1.0f;
	scaling_.y = 1.0f;
	scaling_.z = 1.0f;
	
	rotX_ = 0.0f;
	rotY_ = 0.0f; 
	newRotY_ = 0.0f;
	rotZ_ = 0.0f;

	hatToDraw_ = 0;

	previousFruitEatenID_ = 0;
	lastFruitEatenID_ = 0;
	pointedFruitID_ = 0;
}

PicoFirstClass::PicoFirstClass(const PicoFirstClass& other)
{

}

PicoFirstClass::~PicoFirstClass()
{

}

bool PicoFirstClass::setup(GraphicsManager* graphicsManager, CameraClass* camera, SoundFirstClass* soundManager)
{
	camera_ = camera;

	body_[0] = Object3DFactory::Instance()->CreateObject3D("AnimatedObject3D", graphicsManager, "miniBossCuerpo");
	body_[1] = Object3DFactory::Instance()->CreateObject3D("AnimatedObject3D", graphicsManager, "miniBossDelgadoCuerpo");
	body_[2] = Object3DFactory::Instance()->CreateObject3D("AnimatedObject3D", graphicsManager, "miniBossGordoCuerpo");
	bodyToDraw_ = 0;
	tips_[0] = Object3DFactory::Instance()->CreateObject3D("AnimatedObject3D", graphicsManager, "miniBossExtremidades");
	tips_[1] = Object3DFactory::Instance()->CreateObject3D("AnimatedObject3D", graphicsManager, "miniBossDelgadoExtremidades");
	tips_[2] = Object3DFactory::Instance()->CreateObject3D("AnimatedObject3D", graphicsManager, "miniBossGordoExtremidades");
	tipsToDraw_ = 0;
	eyes_ = Object3DFactory::Instance()->CreateObject3D("AnimatedObject3D", graphicsManager, "miniBossOjos");
	
	hats_[0] = Object3DFactory::Instance()->CreateObject3D("AnimatedObject3D", graphicsManager, "gorroFrutas");
	hats_[1] = Object3DFactory::Instance()->CreateObject3D("AnimatedObject3D", graphicsManager, "gorroPirata");
	hats_[2] = Object3DFactory::Instance()->CreateObject3D("AnimatedObject3D", graphicsManager, "gorroSanta");
	hats_[3] = Object3DFactory::Instance()->CreateObject3D("AnimatedObject3D", graphicsManager, "gorroCumple");
	drawHat_ = false;

	// Set specific multitexture shader for tips and increment textures array
	for(int i = 0; i < 3; i++)
	{
		Shader3DClass* shaderTemp = Shader3DFactory::Instance()->CreateShader3D("MultiTextureShader3D", graphicsManager);
		tips_[i]->setShader3D(shaderTemp);

		tips_[i]->getTextureArrayClass()->setNumberTextures(2);
	}

	loadExpressions(graphicsManager);

	collisionTest_ = new SphereCollision();
	collisionTest_->setup(graphicsManager, Point(0.0f, 1.6f, 0.0f), 0.6f);

	position_.x = -35.25f;
	position_.y = 0.0f;
	position_.z = 0.25f;

	hiddingPosition_.x = -5.5f;
	hiddingPosition_.y = 0.0f;
	hiddingPosition_.z = 0.25f;

	scaling_.x = 0.041f;
	scaling_.y = 0.041f;
	scaling_.z = 0.041f;

	lookAtCamera(false);

	positionUnhidding_[0].x = -3.0f;
	positionUnhidding_[0].y = 0.0f;
	positionUnhidding_[0].z = -0.25f;

	positionUnhidding_[1].x = -2.5f;
	positionUnhidding_[1].y = 0.0f;
	positionUnhidding_[1].z = -2.45f;

	unhiddingStep_ = 0;

	tipsColor_ = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);

	tipsLight_ = new LightClass;
	tipsLight_->setDiffuseColor(tipsColor_.x, tipsColor_.y, tipsColor_.z, 1.0f);

	textureName_ = "";

	faceState_ = NORMAL;

	expressionChangeTime_ = 0.75f;
	expressionPercentage_ = 0.0f;
	actualExpression_ = "normal";
	newExpression_ = "normal";

	for(int i = 0; i < 3; i++)
	{
		MultiTextureShader3DClass* multitextureShader = dynamic_cast<MultiTextureShader3DClass*>(tips_[i]->getShader3D());
		multitextureShader->setPercentage(0.1);
	}

	// Setup clock at the end so it starts when we run
	expressionClock_ = new ClockClass();
	if(!expressionClock_)
	{
		return false;
	}
	expressionClock_->reset();

	inactivityClock_ = new ClockClass();
	if(!inactivityClock_)
	{
		return false;
	}
	inactivityClock_->reset();

	eatingWaitTime_ = 2.0f;
	celebratingWaitTime_ = 1.2f;
	inactivityTime1_ = 10.0f;
	inactivityTime2_ = 10.0f;
	inactivityTime3_ = 10.0f;
	pointingTime_ = 10.0f;

	pointing1_ = false;
	pointing2_ = false;
	pointing3_ = false;

	pointed1_ = false;
	pointed2_ = false;
	pointed3_ = false;

	pointed_ = false;

	leavingTime_ = 5.0f;
	sayingGoddbyeTime_ = 5.0f;

	saidGoodbye_ = false;

	int screenWidth, screenHeight;
	graphicsManager->getScreenSize(screenWidth, screenHeight);

	info_ = new TextClass();
	if(!info_)
	{
		return false;
	}

	// Initialize the text object.
	if(!info_->setup(graphicsManager->getDevice(), graphicsManager->getDeviceContext(), graphicsManager->getShader2D(), screenWidth, screenHeight, 20, 40, "Rot: "))
	{
		MessageBoxA(NULL, "Could not initialize the FPS text object.", "GUIFrame - Error", MB_OK);
		return false;
	}

	behindFruit_ = 1.2f;

	hasToHide_ = true;
	picoState_ = HIDDING;
	goToPosition(hiddingPosition_);

	soundManager_ = soundManager;

	return true;
}

void PicoFirstClass::update(float elapsedTime)
{
	expressionClock_->tick();
	inactivityClock_->tick();

	for(int i = 0; i < 3; i++)
	{
		body_[i]->update(elapsedTime);
		tips_[i]->update(elapsedTime);

		// Update textures for the tips
		tips_[i]->getTextureArrayClass()->getTexturesArray()[0] = expressions_.at(actualExpression_)->getTexture();
		tips_[i]->getTextureArrayClass()->getTexturesArray()[1] = expressions_.at(newExpression_)->getTexture();

		MultiTextureShader3DClass* multitextureShader = dynamic_cast<MultiTextureShader3DClass*>(tips_[i]->getShader3D());
		multitextureShader->setPercentage(expressionPercentage_);
	}
	eyes_->update(elapsedTime);
	
	for(int i = 0; i < 4; i++)
	{
		hats_[i]->update(elapsedTime);
	}

	switch(picoState_)
	{
		case HIDDING:
			{
				if(lookAtCamera(true))
				{
					changeAnimation("backTree", 0.2f);
					picoState_ = HIDDEN;
					hasToHide_ = false;
				}
			}
			break;
		case HIDDEN:
			{
				if(fallenFruits_.size() > 0)
				{
					changeExpression("sorpresa");
					soundManager_->playSurpriseFile();

					goToPosition(positionUnhidding_[unhiddingStep_]);
					unhiddingStep_++;
					picoState_ = UNHIDDING;
				}
			}
			break;
		case UNHIDDING:
			{
				walk(elapsedTime);

				if(checkPicoArrivedObjective())
				{
					if(unhiddingStep_ < UNHIDDING_STEPS)
					{
						goToPosition(positionUnhidding_[unhiddingStep_]);
						unhiddingStep_++;
						picoState_ = UNHIDDING;
					}
					else
					{
						Point fallenFruitPos = fallenFruits_.front()->getPosition();
						fallenFruitPos.z += behindFruit_;

						goToPosition(fallenFruitPos);
						picoState_ = WAITING;
						inactivityClock_->reset();
					}
				}
			}
			break;
		case WAITING:
			{
				if(!pointing1_ && !pointing2_ && !pointing3_ && inactivityClock_->getTime() > inactivityTime1_)
				{
					switch(lastFruitEatenID_)
					{
						case 1:
							{
								changeAnimation("point_right", 0.4f);
								changeExpression("normal");
								soundManager_->playPointingFile();

								pointedFruitID_ = 2;

								LogClass::Instance()->addEntry("Pico_Points_1", levelState_, 2);
							}
							break;
						case 2:
							{
								if(rand() % 2)
								{
									changeAnimation("point_right", 0.4f);
									changeExpression("normal");
									soundManager_->playPointingFile();

									pointedFruitID_ = 3;

									LogClass::Instance()->addEntry("Pico_Points_1", levelState_, 3);
								}
								else
								{
									changeAnimation("point_left", 0.4f);
									changeExpression("normal");
									soundManager_->playPointingFile();

									pointedFruitID_ = 1;

									LogClass::Instance()->addEntry("Pico_Points_1", levelState_, 1);
								}
							}
							break;
						case 3:
							{
								if(rand() % 2 || levelState_ == 4)
								{
									changeAnimation("point_left", 0.4f);
									changeExpression("normal");
									soundManager_->playPointingFile();

									pointedFruitID_ = 2;

									LogClass::Instance()->addEntry("Pico_Points_1", levelState_, 2);
								}
								else
								{
									changeAnimation("point_right", 0.4f);
									changeExpression("normal");
									soundManager_->playPointingFile();

									pointedFruitID_ = 4;

									LogClass::Instance()->addEntry("Pico_Points_1", levelState_, 4);
								}
							}
							break;
						case 4:
							{
								changeAnimation("point_left", 0.4f);
								changeExpression("normal");
								soundManager_->playPointingFile();

								pointedFruitID_ = 3;

								LogClass::Instance()->addEntry("Pico_Points_1", levelState_, 3);
							}
							break;
					}

					// Bool variable to later check if we give him the pointed fruit
					pointed_ = true;

					pointing1_ = true;
					inactivityClock_->reset();
				}

				if(pointing1_ && !pointing2_ && !pointing3_ && !pointed1_ && inactivityClock_->getTime() > pointingTime_)
				{
					changeAnimation("idle", 0.2f);
					changeExpression("normal");

					pointed1_ = true;
					inactivityClock_->reset();
				}

				if(!pointing2_ && !pointing3_ && pointing1_ && pointed1_ && inactivityClock_->getTime() > inactivityTime2_)
				{
					switch(lastFruitEatenID_)
					{
						case 1:
							{
								changeAnimation("point_right", 0.4f);
								changeExpression("normal");
								soundManager_->playPointingFile();

								LogClass::Instance()->addEntry("Pico_Points_2", levelState_, 2);
							}
							break;
						case 2:
							{
								if(pointedFruitID_ == 3)
								{
									changeAnimation("point_right", 0.4f);
									changeExpression("normal");
									soundManager_->playPointingFile();

									LogClass::Instance()->addEntry("Pico_Points_2", levelState_, 3);
								}
								else
								{
									changeAnimation("point_left", 0.4f);
									changeExpression("normal");
									soundManager_->playPointingFile();

									LogClass::Instance()->addEntry("Pico_Points_2", levelState_, 1);
								}
							}
							break;
						case 3:
							{
								if(pointedFruitID_ == 4)
								{
									changeAnimation("point_right", 0.4f);
									changeExpression("normal");
									soundManager_->playPointingFile();

									LogClass::Instance()->addEntry("Pico_Points_2", levelState_, 4);
								}
								else
								{
									changeAnimation("point_left", 0.4f);
									changeExpression("normal");
									soundManager_->playPointingFile();

									LogClass::Instance()->addEntry("Pico_Points_2", levelState_, 2);
								}
							}
							break;
						case 4:
							{
								changeAnimation("point_left", 0.4f);
								changeExpression("normal");
								soundManager_->playPointingFile();

								LogClass::Instance()->addEntry("Pico_Points_2", levelState_, 3);
							}
							break;
					}

					pointing2_ = true;
					inactivityClock_->reset();
				}

				if(pointing1_ && pointing2_ && !pointing3_ && !pointed2_ && inactivityClock_->getTime() > pointingTime_)
				{
					changeAnimation("idle", 0.2f);
					changeExpression("normal");

					pointed2_ = true;
					inactivityClock_->reset();
				}

				if(!pointing3_ && pointing2_ && pointing1_ && pointed2_ && inactivityClock_->getTime() > inactivityTime3_)
				{
					LogClass::Instance()->addEntry("Fruit_Sound_Aid", levelState_, 3);
					soundManager_->playOwl();

					pointing3_ = true;
				}


				if(fallenFruits_.size() > 0)
				{
					Point fallenFruitPos = fallenFruits_.front()->getPosition();
					fallenFruitPos.z += behindFruit_;

					changeExpression("sorpresa");
					soundManager_->playSurpriseFile();

					goToPosition(fallenFruitPos);
					pointing1_ = false;
					pointing2_ = false;
					pointing3_ = false;

					pointed1_ = false;
					pointed2_ = false;
					pointed3_ = false;

					inactivityClock_->reset();
				}
			}
			break;
		case WALKING:
			{
				walk(elapsedTime);

				if(checkPicoArrivedObjective())
				{
					if(hasToHide_)
					{
						picoState_ = HIDDING;
					}
					else
					{
						eatFruit();
					}
				}
			}
			break;
		case TURNING:
			{
				
			}
			break;
		case EATING:
			{
				waitedTime_ += elapsedTime;

				notifyListeners(false);

				if(waitedTime_ > eatingWaitTime_)
				{
					// If Pico has pointed at leats once, we check which is the fruit eaten and the last one.
					if(pointed_)
					{
						if(lastFruitEatenID_ == pointedFruitID_)
						{
							celebratingWaitTime_ = 0.7f;
							changeAnimation("DanceAss", 0.4f);
							soundManager_->playHappySong();
						}
						else
						{
							celebratingWaitTime_ = 1.2f;
							changeAnimation("celebration", 0.4f);
						}

						pointed_ = false;
					}
					else
					{
						celebratingWaitTime_ = 1.2f;
						changeAnimation("celebration", 0.4f);
					}

					changeExpression("feliz");
					soundManager_->playCelebratingFile();
					soundManager_->playTransformationFile();

					waitedTime_ = 0.0f;

					picoState_ = CELEBRATING;
				}
			}
			break;
		case CELEBRATING:
			{
				waitedTime_ += elapsedTime;

				if(waitedTime_ > celebratingWaitTime_)
				{
					switch(fallenFruits_.front()->getFruitEffect())
					{
						case COLOR:
							{
								tipsColor_ = fallenFruits_.front()->getColorEffect();
							}
							break;
						case TEXTURE:
							{
								for(int i = 0; i < 3; i++)
								{
									body_[i]->getTextureArrayClass()->getTexturesArray()[0] = fallenFruits_.front()->getTextureEffect()->getTexture();
								}
							}
							break;
						case HAT:
							{
								setHat(fallenFruits_.front()->getHatEffect());

								if(fallenFruits_.front()->getHatEffect()->getModel()->getModelName() == "gorroCumple")
								{
									soundManager_->playBirthdayFile();
								}
								else if(fallenFruits_.front()->getHatEffect()->getModel()->getModelName() == "gorroFrutas")
								{
									soundManager_->playTropicalFile();
								}
								else if(fallenFruits_.front()->getHatEffect()->getModel()->getModelName() == "gorroPirata")
								{
									soundManager_->playPirateFile();
								}
								else if(fallenFruits_.front()->getHatEffect()->getModel()->getModelName() == "gorroSanta")
								{
									soundManager_->playChristmasFile();
								}
							}
							break;
						case BODY:
							{
								setBody(fallenFruits_.front()->getBodyEffect());
								setTips(fallenFruits_.front()->getTipsEffect());
							}
							break;
					}

					fallenFruits_.front()->resetFruit();
					fallenFruits_.pop_front();

					changeAnimation("idle", 0.2f);

					waitedTime_ = 0.0f;

					picoState_ = WAITING;
					inactivityClock_->reset();
				}
			}
			break;
		case SCARED:
			{
				
			}
			break;
		case WAITING_GOODBYE:
			{
				if(!saidGoodbye_ && inactivityClock_->getTime() > leavingTime_)
				{
					executeAnimation("hola", 0.2f);
					soundManager_->playGoodbyeFile();

					saidGoodbye_ = true;
					inactivityClock_->reset();
				}
				else if(saidGoodbye_ && inactivityClock_->getTime() > leavingTime_)
				{
					changeAnimation("idle", 0.2f);

					saidGoodbye_ = false;
					inactivityClock_->reset();
				}
			}
			break;
		case LEAVING:
			{
				walk(elapsedTime);
			}
			break;
		default:
			{
				
			}
			break;

	}

	switch(faceState_)
	{
		case NORMAL:
			{
				expressionPercentage_ = 0.0f;
			}
			break;
		case CHANGING:
			{
				expressionPercentage_ = expressionClock_->getTime()/expressionChangeTime_;
				if(expressionClock_->getTime() > expressionChangeTime_)
				{
					faceState_ = CHANGED;
				}
			}
			break;
		case CHANGED:
			{
				expressionPercentage_ = 1.0f;
				actualExpression_ = newExpression_;
				faceState_ = NORMAL;
			}
			break;
	}

	collisionTest_->setPosition(position_);
}

void PicoFirstClass::draw(GraphicsManager* graphicsManager, XMFLOAT4X4 worldMatrix, XMFLOAT4X4 viewMatrix, XMFLOAT4X4 projectionMatrix, LightClass* light, bool debug)
{
	if(debug)
	{
		graphicsManager->turnOnWireframeRasterizer();
		collisionTest_->draw(graphicsManager->getDevice(), graphicsManager->getDeviceContext(), worldMatrix, viewMatrix, projectionMatrix, light);
		graphicsManager->turnOnSolidRasterizer();

		std::stringstream RotText;
		RotText << "RotY: " << rotY_ << " NewRotY: " << newRotY_;
		info_->setText(RotText.str(), graphicsManager->getDeviceContext());

		XMFLOAT4X4 orthoMatrix;
		graphicsManager->getOrthoMatrix(orthoMatrix);

		graphicsManager->turnZBufferOff();
		graphicsManager->turnOnAlphaBlending();
			info_->draw(graphicsManager->getDeviceContext(), worldMatrix, viewMatrix, orthoMatrix);
		graphicsManager->turnOffAlphaBlending();
		graphicsManager->turnZBufferOn();
	}

	XMFLOAT4X4 rotatingMatrixZ;
	XMStoreFloat4x4(&rotatingMatrixZ, XMMatrixRotationZ(rotZ_));
	XMStoreFloat4x4(&worldMatrix, XMMatrixMultiply(XMLoadFloat4x4(&worldMatrix), XMLoadFloat4x4(&rotatingMatrixZ)));

	XMFLOAT4X4 rotatingMatrixY;
	XMStoreFloat4x4(&rotatingMatrixY, XMMatrixRotationY(rotY_));
	XMStoreFloat4x4(&worldMatrix, XMMatrixMultiply(XMLoadFloat4x4(&worldMatrix), XMLoadFloat4x4(&rotatingMatrixY)));

	XMFLOAT4X4 rotatingMatrixX;
	XMStoreFloat4x4(&rotatingMatrixX, XMMatrixRotationX(rotX_));
	XMStoreFloat4x4(&worldMatrix, XMMatrixMultiply(XMLoadFloat4x4(&worldMatrix), XMLoadFloat4x4(&rotatingMatrixX)));

	XMFLOAT4X4 scalingMatrix;
	XMStoreFloat4x4(&scalingMatrix, XMMatrixScaling(scaling_.x, scaling_.y, scaling_.z));
	XMStoreFloat4x4(&worldMatrix, XMMatrixMultiply(XMLoadFloat4x4(&worldMatrix), XMLoadFloat4x4(&scalingMatrix)));

	XMFLOAT4X4 movingMatrix;
	XMStoreFloat4x4(&movingMatrix, XMMatrixTranslation(position_.x, position_.y, position_.z));
	XMStoreFloat4x4(&worldMatrix, XMMatrixMultiply(XMLoadFloat4x4(&worldMatrix), XMLoadFloat4x4(&movingMatrix)));

	body_[bodyToDraw_]->draw(graphicsManager->getDevice(), graphicsManager->getDeviceContext(), worldMatrix, viewMatrix, projectionMatrix, light);
	eyes_->draw(graphicsManager->getDevice(), graphicsManager->getDeviceContext(), worldMatrix, viewMatrix, projectionMatrix, light);
	if(drawHat_)
	{
		hats_[hatToDraw_]->draw(graphicsManager->getDevice(), graphicsManager->getDeviceContext(), worldMatrix, viewMatrix, projectionMatrix, light);
	}

	tipsLight_->setAmbientColor(light->getAmbientColor().x, light->getAmbientColor().y, light->getAmbientColor().z, 1.0f);
	tipsLight_->setDiffuseColor(tipsColor_.x*light->getDiffuseColor().x, tipsColor_.y*light->getDiffuseColor().y, tipsColor_.z*light->getDiffuseColor().z, 1.0f);
	tipsLight_->setDirection(light->getDirection().x, light->getDirection().y, light->getDirection().z);

	tips_[tipsToDraw_]->draw(graphicsManager->getDevice(), graphicsManager->getDeviceContext(), worldMatrix, viewMatrix, projectionMatrix, tipsLight_);
}

void PicoFirstClass::destroy()
{
	savePicoConfiguration();

	for(int i = 0; i < 3; i++)
	{
		// Release the model object
		if(body_[i])
		{
			body_[i]->destroy();
			delete body_[i];
			body_[i] = 0;
		}

		// Release the model object
		if(tips_[i])
		{
			tips_[i]->destroy();
			delete tips_[i];
			tips_[i] = 0;
		}
	}

	// Release the model object
	if(eyes_)
	{
		eyes_->destroy();
		delete eyes_;
		eyes_ = 0;
	}

	// Release the model object
	for(int i = 0; i < 4; i++)
	{
		if(hats_[i])
		{
			hats_[i]->destroy();
			delete hats_[i];
			hats_[i] = 0;
		}
	}

	std::map<std::string, TextureClass*>::iterator it;
	for(it = expressions_.begin(); it != expressions_.end(); it++)
	{
		it->second->destroy();
	}
	expressions_.clear();
}

void PicoFirstClass::setLevelState(int level)
{
	pointing1_ = false;
	pointing2_ = false;
	pointing3_ = false;

	pointed1_ = false;
	pointed2_ = false;
	pointed3_ = false;

	inactivityClock_->reset();

	levelState_ = level;
}

void PicoFirstClass::goToPosition(Point position)
{
	changeAnimation("walk", 0.4f);

	objective_.x = position.x;
	//objective_.y = position.y;
	objective_.z = position.z;

	picoState_ = WALKING;
}

void PicoFirstClass::setToRest(bool ereasePrevious)
{
	if(lookAtCamera(false))
	{
		if(picoState_ != WAITING_GOODBYE)
		{
			picoState_ = WAITING;
		}
		inactivityClock_->reset();

		pointing1_ = false;
		pointing2_ = false;
		pointing3_ = false;

		pointed1_ = false;
		pointed2_ = false;
		pointed3_ = false;

		pointed_ = false;

		if(ereasePrevious)
		{
			changeAnimation("idle", 0.2f);

			previousFruitEatenID_ = 0;
			lastFruitEatenID_ = 0;
		}

		fallenFruits_.clear();
	}
}

void PicoFirstClass::makeHappy()
{
	if(picoState_ == WAITING)
	{
		changeExpression("feliz");
		soundManager_->playPurrFile();
	}
}

void PicoFirstClass::sayHello()
{
	if(picoState_ == WAITING)
	{
		executeAnimation("hola", 0.2f);
		soundManager_->playHiFile();
	}
}

void PicoFirstClass::makePointing()
{
	if(position_.x < 0)
	{
		changeAnimation("point_right", 0.4f);
		changeExpression("normal");
		soundManager_->playPointingFile();
	}
	else
	{
		changeAnimation("point_left", 0.4f);
		changeExpression("normal");
		soundManager_->playPointingFile();
	}

	// Bool variable to later check if we give him the pointed fruit
	pointed_ = true;

	pointing1_ = true;
	inactivityClock_->reset();
}

bool PicoFirstClass::isPointing()
{
	return pointing2_ && !pointed2_;
}

int PicoFirstClass::getLastEaten()
{
	return lastFruitEatenID_;
}

int PicoFirstClass::getPointedFruit()
{
	return pointedFruitID_;
}

bool PicoFirstClass::isWaiting()
{
	if(picoState_ == WAITING)
	{
		return true;
	}

	return false;
}

void PicoFirstClass::makeGoodbye()
{
	picoState_ = WAITING_GOODBYE;

	saidGoodbye_ = false;
}

void PicoFirstClass::makeLeave()
{
	goToPosition(Point(-35.25f, 0.0f, -3.0f));
	picoState_ = LEAVING;
}

Point PicoFirstClass::getPosition()
{
	return position_;
}

void PicoFirstClass::setTipsColor(XMFLOAT4 color)
{
	tipsColor_ = color;
}

void PicoFirstClass::setBodyTexture(TextureClass* texture)
{
	for(int i = 0; i < 3; i++)
	{
		body_[i]->getTextureArrayClass()->getTexturesArray()[0] = texture->getTexture();
		textureName_ = texture->getTextureName();
	}
}

void PicoFirstClass::setHat(Object3D* hat)
{
	drawHat_ = true;

	for(int i = 0; i < 4; i++)
	{
		if(hat->getModelName() == hats_[i]->getModelName())
		{
			hatToDraw_ = i;
		}
	}
}

void PicoFirstClass::setBody(Object3D* body)
{
	for(int i = 0; i < 3; i++)
	{
		if(body->getModelName() == body_[i]->getModelName())
		{
			bodyToDraw_ = i;
		}
	}
}

void PicoFirstClass::setTips(Object3D* tips)
{
	for(int i = 0; i < 3; i++)
	{
		if(tips->getModelName() == tips_[i]->getModelName())
		{
			tipsToDraw_ = i;
		}
	}
}

SphereCollision* PicoFirstClass::getCollisionSphere()
{
	return collisionTest_;
}

float PicoFirstClass::approach(float goal, float current, float dt)
{
	float difference = goal - current;

	if(difference > dt)
		return current + dt;
	if(difference < dt)
		return current - dt;

	return goal;
}

void PicoFirstClass::walk(float elapsedTime)
{
	velocity_.x = objective_.x - position_.x;
	velocity_.y = objective_.y - position_.y;
	velocity_.z = objective_.z - position_.z;

	Vector normalizedVelocity = velocity_.normalize();
	velocity_ = normalizedVelocity * 1.8f;

	float newRotY_ = acos(normalizedVelocity.z);
	if(normalizedVelocity.x < 0)
	{
		newRotY_ += 3.141592f;
	}

	if(rotY_ > newRotY_-0.1f && rotY_ < newRotY_+0.1f)
	{
		rotY_ = newRotY_;

		position_.x += velocity_.x*elapsedTime;
		position_.y += velocity_.y*elapsedTime;
		position_.z += velocity_.z*elapsedTime;
	}
	else
	{
		if(rotY_ < newRotY_)
		{
			rotY_ += 0.04f;
		}
		if(rotY_ > newRotY_)
		{
			rotY_ -= 0.04f;
		}
	}
}

bool PicoFirstClass::lookAtCamera(bool check)
{
	Point cameraPos = Point(camera_->getPosition().x, camera_->getPosition().y, camera_->getPosition().z);

	lookAt_.x = cameraPos.x-position_.x;
	lookAt_.y = cameraPos.y-position_.y;
	lookAt_.z = cameraPos.z-position_.z;

	Vector normalizedLookAt = lookAt_.normalize();

	newRotY_ = acos(normalizedLookAt.x);//3.141592f-1.570796f/4; 

	if(normalizedLookAt.x > 0)
	{
		newRotY_ += XM_PIDIV2;
	}
	else
	{
		newRotY_ += XM_PIDIV2;
	}

	if(check)
	{
		if(rotY_ > newRotY_-0.1f && rotY_ < newRotY_+0.1f)
		{
			rotY_ = newRotY_;

			return true;
		}
		else
		{
			if(rotY_ < newRotY_)
			{
				rotY_ += 0.04f;
			}
			if(rotY_ > newRotY_)
			{
				rotY_ -= 0.04f;
			}
		}

		return false;
	}
	else
	{
		rotY_ = newRotY_;

		return true;
	}
}

bool PicoFirstClass::checkPicoArrivedObjective()
{
	if(position_.x < objective_.x+0.05 && position_.x > objective_.x-0.05 && position_.z < objective_.z+0.05 && position_.z > objective_.z-0.05)
	{
		return true;
	}

	return false;
}

void PicoFirstClass::eatFruit()
{
	if(fallenFruits_.size() > 0)
	{
		if(lookAtCamera(true))
		{
			changeAnimation("eat", 0.2f);
			soundManager_->playEatingFile();

			Point fruitPos = fallenFruits_.front()->getPosition();
			fruitPos.y = fruitPos.y + 1.2f;
			fruitPos.z = fruitPos.z + behindFruit_/3.5;
			fallenFruits_.front()->setPosition(fruitPos);
			previousFruitEatenID_ = lastFruitEatenID_;
			lastFruitEatenID_ = fallenFruits_.front()->getLogicID();

			waitedTime_ = 0.0f;

			picoState_ = EATING;
			notifyListeners(false);
		}
	}
	else
	{
		if(lookAtCamera(true))
		{
			changeAnimation("idle", 0.2f);

			picoState_ = WAITING;
		}
	}
}

void PicoFirstClass::notify(InputManager* notifier, InputStruct arg)
{
	switch(arg.keyPressed){
		default:
			{
				
			}
			break;
	}
}

void PicoFirstClass::notify(FruitClass* notifier, Point arg)
{
	fallenFruits_.push_back(notifier);
}

void PicoFirstClass::notify(BirdClass* notifier, bool arg)
{
	if(picoState_ == WALKING && !arg)
	{
		changeAnimation("negation", 0.2f);
		changeExpression("enfadado");

		lookAtCamera(false);

		picoState_ = SCARED;
	}

	if(picoState_ == SCARED && arg)
	{
		Point fallenFruitPos = fallenFruits_.front()->getPosition();
		fallenFruitPos.z += behindFruit_;

		changeExpression("normal");

		goToPosition(fallenFruitPos);
	}
}

void PicoFirstClass::changeAnimation(std::string name, float time)
{
	AnimatedObject3D* animatedTemp;
	AnimatedCal3DModelClass* cal3dTemp;
	for(int i = 0; i < 3; i++)
	{
		animatedTemp = dynamic_cast<AnimatedObject3D*>(body_[i]);
		cal3dTemp = dynamic_cast<AnimatedCal3DModelClass*>(animatedTemp->getModel());
		cal3dTemp->setAnimationToPlay(name, time);
		animatedTemp = dynamic_cast<AnimatedObject3D*>(tips_[i]);
		cal3dTemp = dynamic_cast<AnimatedCal3DModelClass*>(animatedTemp->getModel());
		cal3dTemp->setAnimationToPlay(name, time);
	}
	animatedTemp = dynamic_cast<AnimatedObject3D*>(eyes_);
	cal3dTemp = dynamic_cast<AnimatedCal3DModelClass*>(animatedTemp->getModel());
	cal3dTemp->setAnimationToPlay(name, time);
	for(int i = 0; i < 4; i++)
	{
		animatedTemp = dynamic_cast<AnimatedObject3D*>(hats_[i]);
		cal3dTemp = dynamic_cast<AnimatedCal3DModelClass*>(animatedTemp->getModel());
		cal3dTemp->setAnimationToPlay(name, time);
	}
}

void PicoFirstClass::executeAnimation(std::string name, float time)
{
	AnimatedObject3D* animatedTemp;
	AnimatedCal3DModelClass* cal3dTemp;
	for(int i = 0; i < 3; i++)
	{
		animatedTemp = dynamic_cast<AnimatedObject3D*>(body_[i]);
		cal3dTemp = dynamic_cast<AnimatedCal3DModelClass*>(animatedTemp->getModel());
		cal3dTemp->setAnimationToExecute(name, time);
		animatedTemp = dynamic_cast<AnimatedObject3D*>(tips_[i]);
		cal3dTemp = dynamic_cast<AnimatedCal3DModelClass*>(animatedTemp->getModel());
		cal3dTemp->setAnimationToExecute(name, time);
	}
	animatedTemp = dynamic_cast<AnimatedObject3D*>(eyes_);
	cal3dTemp = dynamic_cast<AnimatedCal3DModelClass*>(animatedTemp->getModel());
	cal3dTemp->setAnimationToExecute(name, time);
	for(int i = 0; i < 4; i++)
	{
		animatedTemp = dynamic_cast<AnimatedObject3D*>(hats_[i]);
		cal3dTemp = dynamic_cast<AnimatedCal3DModelClass*>(animatedTemp->getModel());
		cal3dTemp->setAnimationToExecute(name, time);
	}
}

void PicoFirstClass::changeExpression(std::string newExpression)
{
	if(faceState_ != CHANGING && picoState_ != WALKING)
	{
		newExpression_ = newExpression;
		faceState_ = CHANGING;
		expressionClock_->reset();
	}
}

void PicoFirstClass::loadExpressions(GraphicsManager* graphicsManager)
{
	TextureClass* temp1 = new TextureClass;
	std::string filePath = "./Data/models/miniBossExtremidades/d-e-normal.dds";
	bool result = temp1->setup(graphicsManager->getDevice(), filePath);
	if(!result)
	{
		MessageBoxA(NULL, "Could not load the normal expression textures!", "Pico - Error", MB_ICONERROR | MB_OK);
	}
	expressions_.insert(std::pair<std::string, TextureClass*>("normal", temp1));

	TextureClass* temp2 = new TextureClass;
	filePath = "./Data/models/miniBossExtremidades/d-e-feliz.dds";
	result = temp2->setup(graphicsManager->getDevice(), filePath);
	if(!result)
	{
		MessageBoxA(NULL, "Could not load the feliz expression textures!", "Pico - Error", MB_ICONERROR | MB_OK);
	}
	expressions_.insert(std::pair<std::string, TextureClass*>("feliz", temp2));

	TextureClass* temp3 = new TextureClass;
	filePath = "./Data/models/miniBossExtremidades/d-e-sorpresa.dds";
	result = temp3->setup(graphicsManager->getDevice(), filePath);
	if(!result)
	{
		MessageBoxA(NULL, "Could not load the sorpresa expression textures!", "Pico - Error", MB_ICONERROR | MB_OK);
	}
	expressions_.insert(std::pair<std::string, TextureClass*>("sorpresa", temp3));

	TextureClass* temp4 = new TextureClass;
	filePath = "./Data/models/miniBossExtremidades/d-e-triste.dds";
	result = temp4->setup(graphicsManager->getDevice(), filePath);
	if(!result)
	{
		MessageBoxA(NULL, "Could not load the triste expression textures!", "Pico - Error", MB_ICONERROR | MB_OK);
	}
	expressions_.insert(std::pair<std::string, TextureClass*>("triste", temp4));

	TextureClass* temp5 = new TextureClass;
	filePath = "./Data/models/miniBossExtremidades/d-e-sorpresa2.dds";
	result = temp5->setup(graphicsManager->getDevice(), filePath);
	if(!result)
	{
		MessageBoxA(NULL, "Could not load the sorpresa2 expression textures!", "Pico - Error", MB_ICONERROR | MB_OK);
	}
	expressions_.insert(std::pair<std::string, TextureClass*>("sorpresa2", temp5));

	TextureClass* temp6 = new TextureClass;
	filePath = "./Data/models/miniBossExtremidades/d-e-superfeliz.dds";
	result = temp6->setup(graphicsManager->getDevice(), filePath);
	if(!result)
	{
		MessageBoxA(NULL, "Could not load the superfeliz expression textures!", "Pico - Error", MB_ICONERROR | MB_OK);
	}
	expressions_.insert(std::pair<std::string, TextureClass*>("superfeliz", temp6));

	TextureClass* temp7 = new TextureClass;
	filePath = "./Data/models/miniBossExtremidades/d-e-cabreo.dds";
	result = temp7->setup(graphicsManager->getDevice(), filePath);
	if(!result)
	{
		MessageBoxA(NULL, "Could not load the enfadado expression textures!", "Pico - Error", MB_ICONERROR | MB_OK);
	}
	expressions_.insert(std::pair<std::string, TextureClass*>("enfadado", temp7));
}

void PicoFirstClass::savePicoConfiguration()
{
	pugi::xml_document doc;

	// add node with some name
	pugi::xml_node node = doc.append_child("pico");

	// add body node
	pugi::xml_node body = node.append_child("body");
	pugi::xml_node bodyModel = body.append_child("model");
	bodyModel.append_child(pugi::node_pcdata).set_value(body_[bodyToDraw_]->getModelName().c_str());
	pugi::xml_node bodyTexture = body.append_child("texture");
	bodyTexture.append_child(pugi::node_pcdata).set_value(textureName_.c_str());

	// add tips node
	pugi::xml_node tips = node.append_child("tips");
	pugi::xml_node tipsModel = tips.append_child("model");
	tipsModel.append_child(pugi::node_pcdata).set_value(tips_[tipsToDraw_]->getModelName().c_str());
	pugi::xml_node tipsColor = tips.append_child("color");
	tipsColor.append_attribute("r") = tipsColor_.x;
	tipsColor.append_attribute("g") = tipsColor_.y;
	tipsColor.append_attribute("b") = tipsColor_.z;

	// add hat node
	pugi::xml_node hatModel = node.append_child("hat");
	hatModel.append_child(pugi::node_pcdata).set_value(hats_[hatToDraw_]->getModelName().c_str());

	// Generate fileName
	std::string fileName = "Data/configuration/level1/pico_configuration.xml";

	// save document to file
	std::cout << "Saving result: " << doc.save_file(fileName.c_str()) << std::endl;
}