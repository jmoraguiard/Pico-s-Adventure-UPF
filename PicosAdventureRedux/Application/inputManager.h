#ifndef _INPUT_MANAGER_H_
#define _INPUT_MANAGER_H_

#define DIRECTINPUT_VERSION 0x0800

#pragma comment(lib, "dinput8.lib")
#pragma comment(lib, "dxguid.lib")

#include <dinput.h>

#include <windows.h>

#include <string>
#include <sstream>

#include "../Utils/notifierClass.h"
#include "../math/PointClass.h"

enum MouseButton
	{
		NO_BUTTON,
		LEFT_BUTTON,
		RIGHT_BUTTON,
		WHEEL_BUTTON,
		WHEEL_SCROLL
	};

struct InputStruct
	{
		int keyPressed;
		MouseButton mouseButton;
		Point mouseInfo;
	};

class InputManager : public Notifier<InputManager, InputStruct>
{
	public:
		InputManager();
		InputManager(const InputManager&);
		~InputManager();

		bool setup(HINSTANCE hinstance, HWND hwnd, int screenWidth, int screenHeight);
		bool update();
		void destroy();

		void getMouseLocation(int& mouseX, int& mouseY);
		void setMouseLocation(int mouseX, int mouseY);
		void getMouseMovement(int& mouseDeltaX, int& mouseDeltaY);
		
		bool isLeftMouseButtonDown();
		bool isRightMouseButtonDown();
		bool isWheelMouseDown();

		void keyDown(unsigned int input);
		void keyUp(unsigned int input);

	private:
		bool readMouse();
		void processInput();

		IDirectInput8* directInput_;
		IDirectInputDevice8* mouse_;

		bool keys_[256];
		
		DIMOUSESTATE mouseState_;
		Point mousePosition_;
		Point mouseDelta_;

		int screenWidth_, screenHeight_;
		bool leftPressed_;
		bool rightPressed_;
		bool wheelPressed_;
};

#endif