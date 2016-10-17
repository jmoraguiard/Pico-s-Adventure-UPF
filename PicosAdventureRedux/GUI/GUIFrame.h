#ifndef _GUI_FRAME_H_
#define _GUI_FRAME_H_

#include "../Application/inputManager.h"

#include "../Math/pointClass.h"

#include "../Graphics/graphicsManager.h"
#include "../Graphics/imageClass.h"

#include "../Utils/textClass.h"

#include "GUIButton.h"

#include <map>

class GUIFrame
{
	public:
		GUIFrame();
		GUIFrame(const GUIFrame& camera);
		~GUIFrame();

		bool setup(GraphicsManager* graphicsManager, std::string name, Point position, int width, int height);
		void update(float elapsedTime);
		void draw(ID3D11DeviceContext* deviceContext, XMFLOAT4X4 worldMatrix, XMFLOAT4X4 viewMatrix, XMFLOAT4X4 orthoMatrix);

		std::string getName();

		Point getPosition();
		void setPosition(Point pos);
		
		int getWidth();
		void setWidth(int width);
		int getHeight();
		void setHeight(int height);

		int getNumberButtons();

		bool getButtonIsActive(std::string buttonName);
		void selectButton(std::string buttonName);

		void deleteButton(std::string buttonName);
		void deleteButtons();

		void reorganizeButtons();

		bool offer(Point mouseClick);
		bool offer(Point mouseClick, bool direct);

		GUIButton* addButton(GraphicsManager* graphicsManager, std::string name, int height, ButtonPurpose buttonPurpose);
		GUIButton* addButton(GraphicsManager* graphicsManager, std::string name, Point pos, Point size);
		GUIButton* addButton(GraphicsManager* graphicsManager, std::string name, Point pos, Point size, std::string image);

	private:
		bool checkInside(Point pos);

		std::map<std::string, GUIButton*> guiButtons_;

		TextClass*  text_;
		ImageClass* background_;

		std::string frameName_;

		Point position_;
		int	  width_;
		int   height_;
		
};

#endif //_GUI_FRAME_H_