#include "inputManager.h"

InputManager::InputManager()
{
	directInput_ = 0;
	mouse_ = 0;
}

InputManager::InputManager(const InputManager& other)
{

}

InputManager::~InputManager()
{

}

bool InputManager::setup(HINSTANCE hinstance, HWND hwnd, int screenWidth, int screenHeight)
{
	HRESULT result;

	// Store the screen size which will be used for positioning the mouse cursor.
	screenWidth_ = screenWidth;
	screenHeight_ = screenHeight;

	// Initialize the location of the mouse on the screen.
	mousePosition_.x = 0;
	mousePosition_.y = 0;

	// Initialize the movement of the mouse to 0
	mouseDelta_.x = 0;
	mouseDelta_.y = 0;
	mouseDelta_.z = 0;

	// Initialize the main direct input interface.
	result = DirectInput8Create(hinstance, DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&directInput_, NULL);
	if(FAILED(result))
	{
		MessageBox(hwnd, L"Could not initialize the direct input interface.", L"Error", MB_ICONERROR | MB_OK);
		return false;
	}

	// Initialize all the keys to being released and not pressed.
	for(int i=0; i<256; i++)
	{
		keys_[i] = false;
	}

	// Initialize the direct input interface for the mouse.
	result = directInput_->CreateDevice(GUID_SysMouse, &mouse_, NULL);
	if(FAILED(result))
	{
		MessageBox(hwnd, L"Could not create mouse interface.", L"Error", MB_ICONERROR | MB_OK);
		return false;
	}

	// Set the data format for the mouse using the pre-defined mouse data format.
	result = mouse_->SetDataFormat(&c_dfDIMouse);
	if(FAILED(result))
	{
		MessageBox(hwnd, L"Could not set the mouse format.", L"Error", MB_ICONERROR | MB_OK);
		return false;
	}
	// Set the cooperative level of the mouse to share with other programs.
	result = mouse_->SetCooperativeLevel(hwnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);
	if(FAILED(result))
	{
		MessageBox(hwnd, L"Could not set the mouse to not being shared.", L"Error", MB_ICONERROR | MB_OK);
		return false;
	}

	// Acquire the mouse.
	result = mouse_->Acquire();

	// We set that mouse buttorns are pressed to false so when they get to true we know there has been a change
	leftPressed_ = false;
	rightPressed_ = false;
	wheelPressed_ = false;

	return true;
}

bool InputManager::update()
{
	bool result;

	// Read the current state of the mouse.
	result = readMouse();
	if(!result)
	{
		return false;
	}

	// Process the changes in the mouse and keyboard.
	processInput();

	return true;
}

void InputManager::destroy()
{
	// Release the mouse.
	if(mouse_)
	{
		mouse_->Unacquire();
		mouse_->Release();
		mouse_ = 0;
	}

	// Release the main interface to direct input.
	if(directInput_)
	{
		directInput_->Release();
		directInput_ = 0;
	}

	return;
}

void InputManager::getMouseLocation(int& mouseX, int& mouseY)
{
	mouseX = mousePosition_.x;
	mouseY = mousePosition_.y;
	return;
}

void InputManager::setMouseLocation(int mouseX, int mouseY)
{
	mousePosition_.x = mouseX;
	mousePosition_.y = mouseY;
}

void InputManager::getMouseMovement(int& mouseDeltaX, int& mouseDeltaY)
{
	mouseDeltaX = mouseDelta_.x;
	mouseDeltaY = mouseDelta_.y;
	return;
}

bool InputManager::isLeftMouseButtonDown()
{
	// Check if the left mouse button is currently pressed.
	if(mouseState_.rgbButtons[0] & 0x80)
	{
		return true;
	}

	return false;
}

bool InputManager::isRightMouseButtonDown()
{
	// Check if the left mouse button is currently pressed.
	if(mouseState_.rgbButtons[1] & 0x80)
	{
		return true;
	}

	return false;
}

bool InputManager::isWheelMouseDown()
{
	// Check if the left mouse button is currently pressed.
	if(mouseState_.rgbButtons[2] & 0x80)
	{
		return true;
	}

	return false;
}

void InputManager::keyDown(unsigned int input)
{
	if(!keys_[input]){
		// We create a structure with the InputManager information where whe set the key introduced and mouse to 0 for control
		InputStruct inputStruct = {(int)input, NO_BUTTON, Point(0.0f, 0.0f, 0.0f)};

		notifyListeners(inputStruct);
	}

	// If a key is pressed then save that state in the key array.
	keys_[input] = true;
	return;
}

void InputManager::keyUp(unsigned int input)
{
	// If a key is released then clear that state in the key array.
	keys_[input] = false;
	return;
}

bool InputManager::readMouse()
{
	HRESULT result;

	// Read the mouse device.
	result = mouse_->GetDeviceState(sizeof(DIMOUSESTATE), (LPVOID)&mouseState_);
	if(FAILED(result))
	{
		// If the mouse lost focus or was not acquired then try to get control back.
		if((result == DIERR_INPUTLOST) || (result == DIERR_NOTACQUIRED))
		{
			mouse_->Acquire();
		}
		else
		{
			return false;
		}
	}

	return true;
}

void InputManager::processInput()
{
	// Update the delta of the mouse cursor based on the change of the mouse location during the frame.
	mouseDelta_.x = mouseState_.lX;
	mouseDelta_.y = mouseState_.lY;

	// Ensure the mouse location doesn't exceed the screen width or height.
	if(mousePosition_.x < 0)
	{ 
		mousePosition_.x = 0; 
	}
	if(mousePosition_.y < 0)
	{
		mousePosition_.y = 0;
	}
	
	if(mousePosition_.x > screenWidth_)
	{ 
		mousePosition_.x = screenWidth_; 
	}
	if(mousePosition_.y > screenHeight_)
	{
		mousePosition_.y = screenHeight_;
	}

	// Check whether a button has been pressed or not, first left, then right, then wheel
	if(isLeftMouseButtonDown() && !leftPressed_)
	{
		// We create a structure with the InputManager information where whe set the key introduced and mouse to 0 for control
		InputStruct inputStruct = {0, LEFT_BUTTON, mousePosition_};
		notifyListeners(inputStruct);

		leftPressed_ = true;
	}
	
	if(!isLeftMouseButtonDown())
	{
		leftPressed_ = false;
	}

	if(isRightMouseButtonDown() && !rightPressed_)
	{
		// We create a structure with the InputManager information where whe set the key introduced and mouse to 0 for control
		InputStruct inputStruct = {0, RIGHT_BUTTON, mousePosition_};
		notifyListeners(inputStruct);

		rightPressed_ = true;
	}
	
	if(!isRightMouseButtonDown())
	{
		rightPressed_ = false;
	}

	if(isWheelMouseDown() && !wheelPressed_)
	{
		// We create a structure with the InputManager information where whe set the key introduced and mouse to 0 for control
		InputStruct inputStruct = {0, WHEEL_BUTTON, mousePosition_};
		notifyListeners(inputStruct);

		wheelPressed_ = true;
	}
	
	if(!isWheelMouseDown())
	{
		wheelPressed_ = false;
	}

	// We check if there has been a change in the wheelScroll and uptade mouseDelta_ state
	if(mouseDelta_.z != mouseState_.lZ)
	{
		// If this change is different than resting, notify the change
		if(mouseState_.lZ != 0)
		{
			InputStruct inputStruct = {0, WHEEL_SCROLL, Point(0.0f, 0.0f, mouseState_.lZ)};
			notifyListeners(inputStruct);
		}
		mouseDelta_.z = mouseState_.lZ;
	}

	return;
}
