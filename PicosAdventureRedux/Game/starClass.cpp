#include "starClass.h"

StarClass::StarClass()
{
	model_ = 0;

	initialPosition_.x = 100.0f;
	initialPosition_.y = 100.0f;
	initialPosition_.z = 100.0f;

	finalPosition_.x = 100.0f;
	finalPosition_.y = 100.0f;
	finalPosition_.z = 100.0f;

	position_.x = 0.0f;
	position_.y = 0.0f;
	position_.z = 0.0f;

	velocity_.x = 0.0f;
	velocity_.y = 0.0f;
	velocity_.z = 0.0f;

	scaling_.x = 1.0f;
	scaling_.y = 1.0f;
	scaling_.z = 1.0f;
	
	rotX_ = 0.0f;
	rotY_ = 0.0f; 
	rotZ_ = 0.0f;

	badlight_ = 0;
}

StarClass::StarClass(const StarClass& other)
{
}

StarClass::~StarClass()
{
}

bool StarClass::setup(GraphicsManager *graphicsManager, SoundSecondClass* soundManager, float floorHeight)
{
	model_ = Object3DFactory::Instance()->CreateObject3D("StaticObject3D", graphicsManager, "estrella");

	soundManager_ = soundManager;

	scaling_.x = 0.0065f;
	scaling_.y = 0.0065f;
	scaling_.z = 0.0065f;

	starState_ = IN_SKY;

	floorHeight_ = floorHeight;

	collisionTest_ = new SphereCollision();
	collisionTest_->setup(graphicsManager, Point(0.0f, 0.4f, 0.0f), 1.2f);

	hasFallen_ = false;
	good_ = false;

	// Create the light object.
	badlight_ = new LightClass;
	if(!badlight_)
	{
		return false;
	}

	badlight_->setAmbientColor(0.0f, 0.0f, 0.0f, 1.0f);
	badlight_->setDiffuseColor(0.9f, 0.1f, 0.1f, 1.0f);
	badlight_->setDirection(0.0f, -1.0f, 1.0f);

	return true;
}

void StarClass::update(float elapsedTime)
{
	collisionTest_->setPosition(position_);

	switch(starState_)
	{
		case IN_SKY:
			{
				
			}
			break;
		case FALLING:
			{
				velocity_.x = finalPosition_.x - initialPosition_.x;
				velocity_.y = finalPosition_.y - initialPosition_.y;
				velocity_.z = finalPosition_.z - initialPosition_.z;

				Vector normalizedVelocity = velocity_.normalize();
				velocity_ = normalizedVelocity * 1.5f;

				position_.x += velocity_.x*elapsedTime;
				position_.y += velocity_.y*elapsedTime;
				position_.z += velocity_.z*elapsedTime;

				if(position_.y < floorHeight_)
				{
					position_.y = floorHeight_;
					starState_ = IN_FLOOR;
				}
			}
			break;
		case GO_TO_SPACESHIP:
			{
				velocity_.x = -4.0f - position_.x;
				velocity_.y = floorHeight_ - position_.y;
				velocity_.z = -3.25f - position_.z;

				Vector normalizedVelocity = velocity_.normalize();
				velocity_ = normalizedVelocity * 8.0f;

				position_.x += velocity_.x*elapsedTime;
				position_.y += velocity_.y*elapsedTime;
				position_.z += velocity_.z*elapsedTime;

				if(position_.x < -3.8f)
				{
					starState_ = IN_FLOOR;
				}
			}
			break;
		case IN_FLOOR:
			{
				position_.x = initialPosition_.x;
				position_.y = initialPosition_.y;
				position_.z = initialPosition_.z;

				starState_ = IN_SKY;
			}
			break;
		default:
			{
			}
			break;
	}
}

void StarClass::draw(GraphicsManager* graphicsManager, XMFLOAT4X4 worldMatrix, XMFLOAT4X4 viewMatrix, XMFLOAT4X4 projectionMatrix, LightClass* light, bool debug)
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

	if(starState_ != IN_FLOOR)
	{
		if(!good_)
		{
			model_->draw(graphicsManager->getDevice(), graphicsManager->getDeviceContext(), worldMatrix, viewMatrix, projectionMatrix, badlight_);
		}
		else
		{
			model_->draw(graphicsManager->getDevice(), graphicsManager->getDeviceContext(), worldMatrix, viewMatrix, projectionMatrix, light);
		}
	}
}

void StarClass::destroy()
{
	// Release the model object
	if(model_)
	{
		model_->destroy();
		delete model_;
		model_ = 0;
	}

	// Release the collision object
	if(collisionTest_)
	{
		collisionTest_->destroy();
		delete collisionTest_;
		collisionTest_ = 0;
	}

	if(badlight_)
	{
		delete badlight_;
		badlight_ = 0;
	}
}

void StarClass::makeItFall(bool good)
{
	if(starState_ == IN_SKY)
	{
		position_.x = initialPosition_.x;
		position_.y = initialPosition_.y;
		position_.z = initialPosition_.z;

		good_ = good;

		soundManager_->playFile("falling_star", false);

		starState_ = FALLING;
		hasFallen_ = true;
	}
}

bool StarClass::isFalling()
{
	if(starState_ == FALLING)
	{
		return true;
	}

	return false;
}

bool StarClass::hasFallen()
{
	return hasFallen_;
}

bool StarClass::isInTheFloor()
{
	if(starState_ == IN_FLOOR)
	{
		return true;
	}

	return false;
}

bool StarClass::isInTheSky()
{
	if(starState_ == IN_SKY)
	{
		return true;
	}

	return false;
}

bool StarClass::isGood()
{
	return good_;
}

void StarClass::reset()
{
	starState_ = IN_FLOOR;

	position_.x = initialPosition_.x;
	position_.y = initialPosition_.y;
	position_.z = initialPosition_.z;

	collisionTest_->setPosition(position_);
}

void StarClass::goToSpaceShip()
{
	starState_ = GO_TO_SPACESHIP;
}

void StarClass::setInitialPosition(Point position)
{
	initialPosition_.x = position.x;
	initialPosition_.y = position.y;
	initialPosition_.z = position.z;
}

void StarClass::setFinalPosition(Point position)
{
	finalPosition_.x = position.x;
	finalPosition_.y = position.y;
	finalPosition_.z = position.z;
}

void StarClass::setPosition(Point position)
{
	position_.x = position.x;
	position_.y = position.y;
	position_.z = position.z;
}

Point StarClass::getPosition()
{
	return position_;
}

SphereCollision* StarClass::getCollisionSphere()
{
	return collisionTest_;
}


