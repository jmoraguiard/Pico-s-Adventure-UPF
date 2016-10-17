#ifndef _TOY_CLASS_H_
#define _TOY_CLASS_H_

#include "../Engine/Object3DFactory.h"
#include "../Engine/particleSystem.h"

#include "../Game/soundFourthClass.h"

#include "../Graphics/graphicsManager.h"

#include "../Math/pointClass.h"
#include "../Math/vectorClass.h"

#include <math.h>

class ToyClass
{
	private:
		enum ToyState
			{
				WAITING,
				GOING_TO_PILE,
				IN_FLOOR
			};

	public:
		ToyClass();
		ToyClass(const ToyClass&);
		~ToyClass();

		bool setup(GraphicsManager* graphicsManager, SoundFourthClass* soundManager, std::string model, Point finalPosition, Vector scaling, float rotX, float rotY, float rotZ);
		void update(float elapsedTime);
		void draw(GraphicsManager* graphicsManager, XMFLOAT4X4 worldMatrix, XMFLOAT4X4 viewMatrix, XMFLOAT4X4 projectionMatrix, LightClass* light, bool debug);
		void destroy();

		std::string getName();

		void setInitialPosition(Point position);
		void setFinalPosition(Point position);
		void setPosition(Point position);
		Point getPosition();

		void setScale(Vector newScale);
		Vector getScale();

		void makeItAppear();
		bool checkArrivedObjective();
		bool isInPile();

	private:
		SoundFourthClass* soundManager_;

		Object3D*	model_;

		Point		initialPosition_;
		Point		finalPosition_;
		Point		position_;
		Vector		scaling_; 
		Vector		velocity_;

		float		rotX_;
		float		rotY_; 
		float		rotZ_;

		ToyState	toyState_;

};

#endif //_STAR_CLASS_H_
