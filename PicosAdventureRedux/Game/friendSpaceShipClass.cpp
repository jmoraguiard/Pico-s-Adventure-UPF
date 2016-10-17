#include "friendSpaceShipClass.h"

FriendSpaceShipClass::FriendSpaceShipClass()
{
	model_ = 0;

	position_.x = 0.0f;
	position_.y = 0.0f;
	position_.z = 0.0f;

	endPosition_.x = 0.0f;
	endPosition_.y = 0.0f;
	endPosition_.z = 0.0f;

	newPosition_.x = 0.0f;
	newPosition_.y = 0.0f;
	newPosition_.z = 0.0f;

	velocity_.x = 0.0f;
	velocity_.y = 0.0f;
	velocity_.z = 0.0f;

	scaling_.x = 0.01f;
	scaling_.y = 0.01f;
	scaling_.z = 0.01f;
	
	rotX_ = 0.0f;
	rotY_ = 0.0f; 
	rotZ_ = 0.0f;

	viewportPosition_.x = 0;
	viewportPosition_.y = 0;
	viewportPosition_.z = 0;
}

FriendSpaceShipClass::FriendSpaceShipClass(const FriendSpaceShipClass& other)
{
}

FriendSpaceShipClass::~FriendSpaceShipClass()
{
}

bool FriendSpaceShipClass::setup(GraphicsManager *graphicsManager, SoundThirdClass* soundManager, std::string fileName)
{
	graphicsManager->getScreenSize(screenWidth_, screenHeight_);
	soundManager_ = soundManager;

	model_ = Object3DFactory::Instance()->CreateObject3D("StaticObject3D", graphicsManager, fileName);

	spaceShipParticles_ = new ParticleSystem;
	if(!spaceShipParticles_)
	{
		MessageBoxA(NULL, "Could not create light1 particles instance", "SecondScreen - Error", MB_ICONERROR | MB_OK);
	}
	if(spaceShipParticles_ && !spaceShipParticles_->setup(graphicsManager, "star", getPosition(), 1.5, 200, 200, XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), true))
	{
		MessageBoxA(NULL, "Could not setup light1 particles object", "SecondScreen - Error", MB_ICONERROR | MB_OK);
	}

	// load dirt particles that will cover the spaceship
	dirtDistance_ = 1.1f;
	initialDirtDistance_ = dirtDistance_;
	dirt_ = new ParticleSystem;
	if(!dirt_)
	{
		MessageBoxA(NULL, "Could not create dirt instance", "FreindSpaceShip - Error", MB_ICONERROR | MB_OK);
	}

	if(dirt_ && !dirt_->setup(graphicsManager, "ParticulaNuvolCoets_3", position_, dirtDistance_, 40, 180, XMFLOAT4(0.2f, 0.2f, 0.2f, 1.0f), false))
	{
		MessageBoxA(NULL, "Could not setup dirt object", "FreindSpaceShip - Error", MB_ICONERROR | MB_OK);
	}
	dirt_->setParticlesDeviation(Point(0.0f, 0.1f, 0.5f));
	dirt_->setParticlesVelocity(Point(0.0f, 0.0f, 0.0f), Point(2.0f, 2.0f, 0.0f));
	dirt_->setParticleSize(0.24f);

	touched_ = false;
	
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

	friendSpaceShipState_ = WAITING;

	setType(GREEN);

	return true;
}

void FriendSpaceShipClass::update(float elapsedTime)
{
	updateViewportPosition();
	if(toLeft_)
	{
		spaceShipParticles_->setPosition(Point(position_.x+1.0f, position_.y, position_.z));
	}
	else
	{
		spaceShipParticles_->setPosition(Point(position_.x-1.0f, position_.y, position_.z));
	}
	dirt_->setPosition(Point(position_.x, position_.y, position_.z-0.5));

	switch(friendSpaceShipState_)
	{
		case WAITING:
			{
				spaceShipParticles_->update(elapsedTime, false);
				dirt_->update(elapsedTime, true);
			}
			break;
		case ENTERING:
			{
				spaceShipParticles_->update(elapsedTime, false);
				dirt_->update(elapsedTime, true);

				soundManager_->playFile("nau_atrapada", false);

				velocity_.x = newPosition_.x - position_.x;
				velocity_.y = newPosition_.y - position_.y;
				velocity_.z = newPosition_.z - position_.z;

				Vector normalizedVelocity = velocity_.normalize();
				velocity_ = normalizedVelocity*2;

				position_.x += velocity_.x*elapsedTime;
				position_.y += velocity_.y*elapsedTime;
				position_.z += velocity_.z*elapsedTime;

				if(checkArrivedObjective())
				{
					friendSpaceShipState_ = STUCK;
				}
			}
			break;
		case STUCK:
			{
				spaceShipParticles_->update(elapsedTime, false);
				dirt_->update(elapsedTime, true);

				soundManager_->playFile("nau_atrapada", false);

				if(touched_)
				{
					dirtDistance_ -= 0.35*elapsedTime;

					if(dirtDistance_ < 0)
					{
						dirtDistance_ = 0;
						makeItFree();
					}

					dirt_->setParticleDistance(dirtDistance_);
					touched_ = false;
				}
				else
				{
					dirtDistance_ += 0.25*elapsedTime;

					if(dirtDistance_ > initialDirtDistance_)
					{
						dirtDistance_ = initialDirtDistance_;
					}

					dirt_->setParticleDistance(dirtDistance_);
				}
			}
			break;
		case GOING_AWAY:
			{
				spaceShipParticles_->update(elapsedTime, true);
				dirt_->update(elapsedTime, false);

				velocity_.x = newPosition_.x - position_.x;
				velocity_.y = newPosition_.y - position_.y;
				velocity_.z = newPosition_.z - position_.z;

				Vector normalizedVelocity = velocity_.normalize();
				velocity_ = normalizedVelocity*4.5;

				position_.x += velocity_.x*elapsedTime;
				position_.y += velocity_.y*elapsedTime;
				position_.z += velocity_.z*elapsedTime;

				/*rotX_ += 0.1;
				if(rotX_ > XM_PI)
				{
					rotX_ -= XM_2PI;
				}*/

				if(checkArrivedObjective())
				{
					position_ = endPosition_;
					friendSpaceShipState_ = OUTSIDE;
					rotX_ = 0;
				}
			}
			break;
		case OUTSIDE:
			{
				spaceShipParticles_->update(elapsedTime, false);
				dirt_->update(elapsedTime, false);
			}
			break;
		case SLOW_MOTION:
			{
				spaceShipParticles_->update(elapsedTime, false);
				dirt_->update(elapsedTime, true);

				soundManager_->playFile("nau_atrapada", false);

				if(touched_)
				{
					dirtDistance_ -= 0.35*elapsedTime;

					if(dirtDistance_ < 0)
					{
						dirtDistance_ = 0;
						makeItFree();
					}

					dirt_->setParticleDistance(dirtDistance_);
					touched_ = false;
				}
				else
				{
					dirtDistance_ += 0.25*elapsedTime;

					if(dirtDistance_ > initialDirtDistance_)
					{
						dirtDistance_ = initialDirtDistance_;
					}

					dirt_->setParticleDistance(dirtDistance_);
				}

				velocity_.x = newPosition_.x - position_.x;
				velocity_.y = newPosition_.y - position_.y;
				velocity_.z = newPosition_.z - position_.z;

				Vector normalizedVelocity = velocity_.normalize();
				velocity_ = normalizedVelocity*1.0f;

				position_.x += velocity_.x*elapsedTime;
				position_.y += velocity_.y*elapsedTime;
				position_.z += velocity_.z*elapsedTime;

				if(checkArrivedObjective())
				{
					position_ = endPosition_;
					friendSpaceShipState_ = OUTSIDE;
					notifyListeners(false);
				}
			}
			break;
			case CELEBRATING:
			{
				spaceShipParticles_->update(elapsedTime, true);

				velocity_.x = newPosition_.x - position_.x;
				velocity_.y = newPosition_.y - position_.y;
				velocity_.z = newPosition_.z - position_.z;

				Vector normalizedVelocity = velocity_.normalize();
				velocity_ = normalizedVelocity*3.25;

				position_.x += velocity_.x*elapsedTime;
				position_.y += velocity_.y*elapsedTime;
				position_.z += velocity_.z*elapsedTime;

				rotX_ += 0.1;
				if(rotX_ > XM_PI)
				{
					rotX_ -= XM_2PI;
				}

				if(checkArrivedObjective())
				{
					position_ = endPosition_;
					friendSpaceShipState_ = CELEBRATING_OUTSIDE;
					rotX_ = 0;
				}
			}
			break;
		case CELEBRATING_OUTSIDE:
			{
				spaceShipParticles_->update(elapsedTime, false);
				dirt_->update(elapsedTime, false);
			}
			break;
			break;
		default:
			{

			}
			break;
	}
}

void FriendSpaceShipClass::draw(GraphicsManager* graphicsManager, XMFLOAT4X4 worldMatrix, XMFLOAT4X4 viewMatrix, XMFLOAT4X4 projectionMatrix, LightClass* light, bool debug)
{
	XMFLOAT4X4 viewMatrixO, projectionMatrixO, worldMatrixO;
	worldMatrixO = worldMatrix;
	viewMatrixO = viewMatrix;
	projectionMatrixO = projectionMatrix;

	if(debug)
	{
		XMFLOAT4X4 orthoMatrix;
		graphicsManager->getOrthoMatrix(orthoMatrix);

		int screenWidth, screenHeight;
		graphicsManager->getScreenSize(screenWidth, screenHeight);

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
	if(toLeft_)
	{
		XMStoreFloat4x4(&movingMatrix, XMMatrixTranslation(position_.x+1.0f, position_.y, position_.z));
	}
	else
	{
		XMStoreFloat4x4(&movingMatrix, XMMatrixTranslation(position_.x-1.0f, position_.y, position_.z));
	}
	XMStoreFloat4x4(&worldMatrix, XMMatrixMultiply(XMLoadFloat4x4(&worldMatrix), XMLoadFloat4x4(&movingMatrix)));

	model_->draw(graphicsManager->getDevice(), graphicsManager->getDeviceContext(), worldMatrix, viewMatrix, projectionMatrix, light);

	if(friendSpaceShipState_ == GOING_AWAY || friendSpaceShipState_ == CELEBRATING)
	{
		graphicsManager->turnOnParticlesAlphaBlending();
		graphicsManager->turnZBufferOff();
			spaceShipParticles_->draw(graphicsManager->getDeviceContext(), worldMatrixO, viewMatrixO, projectionMatrixO, light);
		graphicsManager->turnZBufferOn();
		graphicsManager->turnOffAlphaBlending();
	}
	else
	{
		graphicsManager->turnOnParticlesAlphaBlending();
		graphicsManager->turnZBufferOff();
			dirt_->draw(graphicsManager->getDeviceContext(), worldMatrixO, viewMatrixO, projectionMatrixO, light);
		graphicsManager->turnZBufferOn();
		graphicsManager->turnOffAlphaBlending();
	}
}

void FriendSpaceShipClass::destroy()
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

	if(dirt_)
	{
		dirt_->destroy();
		delete dirt_;
		dirt_ = 0;
	}
}

void FriendSpaceShipClass::updateViewportPosition()
{
	/*XMFLOAT4 position = XMFLOAT4(position_.x, position_.y, position_.z, 1.0f);

	XMFLOAT4 resultPos;
	XMStoreFloat4(&resultPos, XMVector4Transform(XMLoadFloat4(&position), XMLoadFloat4x4(&worldMatrix_)));
	XMStoreFloat4(&resultPos, XMVector4Transform(XMLoadFloat4(&resultPos), XMLoadFloat4x4(&viewMatrix_)));
	XMStoreFloat4(&resultPos, XMVector4Transform(XMLoadFloat4(&resultPos), XMLoadFloat4x4(&projectionMatrix_)));
	
	viewportPosition_.x = resultPos.x;
	viewportPosition_.y = resultPos.y;*/

	viewportPosition_.x = (((position_.x)*screenWidth_)/13.5f)+screenWidth_/2.0f;
	viewportPosition_.y = screenHeight_-(((position_.y+1.7)*screenHeight_)/8.3f);
}

void FriendSpaceShipClass::setPosition(Point position)
{
	//if(model_->getName() != "navepascal")
	//{
		if(position.x < 0)
		{
			rotZ_ = -XM_PI/2;
			toLeft_ = false;
		}
		else if(position.x > 0)
		{
			rotZ_ = XM_PI/2;
			toLeft_ = true;
		}
	//}

	position_.x = position.x;
	position_.y = position.y;
	position_.z = position.z;

	endPosition_.x = -position.x;
	endPosition_.y = position.y;
	endPosition_.z = position.z;

	dirtDistance_ = initialDirtDistance_;
	dirt_->setParticleDistance(dirtDistance_);

	friendSpaceShipState_ = WAITING;
}

Point FriendSpaceShipClass::getPosition()
{
	return position_;
}

void FriendSpaceShipClass::goToPosition(Point pos)
{
	if(friendSpaceShipState_ == WAITING)
	{
		friendSpaceShipState_ = ENTERING;
		newPosition_ = pos;
	}
	if(friendSpaceShipState_ == STUCK)
	{
		friendSpaceShipState_ = GOING_AWAY;
		newPosition_ = endPosition_;
	}
}

void FriendSpaceShipClass::celebrateToPosition(Point pos)
{
	if(friendSpaceShipState_ == WAITING)
	{
		friendSpaceShipState_ = CELEBRATING;
		newPosition_ = endPosition_;
	}
}

void FriendSpaceShipClass::flyToPosition(Point pos)
{
	if(friendSpaceShipState_ == WAITING)
	{
		friendSpaceShipState_ = SLOW_MOTION;
		newPosition_ = endPosition_;
	}
}

bool FriendSpaceShipClass::checkArrivedObjective()
{
	if(position_.x < newPosition_.x+0.05 && position_.x > newPosition_.x-0.05 && position_.z < newPosition_.z+0.05 && position_.z > newPosition_.z-0.05)
	{
		return true;
	}

	return false;
}

void FriendSpaceShipClass::freeIt(float elapsedTime)
{
	if(friendSpaceShipState_ == STUCK || friendSpaceShipState_ == SLOW_MOTION)
	{
		touched_ = true;
	}
}

void FriendSpaceShipClass::makeItFree()
{
	if(friendSpaceShipState_ == STUCK || friendSpaceShipState_ == SLOW_MOTION)
	{
		friendSpaceShipState_ = GOING_AWAY;
		newPosition_ = endPosition_;
		soundManager_->playFile("nau_alliberada", false);
		notifyListeners(true);
	}
}

bool FriendSpaceShipClass::isOut()
{
	if(friendSpaceShipState_ == OUTSIDE)
	{
		return true;
	}

	return false;
}

bool FriendSpaceShipClass::hasCelebrated()
{
	if(friendSpaceShipState_ == CELEBRATING_OUTSIDE)
	{
		return true;
	}

	return false;
}

bool FriendSpaceShipClass::isStuck()
{
	if(friendSpaceShipState_ == STUCK || friendSpaceShipState_ == SLOW_MOTION)
	{
		return true;
	}

	return false;
}

bool FriendSpaceShipClass::isWaiting()
{
	if(friendSpaceShipState_ == WAITING)
	{
		return true;
	}

	return false;
}

void FriendSpaceShipClass::setScale(Vector scale)
{
	scaling_.x = scale.x;
	scaling_.y = scale.y;
	scaling_.z = scale.z;
}

Vector FriendSpaceShipClass::getScale()
{
	return scaling_;
}

void FriendSpaceShipClass::setRotationX(float rotX)
{
	rotX_ = rotX;
}

float FriendSpaceShipClass::getRotationX()
{
	return rotX_;
}

void FriendSpaceShipClass::setRotationY(float rotY)
{
	rotY_ = rotY;
}

float FriendSpaceShipClass::getRotationY()
{
	return rotY_;
}

void FriendSpaceShipClass::setRotationZ(float rotZ)
{
	rotZ_ = rotZ;
}

float FriendSpaceShipClass::getRotationZ()
{
	return rotZ_;
}

void FriendSpaceShipClass::setType(DirtColor color)
{
	switch(color)
	{
		case BLUE:
			{
				dirtColor_ = BLUE;
				dirt_->setParticleColor(XMFLOAT4(0.2f, 0.2f, 1.0f, 1.0f));
			}
			break;
		case GREEN:
			{
				dirtColor_ = GREEN;
				dirt_->setParticleColor(XMFLOAT4(0.1f, 1.0f, 0.1f, 1.0f));
			}
			break;
		case RED:
			{
				dirtColor_ = RED;
				dirt_->setParticleColor(XMFLOAT4(1.0f, 0.1f, 0.1f, 1.0f));
			}
			break;
		default:
			{
				dirtColor_ = BLUE;
				dirt_->setParticleColor(XMFLOAT4(0.1f, 0.1f, 0.9f, 1.0f));
			}
			break;
	}
}

Point FriendSpaceShipClass::getViewportPosition()
{
	return viewportPosition_;
}
