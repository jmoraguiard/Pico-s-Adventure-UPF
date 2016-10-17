#ifndef _SYSTEM_CLASS_H_
#define _SYSTEM_CLASS_H_

// Define to speed up building process, by reducing Win32 header files
#define WIN32_LEAN_AND_MEAN
#define WIN32_EXTRA_LEAN

#pragma warning( disable : 4005 )

#include <windows.h>
#include <Windowsx.h>

#include <iostream>
#include <fstream>
#include <sstream>

#include "inputManager.h"
#include "../Utils/listenerClass.h"
#include "applicationManager.h"

class SystemClass : public Listener<InputManager, InputStruct>
{
	public:
		SystemClass();
		SystemClass(const SystemClass&);
		~SystemClass();
				
		bool setup();
		bool setup(int width, int height);
		void run();
		void destroy();
		void processEvents();

		void swapBuffers();

		// Methods for handling system messages, the static re-routes the messages to the WindowProcess
		static LRESULT CALLBACK StaticWindowProcess(HWND windowHandler, UINT messageCode, WPARAM wAdditionalData, LPARAM lAdditionalData);
		LRESULT CALLBACK WindowProcess(HWND windowHandler, UINT messageCode, WPARAM wAdditionalData, LPARAM lAdditionalData);

	private:
		bool createWindow(int width, int height, bool fullscreen);
		void setupPixelFormat(void);

		virtual void notify(InputManager* notifier, InputStruct arg);

		bool isRunning_;
		bool fullscreen_;

		LPCWSTR applicationName_;
		HWND windowHandler_;
		HDC deviceContextHandler_;
		HGLRC renderingContextHandler_;
		HINSTANCE instanceHandler_;
		WNDCLASSEX windowClass_; //Contains window class information. (http://msdn.microsoft.com/en-us/library/windows/desktop/ms633577%28v=vs.85%29.aspx)
		RECT windowRectangle_; //Windows size
		DWORD windowExtendedStyle_; //Window Extended Style
		DWORD windowStyle_; //Window Style
		DEVMODE deviceModeSettings_;

		InputManager* inputManager_;
		ApplicationManager* appManager_;
};

#endif //_SYSTEM_CLASS_H_
