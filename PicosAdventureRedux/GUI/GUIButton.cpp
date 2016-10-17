#include "GUIButton.h"

GUIButton::GUIButton()
{
	buttonName_ = "";
	position_ = Point(0.0f, 0.0f);
	width_ = 0;
	height_ = 0;

	selectTime_ = 0.0f;
	selectionTime_ = 0.0f;
}

GUIButton::GUIButton(const GUIButton& camera)
{
}

GUIButton::~GUIButton()
{
}

bool GUIButton::setup(GraphicsManager* graphicsManager, std::string name, Point position, int width, int height, ButtonPurpose purpose)
{
	buttonName_ = name;
	position_ = position;
	width_ = width;
	height_ = height;
	buttonPurpose_ = purpose;

	graphicsManager->getScreenSize(screenWidth_, screenHeight_);

	// Create the bitmap object.
	background_ = new ImageClass();
	if(!background_)
	{
		return false;
	}

	// Initialize the bitmap object.
	if(!background_->setup(graphicsManager->getDevice(), graphicsManager->getShader2D(), screenWidth_, screenHeight_, "button", width_, height_))
	{
		std::string textToDisplay = "Could not initialize the frame background object for " + buttonName_;
		MessageBoxA(NULL, textToDisplay.c_str(), "GUIFrame - Error", MB_ICONERROR | MB_OK);
		return false;
	}

	text_ = new TextClass();
	if(!text_)
	{
		return false;
	}

	// Initialize the text object.
	if(!text_->setup(graphicsManager->getDevice(), graphicsManager->getDeviceContext(), graphicsManager->getShader2D(), screenWidth_, screenHeight_, (screenWidth_/2)+position_.x, (screenHeight_/2)-position_.y, buttonName_))
	{
		std::string textToDisplay = "Could not initialize the frame text object for " + buttonName_;
		MessageBoxA(NULL, textToDisplay.c_str(), "GUIFrame - Error", MB_OK);
		return false;
	}

	viewportPosition_.x = (screenWidth_/2)+position_.x;
	viewportPosition_.y = (screenHeight_/2)-position_.y;

	selectTime_ = 0.0f;
	selectionTime_ = 2.0f;
	selected_ = false;
	drawSelected_ = false;

	if(purpose == SELECT_OBJECT)
	{
		buttonActive_ = true;
	}
	else
	{
		buttonActive_ = false;
	}
	
	return true;
}

bool GUIButton::setup(GraphicsManager* graphicsManager, std::string name, Point position, int width, int height, std::string image, ButtonPurpose purpose)
{
	buttonName_ = name;
	position_ = position;
	width_ = width;
	height_ = height;
	buttonPurpose_ = purpose;

	graphicsManager->getScreenSize(screenWidth_, screenHeight_);

	// Create the bitmap object.
	background_ = new ImageClass();
	if(!background_)
	{
		return false;
	}

	// Initialize the bitmap object.
	if(!background_->setup(graphicsManager->getDevice(), graphicsManager->getShader2D(), screenWidth_, screenHeight_, image, width_, height_))
	{
		std::string textToDisplay = "Could not initialize the frame background object for " + buttonName_;
		MessageBoxA(NULL, textToDisplay.c_str(), "GUIFrame - Error", MB_ICONERROR | MB_OK);
		return false;
	}

	text_ = new TextClass();
	if(!text_)
	{
		return false;
	}

	// Initialize the text object.
	if(!text_->setup(graphicsManager->getDevice(), graphicsManager->getDeviceContext(), graphicsManager->getShader2D(), screenWidth_, screenHeight_, (screenWidth_/2)+position_.x+5, (screenHeight_/2)-position_.y+5, buttonName_))
	{
		std::string textToDisplay = "Could not initialize the frame text object for " + buttonName_;
		MessageBoxA(NULL, textToDisplay.c_str(), "GUIFrame - Error", MB_OK);
		return false;
	}

	viewportPosition_.x = (screenWidth_/2)+position_.x;
	viewportPosition_.y = (screenHeight_/2)-position_.y;

	selectTime_ = 0.0f;
	selectionTime_ = 2.0f;
	selected_ = false;

	if(purpose == SELECT_OBJECT)
	{
		buttonActive_ = true;
	}
	else
	{
		buttonActive_ = false;
	}
	
	return true;
}

void GUIButton::update(float elapsedTime)
{
	if(selected_)
	{
		selectTime_ += elapsedTime;
		selected_ = false;
		drawSelected_ = true;
	}

	selectTime_ -= 0.001;
	if(selectTime_ < 0.0)
	{
		selectTime_ = 0.0f;
	}
}

void GUIButton::draw(ID3D11DeviceContext* deviceContext, XMFLOAT4X4 worldMatrix, XMFLOAT4X4 viewMatrix, XMFLOAT4X4 orthoMatrix)
{
	if(buttonActive_)
	{
		background_->draw(deviceContext, position_.x, position_.y-2, worldMatrix, viewMatrix, orthoMatrix, XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f));
	}
	else
	{
		if(drawSelected_)
		{
			background_->draw(deviceContext, position_.x, position_.y-2, worldMatrix, viewMatrix, orthoMatrix, XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));
		}
		else
		{
			background_->draw(deviceContext, position_.x, position_.y-2, worldMatrix, viewMatrix, orthoMatrix, XMFLOAT4(0.6f, 0.6f, 0.6f, 1.0f));
		}
	}

	//text_->draw(deviceContext, worldMatrix, viewMatrix, orthoMatrix);
}

void GUIButton::destroy()
{
	if(text_)
	{
		text_->destroy();
		delete text_;
		text_ = 0;
	}
	
	if(background_)
	{
		background_->destroy();
		delete background_;
		background_ = 0;
	}
}

std::string GUIButton::getName()
{
	return buttonName_;
}

Point GUIButton::getPosition()
{
	return position_;
}

void GUIButton::setPosition(Point pos)
{
	position_ = pos;
	text_->setPosition(pos.x+10, pos.y+5);
}
		
int GUIButton::getWidth()
{
	return width_;
}

void GUIButton::setWidth(int width)
{
	width = width_;
}

int GUIButton::getHeight()
{
	return height_;
}

void GUIButton::setHeight(int height)
{
	height_ = height;
}

bool GUIButton::getActive()
{
	return buttonActive_;
}

void GUIButton::setActive(bool active)
{
	buttonActive_ = active;
}

void GUIButton::select()
{
	ButtonStruct buttonStruct = {buttonPurpose_, buttonName_};

	notifyListeners(buttonStruct);

	selectTime_ = 0.0f;
}

bool GUIButton::offer(Point mouseClick)
{
	if(checkInside(mouseClick))
	{
		if(buttonPurpose_ == SELECT_OBJECT)
		{
			buttonActive_ = !buttonActive_;
		}

		selected_ = true;

		if(selectTime_ > selectionTime_)
		{
			ButtonStruct buttonStruct = {buttonPurpose_, buttonName_};

			notifyListeners(buttonStruct);

			selectTime_ = 0.0f;
		}
	}
	else
	{
		drawSelected_ = false;
	}

	return false;
}

bool GUIButton::offer(Point mouseClick, bool direct)
{
	ButtonStruct buttonStruct = {buttonPurpose_, buttonName_};

	notifyListeners(buttonStruct);

	selectTime_ = 0.0f;

	return false;
}

bool GUIButton::checkInside(Point pos)
{
	if(pos.x >= viewportPosition_.x && pos.x <= viewportPosition_.x+width_ &&
	   pos.y >= viewportPosition_.y && pos.y <= viewportPosition_.y+height_)
	{
		return true;
	}

	return false;
}