#include "GUIManager.h"

GUIManager::GUIManager()
{
}

GUIManager::GUIManager(const GUIManager& camera)
{
}

GUIManager::~GUIManager()
{
}

bool GUIManager::setup(GraphicsManager* graphicsManager)
{
	graphicsManager->getScreenSize(screenWidth_, screenHeight_);

	return true;
}

void GUIManager::update(float elapsedTime)
{
	std::map<std::string, GUIFrame*>::iterator it;
	for(it = guiFrames_.begin(); it != guiFrames_.end(); ++it)
	{
		it->second->update(elapsedTime);
	}
}

void GUIManager::draw(ID3D11DeviceContext* deviceContext, XMFLOAT4X4 worldMatrix, XMFLOAT4X4 viewMatrix, XMFLOAT4X4 orthoMatrix)
{
	std::map<std::string, GUIFrame*>::iterator it;
	for(it = guiFrames_.begin(); it != guiFrames_.end(); ++it)
	{
		it->second->draw(deviceContext, worldMatrix, viewMatrix, orthoMatrix);
	}
}

void GUIManager::addFrame(GUIFrame* frame)
{
	guiFrames_.insert(std::pair<std::string, GUIFrame*>(frame->getName(), frame));
}

void GUIManager::notify(InputManager* notifier, InputStruct arg)
{
	switch(arg.keyPressed)
	{
		default:
			{

			}
			break;
	}

	switch(arg.mouseButton)
	{
		case LEFT_BUTTON:
			{
				std::map<std::string, GUIFrame*>::iterator it;
				for(it = guiFrames_.begin(); it != guiFrames_.end(); ++it)
				{
					if(it->second->offer(arg.mouseInfo, true))
					{
						break;
					}
				}
			}
			break;
		default:
			{

			}
			break;
	}
}

void GUIManager::notify(KinectClass* notifier, KinectStruct arg)
{
	Point kinectHandPos = Point(arg.handPos.x, arg.handPos.y);

	std::map<std::string, GUIFrame*>::iterator it;
	for(it = guiFrames_.begin(); it != guiFrames_.end(); ++it)
	{
		if(it->second->offer(kinectHandPos))
		{
			break;
		}
	}
}