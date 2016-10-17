#ifndef _SPHERE_COLLISION_H_
#define _SPHERE_COLLISION_H_

#include "../Graphics/graphicsManager.h"

#include "../Graphics/cameraClass.h"

#include "Object3DFactory.h"

class SphereCollision
{
	public:
		SphereCollision();
		SphereCollision(const SphereCollision &);
		~SphereCollision();

		void setup(GraphicsManager *graphicsManager, Point relativePos, float radius);
		void draw(ID3D11Device* device, ID3D11DeviceContext* deviceContext, XMFLOAT4X4 worldMatrix, XMFLOAT4X4 viewMatrix, XMFLOAT4X4 projectionMatrix, LightClass* light);
		void destroy();

		bool testIntersection(CameraClass* camera, int, int);

		Point getPosition();
		void setPosition(Point pos);

		Point getRelativePosition();
		void setRelativePosition(Point pos);

		float getRadius();
		void setRadius(float radius);

	private:
		bool raySphereIntersect(XMFLOAT3, XMFLOAT3);

		Object3D*	collisionTest_;
		GraphicsManager* graphicsManager_;

		Point		position_;
		float		radius_;
};

#endif //_SPHERE_COLLISION_H_