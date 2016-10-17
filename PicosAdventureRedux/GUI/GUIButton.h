#ifndef _GUI_BUTTON_H_
#define _GUI_BUTTON_H_

#include "../Application/inputManager.h"

#include "../Math/pointClass.h"

#include "../Graphics/graphicsManager.h"
#include "../Graphics/imageClass.h"

#include "../Utils/textClass.h"
#include "../Utils/notifierClass.h"

enum ButtonPurpose
	{
		LOAD_OBJECT,
		SELECT_OBJECT
	};

struct ButtonStruct
	{
		ButtonPurpose	buttonPurpose;
		std::string		buttonInfo;
	};

class GUIButton : public Notifier<GUIButton, ButtonStruct>
{
	public:
		GUIButton();
		GUIButton(const GUIButton& camera);
		~GUIButton();

		bool setup(GraphicsManager* graphicsManager, std::string name, Point position, int width, int height, ButtonPurpose purpose);
		bool setup(GraphicsManager* graphicsManager, std::string name, Point position, int width, int height, std::string image, ButtonPurpose purpose);
		void update(float elapsedTime);
		void draw(ID3D11DeviceContext* deviceContext, XMFLOAT4X4 worldMatrix, XMFLOAT4X4 viewMatrix, XMFLOAT4X4 orthoMatrix);
		void destroy();

		std::string getName();

		Point getPosition();
		void setPosition(Point pos);
		
		int getWidth();
		void setWidth(int width);
		int getHeight();
		void setHeight(int height);

		bool getActive();
		void setActive(bool active);

		void select();

		bool offer(Point mouseClick);
		bool offer(Point mouseClick, bool direct);

	private:
		bool checkInside(Point pos);

		TextClass*  text_;
		ImageClass* background_;

		std::string buttonName_;

		Point position_;
		int	  width_;
		int   height_;

		Point viewportPosition_;

		float selectTime_;
		float selectionTime_;
		bool  selected_;
		bool  drawSelected_;

		int screenWidth_;
		int screenHeight_;

		ButtonPurpose buttonPurpose_;

		bool  buttonActive_;
		
};

#endif //_GUI_BUTTON_H_