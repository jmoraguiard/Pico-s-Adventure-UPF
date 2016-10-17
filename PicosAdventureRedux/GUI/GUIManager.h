#ifndef _GUI_MANAGER_H_
#define _GUI_MANAGER_H_

#include <map>

#include "../Application/inputManager.h"
#include "../Application/kinectClass.h"

#include "../Utils/listenerClass.h"

#include "GUIFrame.h"

class GUIManager : public Listener<InputManager, InputStruct>, public Listener<KinectClass, KinectStruct>
{
	public:
		GUIManager();
		GUIManager(const GUIManager& camera);
		~GUIManager();

		bool setup(GraphicsManager* graphicsmanager);
		void update(float elapsedTime);
		void draw(ID3D11DeviceContext* deviceContext, XMFLOAT4X4 worldMatrix, XMFLOAT4X4 viewMatrix, XMFLOAT4X4 orthoMatrix);

		void addFrame(GUIFrame* frame);

		void notify(InputManager* notifier, InputStruct arg);
		void notify(KinectClass* notifier, KinectStruct arg);

	private:
		std::map<std::string, GUIFrame*> guiFrames_;

		int screenWidth_;
		int screenHeight_;
};

#endif //_GUI_MANAGER_H_