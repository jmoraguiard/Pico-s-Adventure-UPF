#include "spriteAnimationClass.h"

SpriteAnimationClass::SpriteAnimationClass()
{
	for(int i = 0; i < 4; i++)
	{
		sprites_[i] = 0;
	}

	endRayPosition_.x = 0;
	endRayPosition_.y = 0;
}

SpriteAnimationClass::SpriteAnimationClass(const SpriteAnimationClass&)
{

}

SpriteAnimationClass::~SpriteAnimationClass()
{

}

bool SpriteAnimationClass::setup(GraphicsManager* graphicsManager, std::string spriteName, int number)
{
	graphicsManager->getScreenSize(screenWidth_, screenHeight_);

	for(int i = 0; i < number; i++)
	{
		std::stringstream fileName;
		fileName << spriteName << i;

		sprites_[i] = new ImageClass;
		if(!sprites_[i])
		{
			MessageBoxA(NULL, "Could not initialize the image instance.", "SecondScreen - Error", MB_OK);
			return false;
		}

		if(!sprites_[i]->setup(graphicsManager->getDevice(), graphicsManager->getShader2D(), screenWidth_, screenHeight_, fileName.str(), 20, 1024))
		{
			MessageBoxA(NULL, "Could not setup the sprite image.", "SecondScreen - Error", MB_OK);
			return false;
		}
	}

	endRayImage_ = new ImageClass;
	if(!endRayImage_)
	{
		MessageBoxA(NULL, "Could not initialize the kinectHand_ image instance.", "FreindSpaceShip - Error", MB_OK);
		return false;
	}

	if(!endRayImage_->setup(graphicsManager->getDevice(), graphicsManager->getShader2D(), screenWidth_, screenHeight_, "star", 40, 40))
	{
		MessageBoxA(NULL, "Could not setup the kinectHand_ image.", "FreindSpaceShip - Error", MB_OK);
		return false;
	}

	spriteClock_ = new ClockClass();
	if(!spriteClock_)
	{
		return false;
	}
	spriteClock_->reset();

	updateSpriteTime_ = 0.05f;
	spriteIndex_ = 0;
	maxSprites_ = number;
}

void SpriteAnimationClass::update()
{
	spriteClock_->tick();

	if(spriteClock_->getTime() > updateSpriteTime_)
	{
		spriteIndex_++;

		if(spriteIndex_ >= maxSprites_)
		{
			spriteIndex_ = 0;
		}

		spriteClock_->reset();
	}
}

void SpriteAnimationClass::draw(GraphicsManager* graphicsManager, XMFLOAT4X4 worldMatrix, XMFLOAT4X4 viewMatrix, XMFLOAT4X4 orthoMatrix, Point pos, float rot, bool debug)
{
	endRayPosition_.x = 1024*cos(rot);
	endRayPosition_.y = 1024*sin(rot);

	if(debug)
	{
		XMFLOAT4X4 orthoMatrix, worldCopy;
		graphicsManager->getOrthoMatrix(orthoMatrix);

		int screenWidth, screenHeight;
		graphicsManager->getScreenSize(screenWidth, screenHeight);

		// We translate the hand position so we display in proper place the debug end
		worldCopy = worldMatrix;

		XMFLOAT4X4 movingMatrix;
		XMStoreFloat4x4(&movingMatrix, XMMatrixTranslation(pos.x-(float)(screenWidth_/2), (float)(screenHeight_/2)-pos.y, 0.0f));
		XMStoreFloat4x4(&worldCopy, XMMatrixMultiply(XMLoadFloat4x4(&worldCopy), XMLoadFloat4x4(&movingMatrix)));

		endRayImage_->draw(graphicsManager->getDeviceContext(), endRayPosition_.x, endRayPosition_.y, worldCopy, viewMatrix, orthoMatrix, XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));
	}

	// We get the angle in polard coordinates. We add half PI (quarter of a spin) for displaying proper turning 
	//(axis Z having axis Y as 0º and not X, as angle has been calculated)
	XMFLOAT4X4 rotatingMatrixZ;
	XMStoreFloat4x4(&rotatingMatrixZ, XMMatrixRotationZ(rot+XM_PIDIV2));
	XMStoreFloat4x4(&worldMatrix, XMMatrixMultiply(XMLoadFloat4x4(&worldMatrix), XMLoadFloat4x4(&rotatingMatrixZ)));
	XMFLOAT4X4 movingMatrix2;
	XMStoreFloat4x4(&movingMatrix2, XMMatrixTranslation(pos.x-(float)(screenWidth_/2), (float)(screenHeight_/2)-pos.y, 0.0f));
	XMStoreFloat4x4(&worldMatrix, XMMatrixMultiply(XMLoadFloat4x4(&worldMatrix), XMLoadFloat4x4(&movingMatrix2)));

	sprites_[spriteIndex_]->draw(graphicsManager->getDeviceContext(), 0, 0, worldMatrix, viewMatrix, orthoMatrix, XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));
}

void SpriteAnimationClass::destroy()
{
	for(int i = 0; i < 4; i++)
	{
		if(sprites_[i])
		{
			sprites_[i]->destroy();
			delete sprites_[i];
			sprites_[i] = 0;
		}
	}
}

float SpriteAnimationClass::getPointDistance(Point hand, Point point)
{
	Point start;
	start.x = hand.x;
	start.y = hand.y;
	Point end;
	end.x = endRayPosition_.x+hand.x;
	end.y = hand.y-endRayPosition_.y;

	float l2 = pow(end.x-start.x, 2)+pow(end.y-start.y, 2);
	//If line is length 0
	if(l2 == 0.0f)
	{
		return sqrt(pow(start.x-point.x, 2)+pow(start.y-point.y, 2));
	}
	
	Point ps, es;
	ps.x = point.x-start.x;
	ps.y = point.y-start.y;
	es.x = end.x-start.x;
	es.y = end.y-start.y;
	float t = (ps.x*es.x + ps.y*es.y) / l2;

	// Beyond the 'start' end of the segment
	if(t < 0.0f)
	{
		return sqrt(pow(start.x-point.x, 2)+pow(start.y-point.y, 2));
	}
	// Beyond the 'end' end of the segment
	else if(t > 1.0f)
	{
		return sqrt(pow(end.x-point.x, 2)+pow(end.y-point.y, 2));
	}

	Vector projection;
	projection.x = start.x + t*es.x;
	projection.y = start.y + t*es.y;

	return sqrt(pow(projection.x-point.x, 2)+pow(projection.y-point.y, 2));
}

Point SpriteAnimationClass::getEndRayPosition()
{
	return endRayPosition_;
}