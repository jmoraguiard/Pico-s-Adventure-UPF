#include "presentClass.h"

PresentClass::PresentClass()
{
	model_ = 0;

	position_.x = 0.0f;
	position_.y = 0.0f;
	position_.z = 0.0f;

	initialScaling_.x = 0.0001f;
	initialScaling_.y = 0.0001f;
	initialScaling_.z = 0.0001f;

	endScaling_.x = 0.0075f;
	endScaling_.y = 0.0075f;
	endScaling_.z = 0.0075f;

	scaling_ = initialScaling_;

	spawningTime_ = 0.0f;
	waitedTime_ = 0.0f;
	
	rotX_ = 0.0f;
	rotY_ = 0.0f; 
	rotZ_ = 0.0f;

	viewportPosition_.x = 0;
	viewportPosition_.y = 0;
	viewportPosition_.z = 0;
}

PresentClass::PresentClass(const PresentClass& other)
{
}

PresentClass::~PresentClass()
{
}

bool PresentClass::setup(GraphicsManager *graphicsManager, SoundFourthClass* soundManager, std::string fileName)
{
	graphicsManager->getScreenSize(screenWidth_, screenHeight_);
	soundManager_ = soundManager;

	model_ = Object3DFactory::Instance()->CreateObject3D("StaticObject3D", graphicsManager, fileName);

	sparks_ = new ParticleSystem;
	if(!sparks_)
	{
		MessageBoxA(NULL, "Could not create leafs instance", "Fruit - Error", MB_ICONERROR | MB_OK);
	}

	if(sparks_ && !sparks_->setup(graphicsManager, "star", position_, 0.8, 30, 90, XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), true))
	{
		MessageBoxA(NULL, "Could not setup leafs object", "Fruit - Error", MB_ICONERROR | MB_OK);
	}
	sparks_->setParticlesDeviation(Point(0.0f, 0.1f, 0.5f));
	sparks_->setParticlesVelocity(Point(0.0f, 0.0f, 0.0f), Point(2.0f, 2.0f, 0.1f));
	sparks_->setParticleSize(0.24f);
	
	// Load debug image for viewport transformation
	int screenWidth, screenHeight;
	graphicsManager->getScreenSize(screenWidth, screenHeight);

	viewportImage_ = new ImageClass;
	if(!viewportImage_)
	{
		MessageBoxA(NULL, "Could not initialize the kinectHand_ image instance.", "FreindSpaceShip - Error", MB_OK);
		return false;
	}

	if(!viewportImage_->setup(graphicsManager->getDevice(), graphicsManager->getShader2D(), screenWidth, screenHeight, "star", 40, 40))
	{
		MessageBoxA(NULL, "Could not setup the kinectHand_ image.", "FreindSpaceShip - Error", MB_OK);
		return false;
	}

	collisionTest_ = new SphereCollision();
	collisionTest_->setup(graphicsManager, Point(0.0f, 0.4f, 0.0f), 0.7f);

	setType(RED);

	presentState_ = WAITING;

	spawningTime_ = 3.0f;
	openTime_ = 1.0f;

	return true;
}

void PresentClass::update(float elapsedTime)
{
	updateViewportPosition();
	sparks_->setPosition(position_);
	collisionTest_->setPosition(position_);

	switch(presentState_)
	{
		case WAITING:
			{
				sparks_->update(elapsedTime, false);
			}
			break;
		case SPAWNING:
			{
				waitedTime_ += elapsedTime;

				if(waitedTime_ > spawningTime_)
				{
					scaling_.x = endScaling_.x;
					scaling_.y = endScaling_.y;
					scaling_.z = endScaling_.z;

					presentState_ = IN_BUILDING;
				}
				else
				{
					scaling_.x = (waitedTime_*endScaling_.x)/spawningTime_;
					scaling_.y = (waitedTime_*endScaling_.y)/spawningTime_;
					scaling_.z = (waitedTime_*endScaling_.z)/spawningTime_;
				}

				sparks_->update(elapsedTime, false);
			}
			break;
		case IN_BUILDING:
			{
				if(rand() % 400 > 395)
				{
					sparks_->update(elapsedTime, true);
				}
				else
				{
					sparks_->update(elapsedTime, false);
				}
			}
			break;
		case TOUCHED:
			{
				sparks_->update(elapsedTime, true);
				presentState_ = IN_BUILDING;
			}
			break;
		case IN_PLATFORM:
			{
				sparks_->update(elapsedTime, false);
			}
			break;
		case OPENING:
			{
				waitedTime_ += elapsedTime;

				if(waitedTime_ > openTime_)
				{
					scaling_.x = 0.00001f;
					scaling_.y = 0.00001f;
					scaling_.z = 0.00001f;

					presentState_ = DISAPEARING;
				}
				else
				{
					scaling_.x = initialScaling_.x+(waitedTime_*endScaling_.x)/openTime_;
					scaling_.y = initialScaling_.y+(waitedTime_*endScaling_.y)/openTime_;
					scaling_.z = initialScaling_.z+(waitedTime_*endScaling_.z)/openTime_;
				}

				sparks_->update(elapsedTime, false);
			}
			break;
		case DISAPEARING:
			{
				sparks_->update(elapsedTime, false);
			}
			break;
	}
}

void PresentClass::draw(GraphicsManager* graphicsManager, XMFLOAT4X4 worldMatrix, XMFLOAT4X4 viewMatrix, XMFLOAT4X4 projectionMatrix, LightClass* light, bool debug)
{
	if(debug)
	{
		XMFLOAT4X4 orthoMatrix;
		graphicsManager->getOrthoMatrix(orthoMatrix);

		int screenWidth, screenHeight;
		graphicsManager->getScreenSize(screenWidth, screenHeight);

		graphicsManager->turnOnWireframeRasterizer();
			collisionTest_->draw(graphicsManager->getDevice(), graphicsManager->getDeviceContext(), worldMatrix, viewMatrix, projectionMatrix, light);
		graphicsManager->turnOnSolidRasterizer();

		viewportImage_->draw(graphicsManager->getDeviceContext(), ((screenWidth/2)*-1)+viewportPosition_.x, ((screenHeight/2))-viewportPosition_.y, worldMatrix, viewMatrix, orthoMatrix, XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));
	}

	worldMatrix_ = worldMatrix;
	viewMatrix_ = viewMatrix;
	projectionMatrix_ = projectionMatrix;

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

	LightClass* lightTemp = new LightClass;

	switch(presentType_)
	{
		case BLUE:
			{
				lightTemp->setDiffuseColor(0.1f, 0.1f, 1.0f, 1.0f);
			}
			break;
		case GREEN:
			{
				lightTemp->setDiffuseColor(0.1f, 1.0f, 0.1f, 1.0f);
			}
			break;
		case RED:
			{
				lightTemp->setDiffuseColor(1.0f, 0.1f, 0.1f, 1.0f);
			}
			break;
		default:
			{
				lightTemp->setDiffuseColor(1.0f, 1.0f, 1.0f, 1.0f);
			}
			break;
	}
	lightTemp->setAmbientColor(light->getAmbientColor().x, light->getAmbientColor().y, light->getAmbientColor().z, light->getAmbientColor().w);
	lightTemp->setDirection(light->getDirection().x, light->getDirection().y, light->getDirection().z);

	model_->draw(graphicsManager->getDevice(), graphicsManager->getDeviceContext(), worldMatrix, viewMatrix, projectionMatrix, lightTemp);

	graphicsManager->turnOnParticlesAlphaBlending();
	graphicsManager->turnZBufferOff();
		sparks_->draw(graphicsManager->getDeviceContext(), worldMatrix_, viewMatrix_, projectionMatrix_, light);
	graphicsManager->turnZBufferOn();
	graphicsManager->turnOffAlphaBlending();
}

void PresentClass::destroy()
{
	// Release the model object
	if(model_)
	{
		model_->destroy();
		delete model_;
		model_ = 0;
	}

	// Release the collision object
	if(viewportImage_)
	{
		viewportImage_->destroy();
		delete viewportImage_;
		viewportImage_ = 0;
	}

	if(collisionTest_)
	{
		collisionTest_->destroy();
		delete collisionTest_;
		collisionTest_ = 0;
	}
}

void PresentClass::updateViewportPosition()
{
	viewportPosition_.x = (((position_.x)*screenWidth_)/20.5f)+screenWidth_/2.0f;
	viewportPosition_.y = screenHeight_-(((position_.y+3.0f)*screenHeight_)/11.0f);
}

void PresentClass::setPosition(Point position)
{
	position_.x = position.x;
	position_.y = position.y;
	position_.z = position.z;
}

void PresentClass::setInitialPosition(Point position)
{
	if(presentState_ == WAITING || presentState_ == DISAPEARING)
	{
		position_.x = position.x;
		position_.y = position.y;
		position_.z = position.z;

		initialScaling_.x = 0.0001f;
		initialScaling_.y = 0.0001f;
		initialScaling_.z = 0.0001f;

		scaling_.x = initialScaling_.x;
		scaling_.y = initialScaling_.y;
		scaling_.z = initialScaling_.z;

		presentState_ = SPAWNING;
		waitedTime_ = 0;
	}
}

Point PresentClass::getPosition()
{
	return position_;
}

void PresentClass::setScale(Vector scale)
{
	scaling_.x = scale.x;
	scaling_.y = scale.y;
	scaling_.z = scale.z;
}

Vector PresentClass::getScale()
{
	return scaling_;
}

void PresentClass::setRotationX(float rotX)
{
	rotX_ = rotX;
}

float PresentClass::getRotationX()
{
	return rotX_;
}

void PresentClass::setRotationY(float rotY)
{
	rotY_ = rotY;
}

float PresentClass::getRotationY()
{
	return rotY_;
}

void PresentClass::setRotationZ(float rotZ)
{
	rotZ_ = rotZ;
}

float PresentClass::getRotationZ()
{
	return rotZ_;
}

void PresentClass::setType(DirtColor color)
{
	switch(color)
	{
		case BLUE:
			{
				presentType_ = BLUE;
			}
			break;
		case GREEN:
			{
				presentType_ = GREEN;
			}
			break;
		case RED:
			{
				presentType_ = RED;
			}
			break;
		default:
			{
				presentType_ = BLUE;
			}
			break;
	}
}

DirtColor PresentClass::getType()
{
	return presentType_;
}

Point PresentClass::getViewportPosition()
{
	return viewportPosition_;
}

SphereCollision* PresentClass::getCollisionSphere()
{
	return collisionTest_;
}

bool PresentClass::isInBuilding()
{
	if(presentState_ == IN_BUILDING)
	{
		return true;
	}

	return false;
}

void PresentClass::setTouched()
{
	if(presentState_ == IN_BUILDING)
	{
		presentState_ = TOUCHED;
	}
}

void PresentClass::setInPlatform()
{
	presentState_ = IN_PLATFORM;
}

bool PresentClass::isInPlatform()
{
	if(presentState_ == IN_PLATFORM)
	{
		return true;
	}

	return false;
}

void PresentClass::open()
{
	waitedTime_ = 0;

	initialScaling_ = scaling_;

	presentState_ = OPENING;
}
