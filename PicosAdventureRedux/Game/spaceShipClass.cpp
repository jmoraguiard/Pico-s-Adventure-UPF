#include "spaceShipClass.h"

SpaceShipClass::SpaceShipClass()
{
	model_ = 0;

	initialPosition_.x = 0.0f;
	initialPosition_.y = 0.0f;
	initialPosition_.z = 0.0f;

	position_.x = 0.0f;
	position_.y = 0.0f;
	position_.z = 0.0f;

	velocity_.x = 0.0f;
	velocity_.y = 0.0f;
	velocity_.z = 0.0f;

	objective_.x = 0.0f;
	objective_.y = 0.0f;
	objective_.z = 0.0f;

	scaling_.x = 1.0f;
	scaling_.y = 1.0f;
	scaling_.z = 1.0f;
	
	rotX_ = 0.0f;
	rotY_ = 0.0f; 
	rotZ_ = 0.0f;

	enlargedInitialPosition_.x = 0.0f;
	enlargedInitialPosition_.y = 0.0f;
	enlargedInitialPosition_.z = 0.0f;

	enlargedInitialScale_.x = 1.0f;
	enlargedInitialScale_.y = 1.0f;
	enlargedInitialScale_.z = 1.0f;

	enlargedFinalPosition_.x = 0.0f;
	enlargedFinalPosition_.y = 0.0f;
	enlargedFinalPosition_.z = 0.0f;

	enlargedFinalScale_.x = 2.0f;
	enlargedFinalScale_.y = 2.0f;
	enlargedFinalScale_.z = 2.0f;
}

SpaceShipClass::SpaceShipClass(const SpaceShipClass& other)
{
}

SpaceShipClass::~SpaceShipClass()
{
}

bool SpaceShipClass::setup(GraphicsManager* graphicsManager, SoundSecondClass* soundManager, float velocityConstant)
{
	soundManager_ = soundManager;

	model_ = Object3DFactory::Instance()->CreateObject3D("AnimatedObject3D", graphicsManager, "nave");

	AnimatedObject3D* animatedTemp = dynamic_cast<AnimatedObject3D*>(model_);
	AnimatedCal3DModelClass* cal3dTemp = dynamic_cast<AnimatedCal3DModelClass*>(animatedTemp->getModel());
	cal3dTemp->setAnimationToPlay("nave", 0.4f);

	spaceShipParticles_ = new ParticleSystem;
	if(!spaceShipParticles_)
	{
		MessageBoxA(NULL, "Could not create light1 particles instance", "SpaceShip - Error", MB_ICONERROR | MB_OK);
	}
	if(spaceShipParticles_ && !spaceShipParticles_->setup(graphicsManager, "star", getPosition(), 1.5, 200, 200, XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), true))
	{
		MessageBoxA(NULL, "Could not setup light1 particles object", "SpaceShip - Error", MB_ICONERROR | MB_OK);
	}

	orange_ = new TextureClass;
	if(!orange_)
	{
		MessageBoxA(NULL, "Could not create orange_ texture!", "SpaceShip - Error", MB_ICONERROR | MB_OK);
		return false;
	}
	std::string filePath = "./Data/models/nave/d-nave-amarilla.dds";
	bool result = orange_->setup(graphicsManager->getDevice(), filePath);
	if(!result)
	{
		MessageBoxA(NULL, "Could not load orange_ texture!", "SpaceShip - Error", MB_ICONERROR | MB_OK);
		return false;
	}

	blue_ = new TextureClass;
	if(!blue_)
	{
		MessageBoxA(NULL, "Could not create blue_ texture!", "SpaceShip - Error", MB_ICONERROR | MB_OK);
		return false;
	}
	filePath = "./Data/models/nave/d-nave-azul.dds";
	result = blue_->setup(graphicsManager->getDevice(), filePath);
	if(!result)
	{
		MessageBoxA(NULL, "Could not load blue_ texture!", "SpaceShip - Error", MB_ICONERROR | MB_OK);
		return false;
	}

	green_ = new TextureClass;
	if(!green_)
	{
		MessageBoxA(NULL, "Could not create green_ texture!", "SpaceShip - Error", MB_ICONERROR | MB_OK);
		return false;
	}
	filePath = "./Data/models/nave/d-nave-verde.dds";
	result = green_->setup(graphicsManager->getDevice(), filePath);
	if(!result)
	{
		MessageBoxA(NULL, "Could not load green_ texture!", "SpaceShip - Error", MB_ICONERROR | MB_OK);
		return false;
	}

	enlargingTime_ = 6.0f;

	velocityConstant_ = velocityConstant;

	spaceShipState_ = WAITING;

	return true;
}

void SpaceShipClass::update(float elapsedTime)
{
	model_->update(elapsedTime);

	switch(spaceShipState_)
	{
		case WAITING:
			{

			}
			break;
		case LAUNCHING:
			{
				spaceShipParticles_->setPosition(Point(position_.x, position_.y+0.1, position_.z));
				spaceShipParticles_->update(elapsedTime, true);

				position_.y += velocity_.y*elapsedTime;

				velocity_.y -= 2.0f*elapsedTime;

				if(position_.y < floorHeight_)
				{
					soundManager_->playFile("piece_fall", false);

					position_.y = floorHeight_;

					spaceShipState_ = WAITING;
				}
			}
			break;
		case FLYING:
			{
				spaceShipParticles_->setPosition(Point(position_.x, position_.y+0.1, position_.z));
				spaceShipParticles_->update(elapsedTime, true);

				fly(elapsedTime);
			}
			break;
		case ENLARGING:
			{
				spaceShipParticles_->setPosition(Point(position_.x, position_.y+0.1, position_.z));
				spaceShipParticles_->update(elapsedTime, true);

				enlargingElapsedTime_ += elapsedTime;

				if(enlargingElapsedTime_ < enlargingTime_)
				{
					Point movement;
					movement.x = enlargedFinalPosition_.x - enlargedInitialPosition_.x;
					movement.y = enlargedFinalPosition_.y - enlargedInitialPosition_.y;
					movement.z = enlargedFinalPosition_.z - enlargedInitialPosition_.z;

					position_.x = enlargedInitialPosition_.x + movement.x * (enlargingElapsedTime_/enlargingTime_);
					position_.y = enlargedInitialPosition_.y + movement.y * (enlargingElapsedTime_/enlargingTime_);
					position_.z = enlargedInitialPosition_.z + movement.z * (enlargingElapsedTime_/enlargingTime_);

					Vector scaling;
					scaling.x = enlargedFinalScale_.x - enlargedInitialScale_.x;
					scaling.y = enlargedFinalScale_.y - enlargedInitialScale_.y;
					scaling.z = enlargedFinalScale_.z - enlargedInitialScale_.z;

					scaling_.x = enlargedInitialScale_.x + scaling.x * (enlargingElapsedTime_/enlargingTime_);
					scaling_.y = enlargedInitialScale_.y + scaling.y * (enlargingElapsedTime_/enlargingTime_);
					scaling_.z = enlargedInitialScale_.z + scaling.z * (enlargingElapsedTime_/enlargingTime_);
				}
				else
				{
					position_ = enlargedFinalPosition_;
					scaling_ = enlargedFinalScale_;

					spaceShipState_ = PREPARED;
				}
			}
			break;
		case PREPARED:
			{
				spaceShipParticles_->setPosition(Point(position_.x, position_.y+0.1, position_.z));
				spaceShipParticles_->update(elapsedTime, true);
			}
		default:
			{

			}
			break;
	}
}

void SpaceShipClass::draw(GraphicsManager* graphicsManager, XMFLOAT4X4 worldMatrix, XMFLOAT4X4 viewMatrix, XMFLOAT4X4 projectionMatrix, LightClass* light, bool debug)
{
	if(spaceShipState_ != WAITING)
	{
		graphicsManager->turnOnParticlesAlphaBlending();
		//graphicsManager->turnZBufferOff();
			spaceShipParticles_->draw(graphicsManager->getDeviceContext(), worldMatrix, viewMatrix, projectionMatrix, light);
		//graphicsManager->turnZBufferOn();
		graphicsManager->turnOffAlphaBlending();
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

	model_->draw(graphicsManager->getDevice(), graphicsManager->getDeviceContext(), worldMatrix, viewMatrix, projectionMatrix, light);
}

void SpaceShipClass::destroy()
{
	// Release the model object
	if(model_)
	{
		model_->destroy();
		delete model_;
		model_ = 0;
	}
}

void SpaceShipClass::goToPosition(Point position)
{
	objective_.x = position.x;
	objective_.y = position.y;
	objective_.z = position.z;

	spaceShipState_ = FLYING;
}

void SpaceShipClass::makeLaunch(int level)
{
	if(spaceShipState_ == WAITING)
	{
		std::stringstream sound;
		sound << "fuel" << level;
		soundManager_->playFile(sound.str(), false);

		velocity_.x = 0.0f;
		velocity_.y = level*1.0f;
		velocity_.z = 0.0f;

		spaceShipState_ = LAUNCHING;
	}
}

void SpaceShipClass::makeBig(Point newPos, Vector newScale)
{
	enlargedInitialPosition_ = position_;
	enlargedInitialScale_ = scaling_;
	enlargedFinalPosition_ = newPos;
	enlargedFinalScale_ = newScale;

	enlargingElapsedTime_ = 0.0f;

	spaceShipState_ = ENLARGING;
}

bool SpaceShipClass::isPrepared()
{
	if(spaceShipState_ == PREPARED)
	{
		return true;
	}

	return false;
}

Object3D* SpaceShipClass::getObject()
{
	return model_;
}

void SpaceShipClass::setInitialPosition(Point pos)
{
	initialPosition_.x = pos.x;
	initialPosition_.y = pos.y;
	initialPosition_.z = pos.z;
}

void SpaceShipClass::setPosition(Point pos)
{
	position_.x = pos.x;
	position_.y = pos.y;
	position_.z = pos.z;
}

Point SpaceShipClass::getPosition()
{
	return position_;
}

void SpaceShipClass::setScale(Vector scale)
{
	scaling_.x = scale.x;
	scaling_.y = scale.y;
	scaling_.z = scale.z;
}

void SpaceShipClass::setRotation(float x, float y, float z)
{
	rotX_ = x;
	rotY_ = y;
	rotZ_ = z;
}

void SpaceShipClass::setFloorHeight(float floor)
{
	floorHeight_ = floor;
}

void SpaceShipClass::setSpaceShipState(SpaceShipState state)
{
	spaceShipState_ = state;
}

void SpaceShipClass::fly(float elapsedTime)
{
	velocity_.x = objective_.x - position_.x;
	velocity_.y = objective_.y - position_.y;
	velocity_.z = objective_.z - position_.z;

	Vector normalizedVelocity = velocity_.normalize();
	velocity_ = normalizedVelocity*velocityConstant_;

	//rotY_ = acos(normalizedVelocity.z);
	/*if(normalizedVelocity.x < 0)
	{
		rotY_ += 3.141592f;
	}*/

	position_.x += velocity_.x*elapsedTime;
	position_.y += velocity_.y*elapsedTime;
	position_.z += velocity_.z*elapsedTime;
}

bool SpaceShipClass::checkArrivedObjective()
{
	if(position_.x < objective_.x+0.05 && position_.x > objective_.x-0.05 && position_.z < objective_.z+0.05 && position_.z > objective_.z-0.05)
	{
		return true;
	}

	return false;
}

void SpaceShipClass::setTexture(std::string color)
{
	if(color == "redSpaceShip")
		return;
	if(color == "orangeSpaceShip")
	{
		model_->getTextureArrayClass()->getTexturesArray()[0] = orange_->getTexture();
	}
	if(color == "greenSpaceShip")
	{
		model_->getTextureArrayClass()->getTexturesArray()[0] = green_->getTexture();
	}
	if(color == "blueSpaceShip")
	{
		model_->getTextureArrayClass()->getTexturesArray()[0] = blue_->getTexture();
	}
}

