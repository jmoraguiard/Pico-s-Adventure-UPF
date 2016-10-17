#ifndef _SPRITE_ANIMATION_CLASS_H_
#define _SPRITE_ANIMATION_CLASS_H_

#include "../Graphics/graphicsManager.h"
#include "../Graphics/imageClass.h"

#include "../Math/pointClass.h"
#include "../Math/vectorClass.h"

#include "../Utils/clockClass.h"

class SpriteAnimationClass
{
	public:
		SpriteAnimationClass();
		SpriteAnimationClass(const SpriteAnimationClass&);
		~SpriteAnimationClass();

		bool setup(GraphicsManager* graphicsManager, std::string spriteName, int number);
		void update();
		void draw(GraphicsManager* graphicsManager, XMFLOAT4X4 worldMatrix, XMFLOAT4X4 viewMatrix, XMFLOAT4X4 orthoMatrix, Point pos, float rot, bool debug);
		void destroy();

		float getPointDistance(Point hand, Point point);
		Point getEndRayPosition();

	private:
		ImageClass*		sprites_[4];
		int				maxSprites_;
		int				spriteIndex_;

		ImageClass*		endRayImage_;
		Point			endRayPosition_;

		int				screenWidth_;
		int				screenHeight_;

		ClockClass*		spriteClock_;
		float			updateSpriteTime_;
};

#endif //#ifndef _SPRITE_ANIMATION_CLASS_H_