#ifndef _APPLICATION_MANAGER_H_
#define _APPLICATION_MANAGER_H_

#include <windows.h>

#include "../Graphics/graphicsManager.h"

#include "inputManager.h"
#include "kinectClass.h"

#include "../Utils/clockClass.h"

class ApplicationState;

class ApplicationManager
{
	public:
		ApplicationManager();
		virtual ~ApplicationManager();

		bool setup(HWND windowsHandler, InputManager* inputManager, int width, int height, bool fullscreen);
		void update();
		void draw();
		void destroy();

		bool changeState(ApplicationState* appState);

	private:
		ApplicationState* appState_;

		InputManager* inputManager_;
		GraphicsManager* graphicsManager_;
		ClockClass* clockClass_;
		KinectClass* kinectManager_;

		HWND windowHandler_;
};

#endif
