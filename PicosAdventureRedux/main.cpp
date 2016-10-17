#include "Application/systemClass.h"

int WINAPI wWinMain(HINSTANCE hInstance, //The handle of the application's current instance
					HINSTANCE prevInstance,	//The handle to previous instance of the application. According to MSDN is always NULL.
					LPWSTR cmdLine,	//The command line for the application without program's name. To retrieve the entire command line, use
									//the GetCommandLine function.
					int cmdShow) //And ID that specifies how the window should be sown
{
	SystemClass* system; // Pointer ti the system object.
	
	// Create the system object. If it is not created, exit.
	system = new SystemClass();
	if(!system)
	{
		MessageBox(NULL, L"Could not create the SystemClass", L"Main - Error", MB_ICONERROR | MB_OK);
		return 0;
	}

	// Initialize and run the system object.
	if(system->setup())
	{
		system->run();
	}

	// When done, destroy the system object and release.
	system->destroy();
	delete system;
	system = 0;

	return 0;
}
