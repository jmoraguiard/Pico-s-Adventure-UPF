#include "fruitClass.h"

FruitClass::FruitClass()
{
	model_ = 0;

	logicFruitID_ = 0;

	initialPosition_.x = 0.0f;
	initialPosition_.y = 0.0f;
	initialPosition_.z = 0.0f;

	position_.x = 0.0f;
	position_.y = 0.0f;
	position_.z = 0.0f;

	velocity_.x = 0.0f;
	velocity_.y = 0.0f;
	velocity_.z = 0.0f;

	scaling_.x = 1.0f;
	scaling_.y = 1.0f;
	scaling_.z = 1.0f;

	initialScaling_.x = 1.0f;
	initialScaling_.y = 1.0f;
	initialScaling_.z = 1.0f;

	initialSpawningTime_ = 0.0f;
	spawningTime_ = 0.0f;
	waitedTime_ = 0.0f;

	shakenTime_ = 0.0f;
	initialFallTime_ = 0.0f;
	fallTime_ = 0.0f;
	
	rotX_ = 0.0f;
	rotY_ = 0.0f; 
	rotZ_ = 0.0f;

	leafs_ = 0;

	textureEffect_ = 0;
	hatEffect_ = 0;
}

FruitClass::FruitClass(const FruitClass& other)
{
}

FruitClass::~FruitClass()
{
}

bool FruitClass::setup(GraphicsManager *graphicsManager, SoundFirstClass* soundManager, std::string fileName, Point position, float floorHeight, Vector scaling, float rotX, float rotY, float rotZ)
{
	model_ = Object3DFactory::Instance()->CreateObject3D("StaticObject3D", graphicsManager, fileName);

	name_ = fileName;

	initialPosition_.x = position.x;
	initialPosition_.y = position.y;
	initialPosition_.z = position.z;

	position_.x = position.x;
	position_.y = position.y;
	position_.z = position.z;

	endScaling_.x = scaling.x;
	endScaling_.y = scaling.y;
	endScaling_.z = scaling.z;

	initialScaling_.x = 0.0001f;
	initialScaling_.y = 0.0001f;
	initialScaling_.z = 0.0001f;

	scaling_.x = initialScaling_.x;
	scaling_.y = initialScaling_.y;
	scaling_.z = initialScaling_.z;

	initialSpawningTime_ = 4.0f;
	spawningTime_ = initialSpawningTime_;

	initialFallTime_ = 0.15f;
	fallTime_ = initialFallTime_;
	shaken_ = false;
	
	rotX_ = rotX;
	rotY_ = rotY; 
	rotZ_ = rotZ;

	fruitState_ = SPAWNING;

	floorHeight_ = floorHeight;

	leafs_ = new ParticleSystem;
	if(!leafs_)
	{
		MessageBoxA(NULL, "Could not create leafs instance", "Fruit - Error", MB_ICONERROR | MB_OK);
	}

	if(leafs_ && !leafs_->setup(graphicsManager, "hoja2", initialPosition_, 2.8, 30, 90, XMFLOAT4(0.15f, 0.5f, 0.15f, 1.0f), true))
	{
		MessageBoxA(NULL, "Could not setup leafs object", "Fruit - Error", MB_ICONERROR | MB_OK);
	}

	collisionTest_ = new SphereCollision();
	collisionTest_->setup(graphicsManager, Point(0.0f, 0.4f, 0.0f), 0.4f);

	hasFallen_ = false;

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

	return true;
}

void FruitClass::update(float elapsedTime)
{
	collisionTest_->setPosition(position_);

	alertClock_->tick();
	if(inAlertMode_)
	{
		if(alertClock_->getTime() > alertTime_)
		{
			alertDisplay_ = !alertDisplay_;
			alertClock_->reset();
		}
	}

	switch(fruitState_)
	{
		case SPAWNING:
			{
				waitedTime_ += elapsedTime;

				if(waitedTime_ > spawningTime_)
				{
					scaling_.x = endScaling_.x;
					scaling_.y = endScaling_.y;
					scaling_.z = endScaling_.z;

					shakenTime_ = 0;

					fruitState_ = IN_TREE;
				}
				else
				{
					scaling_.x = (waitedTime_*endScaling_.x)/spawningTime_;
					scaling_.y = (waitedTime_*endScaling_.y)/spawningTime_;
					scaling_.z = (waitedTime_*endScaling_.z)/spawningTime_;
				}
			}
			break;
		case IN_TREE:
			{
				if(shaken_)
				{
					if(shakenTime_ == 0)
					{
						LogClass::Instance()->addEntry("Fruit_Touch", gameLevel_, logicFruitID_);
					}

					shakenTime_ += elapsedTime;
					shaken_ = false;
					leafs_->update(elapsedTime, true);
					return;
				}

				shakenTime_ -= 0.004;
				if(shakenTime_ < 0.0)
				{
					shakenTime_ = 0.0f;
				}
			}
			break;
		case FALLING:
			{
				position_.x += velocity_.x*elapsedTime;
				position_.y += velocity_.y*elapsedTime;
				position_.z += velocity_.z*elapsedTime;

				inAlertMode_ = false;

				if(position_.y < floorHeight_)
				{
					position_.y = floorHeight_;
					fruitState_ = IN_FLOOR;
					notifyListeners(position_);
				}
			}
			break;
		case IN_FLOOR:
			{
				
			}
			break;
	}

	leafs_->update(elapsedTime, false);
}

void FruitClass::draw(GraphicsManager* graphicsManager, XMFLOAT4X4 worldMatrix, XMFLOAT4X4 viewMatrix, XMFLOAT4X4 projectionMatrix, LightClass* light, bool debug)
{
	if(debug)
	{
		graphicsManager->turnOnWireframeRasterizer();
		collisionTest_->draw(graphicsManager->getDevice(), graphicsManager->getDeviceContext(), worldMatrix, viewMatrix, projectionMatrix, light);
		graphicsManager->turnOnSolidRasterizer();
	}

	graphicsManager->turnOnParticlesAlphaBlending();
	graphicsManager->turnZBufferOff();
		leafs_->draw(graphicsManager->getDeviceContext(), worldMatrix, viewMatrix, projectionMatrix, light);
	graphicsManager->turnZBufferOn();
	graphicsManager->turnOffAlphaBlending();

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

void FruitClass::destroy()
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

	if(textureEffect_)
	{
		textureEffect_->destroy();
		delete textureEffect_;
		textureEffect_ = 0;
	}
}

std::string FruitClass::getName()
{
	return name_;
}

int FruitClass::getLogicID()
{
	return logicFruitID_;
}

void FruitClass::setLogicID(int id)
{
	logicFruitID_ = id;
}

void FruitClass::setLevel(int level)
{
	gameLevel_ = level;
}

void FruitClass::shakeIt()
{
	if(fruitState_ == IN_TREE)
	{
		shaken_ = true;
		inAlertMode_ = false;

		if(shakenTime_ > fallTime_)
		{
			makeItFall();
		}
	}
}

bool FruitClass::makeItFall()
{
	if(fruitState_ == IN_TREE)
	{
		velocity_.x = 0.0f;
		velocity_.y = -1.6f;
		velocity_.z = -0.3f;

		spawningTime_ += 1.0f;

		soundManager_->playFruitFall();

		LogClass::Instance()->addEntry("Fruit_Fall", gameLevel_, logicFruitID_);

		inAlertMode_ = false;

		fruitState_ = FALLING;
		hasFallen_ = true;
		return true;
	}
	return false;
}

void FruitClass::resetFruit()
{
	position_.x = initialPosition_.x;
	position_.y = initialPosition_.y;
	position_.z = initialPosition_.z;

	scaling_.x = initialScaling_.x;
	scaling_.y = initialScaling_.y;
	scaling_.z = initialScaling_.z;

	waitedTime_ = 0.0f;

	fruitState_ = SPAWNING;
}

void FruitClass::resetSpawningTime()
{
	spawningTime_ = initialSpawningTime_;
}

void FruitClass::setPosition(Point position)
{
	position_.x = position.x;
	position_.y = position.y;
	position_.z = position.z;
}

Point FruitClass::getPosition()
{
	return position_;
}

void FruitClass::setScale(Vector scale)
{
	scaling_.x = scale.x;
	scaling_.y = scale.y;
	scaling_.z = scale.z;
}

Vector FruitClass::getScale()
{
	return scaling_;
}

void FruitClass::setRotationX(float rotX)
{
	rotX_ = rotX;
}

float FruitClass::getRotationX()
{
	return rotX_;
}

void FruitClass::setRotationY(float rotY)
{
	rotY_ = rotY;
}

float FruitClass::getRotationY()
{
	return rotY_;
}

void FruitClass::setRotationZ(float rotZ)
{
	rotZ_ = rotZ;
}

float FruitClass::getRotationZ()
{
	return rotZ_;
}

SphereCollision* FruitClass::getCollisionSphere()
{
	return collisionTest_;
}

void FruitClass::setFruitEffectType(FruitEffect effect)
{
	fruitEffect_ = effect;
}

FruitEffect FruitClass::getFruitEffect()
{
	return fruitEffect_;
}

void FruitClass::setColorEffect(XMFLOAT4 color)
{
	colorEffect_ = color;
}

XMFLOAT4 FruitClass::getColorEffect()
{
	return colorEffect_;
}

void FruitClass::setTextureEffect(TextureClass* texture)
{
	textureEffect_ = texture;
}

TextureClass* FruitClass::getTextureEffect()
{
	return textureEffect_;
}

void FruitClass::setHatEffect(Object3D* hat)
{
	hatEffect_ = hat;
}

Object3D* FruitClass::getHatEffect()
{
	return hatEffect_;
}

void FruitClass::setBodyEffect(Object3D* body, Object3D* tips)
{
	bodyEffect_ = body;
	tipsEffect_ = tips;
}

Object3D* FruitClass::getBodyEffect()
{
	return bodyEffect_;
}

Object3D* FruitClass::getTipsEffect()
{
	return tipsEffect_;
}

bool FruitClass::hasFallen()
{
	return hasFallen_;
}

void FruitClass::activateAlert(bool active)
{
	inAlertMode_ = active;
}

