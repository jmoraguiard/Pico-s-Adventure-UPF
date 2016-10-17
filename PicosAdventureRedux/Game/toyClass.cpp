#include "toyClass.h"

ToyClass::ToyClass()
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
}

ToyClass::ToyClass(const ToyClass& other)
{
}

ToyClass::~ToyClass()
{
}

bool ToyClass::setup(GraphicsManager *graphicsManager, SoundFourthClass* soundManager, std::string model, Point finalPosition, Vector scaling, float rotX, float rotY, float rotZ)
{
	model_ = Object3DFactory::Instance()->CreateObject3D("StaticObject3D", graphicsManager, model);

	soundManager_ = soundManager;

	finalPosition_ = finalPosition;
	scaling_ = scaling;
	rotX_ = rotX;
	rotY_ = rotY;
	rotZ_ = rotZ;

	toyState_ = WAITING;

	return true;
}

void ToyClass::update(float elapsedTime)
{
	switch(toyState_)
	{
		case WAITING:
			{
				
			}
			break;
		case GOING_TO_PILE:
			{
				velocity_.x = finalPosition_.x - position_.x;
				velocity_.y = finalPosition_.y - position_.y;
				velocity_.z = finalPosition_.z - position_.z;

				Vector normalizedVelocity = velocity_.normalize();
				velocity_ = normalizedVelocity * 1.75f;

				position_.x += velocity_.x*elapsedTime;
				position_.y += velocity_.y*elapsedTime;
				position_.z += velocity_.z*elapsedTime;

				if(checkArrivedObjective())
				{
					position_ = finalPosition_;
					toyState_ = IN_FLOOR;
				}
			}
			break;
		case IN_FLOOR:
			{
				
			}
			break;
		default:
			{
			}
			break;
	}
}

void ToyClass::draw(GraphicsManager* graphicsManager, XMFLOAT4X4 worldMatrix, XMFLOAT4X4 viewMatrix, XMFLOAT4X4 projectionMatrix, LightClass* light, bool debug)
{
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

void ToyClass::destroy()
{
	// Release the model object
	if(model_)
	{
		model_->destroy();
		delete model_;
		model_ = 0;
	}
}

void ToyClass::makeItAppear()
{
	if(toyState_ == WAITING)
	{
		position_.x = initialPosition_.x;
		position_.y = initialPosition_.y;
		position_.z = initialPosition_.z;

		//soundManager_->playFile("falling_star", false);

		toyState_ = GOING_TO_PILE;
	}
}

void ToyClass::setInitialPosition(Point position)
{
	initialPosition_.x = position.x;
	initialPosition_.y = position.y;
	initialPosition_.z = position.z;
}

void ToyClass::setFinalPosition(Point position)
{
	finalPosition_.x = position.x;
	finalPosition_.y = position.y;
	finalPosition_.z = position.z;
}

void ToyClass::setPosition(Point position)
{
	position_.x = position.x;
	position_.y = position.y;
	position_.z = position.z;
}

Point ToyClass::getPosition()
{
	return position_;
}

bool ToyClass::checkArrivedObjective()
{
	if(position_.x < finalPosition_.x+0.05 && position_.x > finalPosition_.x-0.05 && position_.y < finalPosition_.y+0.05 && position_.y > finalPosition_.y-0.05 && position_.z < finalPosition_.z+0.05 && position_.z > finalPosition_.z-0.05)
	{
		return true;
	}

	return false;
}

bool ToyClass::isInPile()
{
	if(toyState_ == IN_FLOOR)
	{
		return true;
	}

	return false;
}
