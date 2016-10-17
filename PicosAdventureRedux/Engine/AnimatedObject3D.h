#ifndef _ANIMATED_OBJECT_3D_H_
#define _ANIMATED_OBJECT_3D_H_

#include "Object3D.h"

class AnimatedObject3D : public Object3D
{
	public:
		AnimatedObject3D();
		AnimatedObject3D(const AnimatedObject3D &);
		~AnimatedObject3D();

		virtual bool setup(GraphicsManager* graphicsManager, std::string modelName, std::string objectName);
		virtual void update(float dt);
		virtual void draw(ID3D11Device* device, ID3D11DeviceContext* deviceContext, XMFLOAT4X4 worldMatrix, XMFLOAT4X4 viewMatrix, XMFLOAT4X4 projectionMatrix, LightClass* light);
		virtual void destroy();

		void playAnimation();
		void stopAnimation();

		static Object3D * __stdcall Create(GraphicsManager* graphicsManager, std::string modelName, std::string objectName);
	private:
		
};

#endif //_ANIMATED_OBJECT_3D_H_