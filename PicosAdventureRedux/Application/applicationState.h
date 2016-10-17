#ifndef _APPLICATION_STATE_H_
#define _APPLICATION_STATE_H_

#include "applicationManager.h"

#include "inputManager.h"

#include "../Utils/listenerClass.h"

#include "../Graphics/graphicsManager.h"
#include "../Graphics/cameraClass.h"

#include "../Math/pointClass.h"

#include <string>

class ApplicationState : public Listener<InputManager, InputStruct>
{
	public:
		ApplicationState();
		virtual ~ApplicationState();

		virtual bool setup(ApplicationManager* appManager, GraphicsManager* graphicsManager, InputManager * inputManager, KinectClass* kinectManager) = 0;
		virtual void update(float elapsedTime) = 0;
		virtual void draw() = 0;
		virtual void destroy() = 0;

		virtual void notify(InputManager* notifier, InputStruct arg) = 0;

		bool changeState(ApplicationState* appState);

	protected:
		ApplicationManager* appManager_;
		GraphicsManager* graphicsManager_;
};

#endif
