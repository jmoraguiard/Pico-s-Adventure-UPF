#include "platformClass.h"

PlatformClass::PlatformClass()
{
	model_ = 0;

	initialPosition_.x = 0.0f;
	initialPosition_.y = 0.2f;
	initialPosition_.z = -3.25f;

	position_ = initialPosition_;

	velocity_.x = 0.0f;
	velocity_.y = 0.0f;
	velocity_.z = 0.0f;

	objective_.x = 0.0f;
	objective_.y = 0.0f;
	objective_.z = 0.0f;

	scaling_.x = 0.007f;
	scaling_.y = 0.006f;
	scaling_.z = 0.006f;
	
	rotX_ = 0.0f;
	rotY_ = 0.0f; 
	rotZ_ = 0.0f;
}

PlatformClass::PlatformClass(const PlatformClass& other)
{
}

PlatformClass::~PlatformClass()
{
}

bool PlatformClass::setup(GraphicsManager* graphicsManager, SoundFourthClass* soundManager)
{
	soundManager_ = soundManager;

	model_ = Object3DFactory::Instance()->CreateObject3D("StaticObject3D", graphicsManager, "plataforma");

	platformParticles_[0] = new ParticleSystem;
	if(!platformParticles_[0])
	{
		MessageBoxA(NULL, "Could not create light1 particles instance", "SecondScreen - Error", MB_ICONERROR | MB_OK);
	}
	if(platformParticles_[0] && !platformParticles_[0]->setup(graphicsManager, "star", Point(0.0f, 0.0f, 0.0f), 1.5, 200, 200, XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), true))
	{
		MessageBoxA(NULL, "Could not setup light1 particles object", "SecondScreen - Error", MB_ICONERROR | MB_OK);
	}

	platformParticles_[1] = new ParticleSystem;
	if(!platformParticles_[1])
	{
		MessageBoxA(NULL, "Could not create light1 particles instance", "SecondScreen - Error", MB_ICONERROR | MB_OK);
	}
	if(platformParticles_[1] && !platformParticles_[1]->setup(graphicsManager, "star", Point(0.0f, 0.0f, 0.0f), 1.5, 200, 200, XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), true))
	{
		MessageBoxA(NULL, "Could not setup light1 particles object", "SecondScreen - Error", MB_ICONERROR | MB_OK);
	}

	platformState_ = WAITING;

	return true;
}

void PlatformClass::update(float elapsedTime)
{
	platformParticles_[0]->setPosition(Point(position_.x+0.65f, position_.y-0.2f, position_.z));
	platformParticles_[1]->setPosition(Point(position_.x-0.65f, position_.y-0.2f, position_.z));
	
	switch(platformState_)
	{
		case WAITING:
			{
				platformParticles_[0]->update(elapsedTime, false);
				platformParticles_[1]->update(elapsedTime, false);
			}
			break;
		case GOING_TO:
			{
				platformParticles_[0]->update(elapsedTime, true);
				platformParticles_[1]->update(elapsedTime, true);

				velocity_.x = objective_.x - position_.x;
				velocity_.y = objective_.y - position_.y;
				velocity_.z = objective_.z - position_.z;

				Vector normalizedVelocity = velocity_.normalize();
				velocity_ = normalizedVelocity*2.0f;

				position_.x += velocity_.x*elapsedTime;
				position_.y += velocity_.y*elapsedTime;
				position_.z += velocity_.z*elapsedTime;

				if(checkArrivedObjective())
				{
					platformState_ = GETTING_PRESENT;
				}
				else
				{	
					objective_ = initialPosition_;
					platformState_ = COMING_BACK;
				}
			}
			break;
		case GETTING_PRESENT:
			{
				platformParticles_[0]->update(elapsedTime, true);
				platformParticles_[1]->update(elapsedTime, true);
			}
			break;
		case COMING_BACK:
			{
				platformParticles_[0]->update(elapsedTime, true);
				platformParticles_[1]->update(elapsedTime, true);

				velocity_.x = objective_.x - position_.x;
				velocity_.y = objective_.y - position_.y;
				velocity_.z = objective_.z - position_.z;

				Vector normalizedVelocity = velocity_.normalize();
				velocity_ = normalizedVelocity*0.35f;

				position_.x += velocity_.x*elapsedTime;
				position_.y += velocity_.y*elapsedTime;
				position_.z += velocity_.z*elapsedTime;

				if(checkArrivedObjective())
				{
					position_ = initialPosition_;
					platformState_ = WAITING;
				}
			}
			break;
		case BRINGING_BACK:
			{
				platformParticles_[0]->update(elapsedTime, true);
				platformParticles_[1]->update(elapsedTime, true);

				velocity_.x = objective_.x - position_.x;
				velocity_.y = objective_.y - position_.y;
				velocity_.z = objective_.z - position_.z;

				Vector normalizedVelocity = velocity_.normalize();
				velocity_ = normalizedVelocity*1.75f;

				position_.x += velocity_.x*elapsedTime;
				position_.y += velocity_.y*elapsedTime;
				position_.z += velocity_.z*elapsedTime;

				if(checkArrivedObjective())
				{
					position_ = initialPosition_;
					platformState_ = WAITING;
				}
			}
			break;
		default:
			{

			}
			break;
	}
}

void PlatformClass::draw(GraphicsManager* graphicsManager, XMFLOAT4X4 worldMatrix, XMFLOAT4X4 viewMatrix, XMFLOAT4X4 projectionMatrix, LightClass* light, bool debug)
{
	if(platformState_ != WAITING)
	{
		graphicsManager->turnOnParticlesAlphaBlending();
		graphicsManager->turnZBufferOff();
			platformParticles_[0]->draw(graphicsManager->getDeviceContext(), worldMatrix, viewMatrix, projectionMatrix, light);
			platformParticles_[1]->draw(graphicsManager->getDeviceContext(), worldMatrix, viewMatrix, projectionMatrix, light);
		graphicsManager->turnZBufferOn();
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

void PlatformClass::destroy()
{
	// Release the model object
	if(model_)
	{
		model_->destroy();
		delete model_;
		model_ = 0;
	}
}

void PlatformClass::goToPosition(Point position)
{
	if(platformState_ != GETTING_PRESENT)
	{
		objective_.x = position.x;
		objective_.y = position.y;
		objective_.z = position.z;

		platformState_ = GOING_TO;
	}
}

void PlatformClass::bringBackPresent()
{
	if(platformState_ == GETTING_PRESENT)
	{
		objective_.x = initialPosition_.x;
		objective_.y = initialPosition_.y;
		objective_.z = initialPosition_.z;

		platformState_ = BRINGING_BACK;
	}
}

bool PlatformClass::checkArrivedObjective()
{
	if(position_.x < objective_.x+0.05 && position_.x > objective_.x-0.05 && position_.z < objective_.z+0.05 && position_.z > objective_.z-0.05)
	{
		return true;
	}

	return false;
}

Point PlatformClass::getPosition()
{
	return position_;
}

bool PlatformClass::isGettingPresent()
{
	if(platformState_ == GETTING_PRESENT)
	{
		return true;
	}

	return false;
}

bool PlatformClass::isWaiting()
{
	if(platformState_ == WAITING)
	{
		return true;
	}

	return false;
}
