#include "birdClass.h"

BirdClass::BirdClass()
{
	model_ = 0;

	fallenFruit_ = 0;

	birdState_ = HIDDEN;

	leftInitialPosition_.x = 0.0f;
	leftInitialPosition_.y = 0.0f;
	leftInitialPosition_.z = 0.0f;

	rightInitialPosition_.x = 0.0f;
	rightInitialPosition_.y = 0.0f;
	rightInitialPosition_.z = 0.0f;

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

	stealFood_ = false;
}

BirdClass::BirdClass(const BirdClass& other)
{
}

BirdClass::~BirdClass()
{
}

bool BirdClass::setup(GraphicsManager* graphicsManager, SoundFirstClass* soundManager)
{
	model_ = Object3DFactory::Instance()->CreateObject3D("AnimatedObject3D", graphicsManager, "pajaro");

	leftInitialPosition_.x = -8.0f;
	leftInitialPosition_.y = 6.5f;
	leftInitialPosition_.z = 3.5f;

	rightInitialPosition_.x = 8.0f;
	rightInitialPosition_.y = 6.5f;
	rightInitialPosition_.z = 3.5f;

	position_.x = leftInitialPosition_.x;
	position_.y = leftInitialPosition_.y;
	position_.z = leftInitialPosition_.z;

	birdState_ = HIDDEN;

	scaling_.x = 1.5f;
	scaling_.y = 1.5f;
	scaling_.z = 1.5f;
	
	rotX_ = 0.0f;
	rotY_ = 3.141592f-1.570796f/4; 
	rotZ_ = 0.0f;

	stealFood_ = false;

	collisionTest_ = new SphereCollision();
	collisionTest_->setup(graphicsManager, Point(0.0f, 0.4f, 0.0f), 1.0f);

	activateAlert_ = new ClockClass();
	if(!activateAlert_)
	{
		return false;
	}
	activateAlert_->reset();
	activateTime_ = 10.0f;

	inAlertMode_ = false;
	alertTime_ = 0.4f;
	alertDisplay_ = true;
	alertClock_ = new ClockClass();
	if(!alertClock_)
	{
		return false;
	}
	alertClock_->reset();

	soundManager_ = soundManager;

	gameLevel_ = 0;

	return true;
}

void BirdClass::update(float elapsedTime)
{
	model_->update(elapsedTime);

	alertClock_->tick();
	if(inAlertMode_)
	{
		if(alertClock_->getTime() > alertTime_)
		{
			alertDisplay_ = !alertDisplay_;
			alertClock_->reset();
		}
	}

	switch(birdState_)
	{
		case HIDDEN:
			{
				if(fallenFruit_ != 0)
				{
					Point fallenFruitPos = fallenFruit_->getPosition();

					if(fallenFruitPos.x <0)
					{
						position_.x = leftInitialPosition_.x;
						position_.y = leftInitialPosition_.y;
						position_.z = leftInitialPosition_.z;
					}
					else
					{
						position_.x = rightInitialPosition_.x;
						position_.y = rightInitialPosition_.y;
						position_.z = rightInitialPosition_.z;
					}

					goToPosition(fallenFruitPos);
					soundManager_->playBirdEnter();
				}
			}
			break;
		case WAITING:
			{
				
			}
			break;
		case FLYING:
			{
				fly(elapsedTime);

				soundManager_->playBirdEnter();

				checkArrivedObjective();
			}
			break;
		case TEASING:
			{
				activateAlert_->tick();
				if(!inAlertMode_ && activateAlert_->getTime() > activateTime_)
				{
					inAlertMode_ = true;

					LogClass::Instance()->addEntry("Bird_Visual_Aid", gameLevel_, 0);
					activateAlert_->reset();
				}
				if(inAlertMode_ && activateAlert_->getTime() > activateTime_)
				{
					soundManager_->playOwl();

					LogClass::Instance()->addEntry("Bird_Sound_Aid", gameLevel_, 0);
					activateAlert_->reset();
				}
			}
			break;
		case RUNNING_AWAY:
			{
				fly(elapsedTime);

				soundManager_->playBirdEnter();

				checkArrivedObjective();
			}
			break;
	}

	collisionTest_->setPosition(position_);
}

void BirdClass::draw(GraphicsManager* graphicsManager, XMFLOAT4X4 worldMatrix, XMFLOAT4X4 viewMatrix, XMFLOAT4X4 projectionMatrix, LightClass* light, bool debug)
{
	if(debug)
	{
		graphicsManager->turnOnWireframeRasterizer();
		collisionTest_->draw(graphicsManager->getDevice(), graphicsManager->getDeviceContext(), worldMatrix, viewMatrix, projectionMatrix, light);
		graphicsManager->turnOnSolidRasterizer();
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

	if(!inAlertMode_ || alertDisplay_)
	{
		model_->draw(graphicsManager->getDevice(), graphicsManager->getDeviceContext(), worldMatrix, viewMatrix, projectionMatrix, light);
	}
}

void BirdClass::destroy()
{
	// Release the model object
	if(model_)
	{
		model_->destroy();
		delete model_;
		model_ = 0;
	}
}

void BirdClass::setLevel(int level)
{
	gameLevel_ = level;
}

void BirdClass::setStealFood(bool steal)
{
	if(steal)
	{
		position_.x = leftInitialPosition_.x;
		position_.y = leftInitialPosition_.y;
		position_.z = leftInitialPosition_.z;

		birdState_ = HIDDEN;
	}

	stealFood_ = steal;
}

bool BirdClass::getStealFood()
{
	return stealFood_;
}

void BirdClass::goToPosition(Point position)
{
	AnimatedObject3D* animatedTemp = dynamic_cast<AnimatedObject3D*>(model_);
	AnimatedCal3DModelClass* cal3dTemp = dynamic_cast<AnimatedCal3DModelClass*>(animatedTemp->getModel());
	cal3dTemp->setAnimationToPlay("fly", 0.4f);

	objective_.x = position.x;
	objective_.y = position.y;
	objective_.z = position.z;

	birdState_ = FLYING;
}

void BirdClass::scared(bool force, bool hand)
{
	if(birdState_ == TEASING || (birdState_ == FLYING && !hand) || force)
	{
		notifyListeners(true);

		if(position_.x > 0)
		{
			goToPosition(leftInitialPosition_);
		}
		else
		{
			goToPosition(rightInitialPosition_);
		}

		fallenFruit_ = 0;

		LogClass::Instance()->addEntry("Bird_Scared", 0, 0);

		birdState_ = RUNNING_AWAY;
		inAlertMode_ = false;
	}
}

bool BirdClass::getIsTeasing()
{
	if(birdState_ == FLYING || birdState_ == TEASING || birdState_ == RUNNING_AWAY)
	{
		return true;
	}
	else if(birdState_ == HIDDEN || birdState_ == WAITING)
	{

	}
	return false;
}

float BirdClass::approach(float goal, float current, float dt)
{
	float difference = goal - current;

	if(difference > dt)
		return current + dt;
	if(difference < dt)
		return current - dt;

	return goal;
}

void BirdClass::fly(float elapsedTime)
{
	velocity_.x = objective_.x - position_.x;
	velocity_.y = objective_.y - position_.y;
	velocity_.z = objective_.z - position_.z;

	Vector normalizedVelocity = velocity_.normalize();
	velocity_ = normalizedVelocity * 10;

	rotY_ = acos(normalizedVelocity.z);
	if(normalizedVelocity.x < 0)
	{
		rotY_ += 3.141592f;
	}

	position_.x += velocity_.x*elapsedTime;
	position_.y += velocity_.y*elapsedTime;
	position_.z += velocity_.z*elapsedTime;
}

void BirdClass::checkArrivedObjective()
{
	if(position_.x < objective_.x+0.05 && position_.x > objective_.x-0.05 && position_.z < objective_.z+0.05 && position_.z > objective_.z-0.05)
	{
		if(birdState_ == FLYING)
		{
			if(fallenFruit_ != 0)
			{
				notifyListeners(false);

				AnimatedObject3D* animatedTemp = dynamic_cast<AnimatedObject3D*>(model_);
				AnimatedCal3DModelClass* cal3dTemp = dynamic_cast<AnimatedCal3DModelClass*>(animatedTemp->getModel());
				cal3dTemp->setAnimationToPlay("walk", 0.2f);

				rotY_ = 3.141592f;

				birdState_ = TEASING;
				activateAlert_->reset();
				soundManager_->playBirdEat();
			}
			else
			{
				AnimatedObject3D* animatedTemp = dynamic_cast<AnimatedObject3D*>(model_);
				AnimatedCal3DModelClass* cal3dTemp = dynamic_cast<AnimatedCal3DModelClass*>(animatedTemp->getModel());
				cal3dTemp->setAnimationToPlay("walk", 0.2f);

				rotY_ = 3.141592f;

				birdState_ = WAITING;
			}
		}
		if(birdState_ == RUNNING_AWAY)
		{
			birdState_ = HIDDEN;
		}
	}
}

SphereCollision* BirdClass::getCollisionSphere()
{
	return collisionTest_;
}

void BirdClass::notify(FruitClass* notifier, Point arg)
{
	if(stealFood_ && fallenFruit_ == 0)
	{
		fallenFruit_ = notifier;
	}
}

void BirdClass::notify(PicoFirstClass* notifier, bool arg)
{
	scared(false, false);
}
