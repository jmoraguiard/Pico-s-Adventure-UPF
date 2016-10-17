#include "GUIFrame.h"

GUIFrame::GUIFrame()
{
	frameName_ = "";
	position_ = Point(0.0f, 0.0f);
	width_ = 0;
	height_ = 0;
}

GUIFrame::GUIFrame(const GUIFrame& camera)
{
}

GUIFrame::~GUIFrame()
{
}

bool GUIFrame::setup(GraphicsManager* graphicsManager, std::string name, Point position, int width, int height)
{
	frameName_ = name;
	position_ = position;
	width_ = width;
	height_ = height;

	int screenWidth, screenHeight;
	graphicsManager->getScreenSize(screenWidth, screenHeight);

	// Create the bitmap object.
	background_ = new ImageClass();
	if(!background_)
	{
		return false;
	}

	// Initialize the bitmap object.
	if(!background_->setup(graphicsManager->getDevice(), graphicsManager->getShader2D(), screenWidth, screenHeight, "seafloor", width_, height_))
	{
		std::string textToDisplay = "Could not initialize the frame background object for " + frameName_;
		MessageBoxA(NULL, textToDisplay.c_str(), "GUIFrame - Error", MB_ICONERROR | MB_OK);
		return false;
	}

	text_ = new TextClass();
	if(!text_)
	{
		return false;
	}

	// Initialize the text object.
	if(!text_->setup(graphicsManager->getDevice(), graphicsManager->getDeviceContext(), graphicsManager->getShader2D(), screenWidth, screenHeight, position_.x+5, position_.y+5, frameName_))
	{
		std::string textToDisplay = "Could not initialize the frame text object for " + frameName_;
		MessageBoxA(NULL, textToDisplay.c_str(), "GUIFrame - Error", MB_OK);
		return false;
	}
	
	return true;
}

void GUIFrame::update(float elapsedTime)
{
	std::map<std::string, GUIButton*>::iterator it;
	for(it = guiButtons_.begin(); it != guiButtons_.end(); ++it)
	{
		it->second->update(elapsedTime);
	}
}

void GUIFrame::draw(ID3D11DeviceContext* deviceContext, XMFLOAT4X4 worldMatrix, XMFLOAT4X4 viewMatrix, XMFLOAT4X4 orthoMatrix)
{
	std::map<std::string, GUIButton*>::iterator it;
	for(it = guiButtons_.begin(); it != guiButtons_.end(); ++it)
	{
		it->second->draw(deviceContext, worldMatrix, viewMatrix, orthoMatrix);
	}

	//background_->draw(deviceContext, position_.x, position_.y-2, worldMatrix, viewMatrix, orthoMatrix);

	text_->draw(deviceContext, worldMatrix, viewMatrix, orthoMatrix);
}

std::string GUIFrame::getName()
{
	return frameName_;
}

Point GUIFrame::getPosition()
{
	return position_;
}

void GUIFrame::setPosition(Point pos)
{
	position_ = pos;
}
		
int GUIFrame::getWidth()
{
	return width_;
}

void GUIFrame::setWidth(int width)
{
	width = width_;
}

int GUIFrame::getHeight()
{
	return height_;
}

void GUIFrame::setHeight(int height)
{
	height_ = height;
}

int GUIFrame::getNumberButtons()
{
	return guiButtons_.size();
}

bool GUIFrame::getButtonIsActive(std::string buttonName)
{
	return guiButtons_.at(buttonName)->getActive();
}

void GUIFrame::selectButton(std::string buttonName)
{
	guiButtons_.at(buttonName)->select();
}

void GUIFrame::deleteButton(std::string buttonName)
{
	guiButtons_.erase(buttonName);
	reorganizeButtons();
}

void GUIFrame::deleteButtons()
{
	/*std::map<std::string, GUIButton*>::iterator it;
	for(it = guiButtons_.begin(); it != guiButtons_.end(); ++it)
	{
		it->second->destroy();
		it = guiButtons_.erase(it);
		continue;
	}*/
	guiButtons_.clear();
}

void GUIFrame::reorganizeButtons()
{
	int posY = 25;
	std::map<std::string, GUIButton*>::iterator it;
	for(it = guiButtons_.begin(); it != guiButtons_.end(); ++it)
	{
		// Calculate new position for each of the buttons
		Point newPos;
		newPos.x = it->second->getPosition().x;
		newPos.y = posY;

		// Set new position
		it->second->setPosition(newPos);

		// Add the height of the newly placed button
		posY += it->second->getHeight();
	}
}

bool GUIFrame::offer(Point mouseClick)
{
	// First we check if mouse position is inside the frame
	if(checkInside(mouseClick))
	{
		// then we iterate over all Buttons and offer them the mouse
		std::map<std::string, GUIButton*>::iterator it;
		for(it = guiButtons_.begin(); it != guiButtons_.end(); ++it)
		{
			if(it->second->offer(mouseClick))
			{
				break;
			}
		}
	}

	return false;
}

bool GUIFrame::offer(Point mouseClick, bool direct)
{
	// First we check if mouse position is inside the frame
	if(checkInside(mouseClick))
	{
		// then we iterate over all Buttons and offer them the mouse
		std::map<std::string, GUIButton*>::iterator it;
		for(it = guiButtons_.begin(); it != guiButtons_.end(); ++it)
		{
			if(it->second->offer(mouseClick, direct))
			{
				break;
			}
		}
	}

	return false;
}

GUIButton* GUIFrame::addButton(GraphicsManager* graphicsManager, std::string name, int height, ButtonPurpose buttonPurpose)
{
	GUIButton* button = new GUIButton();

	// Calculate the position of the button given the position of frame and how many buttons there were before
	int posY = 25;
	std::map<std::string, GUIButton*>::iterator it;
	for(it = guiButtons_.begin(); it != guiButtons_.end(); ++it)
	{
		posY += it->second->getHeight();
	}

	// Having the final vertical position, we setup the button
	button->setup(graphicsManager, name, Point(position_.x, position_.y+posY), width_, height, buttonPurpose);

	// We add it to the map of buttons to keep track of it
	guiButtons_.insert(std::pair<std::string, GUIButton*>(button->getName(), button));

	// Update the total height of the frame
	if(posY+button->getHeight() > height_)
	{
		height_ = posY+button->getHeight();
	}

	return button;
}

GUIButton* GUIFrame::addButton(GraphicsManager* graphicsManager, std::string name, Point pos, Point size)
{
	GUIButton* button = new GUIButton();

	// Having the final vertical position, we setup the button
	button->setup(graphicsManager, name, pos, size.x, size.y, SELECT_OBJECT);

	// We add it to the map of buttons to keep track of it
	guiButtons_.insert(std::pair<std::string, GUIButton*>(button->getName(), button));

	// Update the total height of the frame
	if(pos.y+button->getHeight() > height_)
	{
		height_ = pos.y+button->getHeight();
	}

	return button;
}

GUIButton* GUIFrame::addButton(GraphicsManager* graphicsManager, std::string name, Point pos, Point size, std::string image)
{
	GUIButton* button = new GUIButton();

	// Having the final vertical position, we setup the button
	button->setup(graphicsManager, name, pos, size.x, size.y, image, LOAD_OBJECT);

	// We add it to the map of buttons to keep track of it
	guiButtons_.insert(std::pair<std::string, GUIButton*>(button->getName(), button));

	// Update the total height of the frame
	if(pos.y+button->getHeight() > height_)
	{
		height_ = pos.y+button->getHeight();
	}

	return button;
}

bool GUIFrame::checkInside(Point pos)
{
	if(pos.x >= position_.x && pos.x <= position_.x+width_ &&
	   pos.y >= position_.y && pos.y <= position_.y+height_)
	{
		return true;
	}

	return false;
}