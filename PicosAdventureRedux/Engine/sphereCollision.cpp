#include "sphereCollision.h"

SphereCollision::SphereCollision()
{
	collisionTest_ = 0;
}

SphereCollision::SphereCollision(const SphereCollision &)
{

}

SphereCollision::~SphereCollision()
{

}

void SphereCollision::setup(GraphicsManager *graphicsManager, Point relativePos, float radius)
{
	//Load collision text
	collisionTest_ = Object3DFactory::Instance()->CreateObject3D("StaticObject3D", graphicsManager, "sphere");
	collisionTest_->setScale(Vector(0.21f*radius, 0.21f*radius, 0.21f*radius));

	graphicsManager_ = graphicsManager;

	position_ = relativePos;

	radius_ = radius;
}

void SphereCollision::draw(ID3D11Device* device, ID3D11DeviceContext* deviceContext, XMFLOAT4X4 worldMatrix, XMFLOAT4X4 viewMatrix, XMFLOAT4X4 projectionMatrix, LightClass* light)
{
	collisionTest_->draw(device, deviceContext, worldMatrix, viewMatrix, projectionMatrix, light);
}

void SphereCollision::destroy()
{
	if(collisionTest_)
	{
		collisionTest_->destroy();
		delete collisionTest_;
		collisionTest_ = 0;
	}
}

Point SphereCollision::getPosition()
{
	return collisionTest_->getPosition();
}

void SphereCollision::setPosition(Point pos)
{
	collisionTest_->setPosition(Point(pos.x+position_.x, pos.y+position_.y, pos.z+position_.z));
}

Point SphereCollision::getRelativePosition()
{
	return position_;
}

void SphereCollision::setRelativePosition(Point pos)
{
	position_ = pos;
}

float SphereCollision::getRadius()
{
	return radius_;
}

void SphereCollision::setRadius(float radius)
{
	collisionTest_->setScale(Vector(0.21f*radius, 0.21f*radius, 0.21f*radius));

	radius_ = radius;
}

bool SphereCollision::testIntersection(CameraClass* camera, int mouseX, int mouseY)
{
	float pointX, pointY;
	XMFLOAT4X4 projectionMatrix, viewMatrix, inverseViewMatrix, worldMatrix, translateMatrix, inverseWorldMatrix;
	XMFLOAT3 direction, origin, rayOrigin, rayDirection;
	bool intersect;
	int width, height;

	graphicsManager_->getScreenSize(width, height);

	// Move the mouse cursor coordinates into the -1 to +1 range.
	pointX = ((2.0f * (float)mouseX) / (float)width) - 1.0f;
	pointY = (((2.0f * (float)mouseY) / (float)height) - 1.0f) * -1.0f;
		
	// Adjust the points using the projection matrix to account for the aspect ratio of the viewport.
	graphicsManager_->getProjectionMatrix(projectionMatrix);
	pointX = pointX / projectionMatrix._11;
	pointY = pointY / projectionMatrix._22;

	// Get the inverse of the view matrix.
	camera->getViewMatrix(viewMatrix);
	XMVECTOR pDeterminant;
	XMStoreFloat4x4(&inverseViewMatrix, XMMatrixInverse(&pDeterminant, XMLoadFloat4x4(&viewMatrix)));
	//D3DXMatrixInverse(&inverseViewMatrix, NULL, &viewMatrix);

	// Calculate the direction of the picking ray in view space.
	direction.x = (pointX * inverseViewMatrix._11) + (pointY * inverseViewMatrix._21) + inverseViewMatrix._31;
	direction.y = (pointX * inverseViewMatrix._12) + (pointY * inverseViewMatrix._22) + inverseViewMatrix._32;
	direction.z = (pointX * inverseViewMatrix._13) + (pointY * inverseViewMatrix._23) + inverseViewMatrix._33;

	// Get the origin of the picking ray which is the position of the camera.
	origin = camera->getPosition();

	// Get the world matrix and translate to the location of the sphere.
	graphicsManager_->getWorldMatrix(worldMatrix);
	XMStoreFloat4x4(&translateMatrix, XMMatrixTranslation(collisionTest_->getPosition().x, collisionTest_->getPosition().y, collisionTest_->getPosition().z));
	XMStoreFloat4x4(&worldMatrix, XMMatrixMultiply(XMLoadFloat4x4(&worldMatrix), XMLoadFloat4x4(&translateMatrix)));

	// Now get the inverse of the translated world matrix.
	XMStoreFloat4x4(&inverseWorldMatrix, XMMatrixInverse(&pDeterminant, XMLoadFloat4x4(&worldMatrix)));

	// Now transform the ray origin and the ray direction from view space to world space.
	XMStoreFloat3(&rayOrigin, XMVector3TransformCoord(XMLoadFloat3(&origin), XMLoadFloat4x4(&inverseWorldMatrix)));
	XMStoreFloat3(&rayDirection, XMVector3TransformNormal(XMLoadFloat3(&direction), XMLoadFloat4x4(&inverseWorldMatrix)));

	// Normalize the ray direction.
	XMStoreFloat3(&rayDirection, XMVector3Normalize(XMLoadFloat3(&rayDirection)));

	// Now perform the ray-sphere intersection test.
	intersect = raySphereIntersect(rayOrigin, rayDirection);
	
	if(intersect == true)
	{
		// If it does intersect then set the intersection to "yes" in the text string that is displayed to the screen.
		return true;
	}
	else
	{
		// If not then set the intersection to "No".
		return false;
	}

	return false;
}

bool SphereCollision::raySphereIntersect(XMFLOAT3 rayOrigin, XMFLOAT3 rayDirection)
{
	float a, b, c, discriminant;


	// Calculate the a, b, and c coefficients.
	a = (rayDirection.x * rayDirection.x) + (rayDirection.y * rayDirection.y) + (rayDirection.z * rayDirection.z);
	b = ((rayDirection.x * rayOrigin.x) + (rayDirection.y * rayOrigin.y) + (rayDirection.z * rayOrigin.z)) * 2.0f;
	c = ((rayOrigin.x * rayOrigin.x) + (rayOrigin.y * rayOrigin.y) + (rayOrigin.z * rayOrigin.z)) - (radius_ * radius_);

	// Find the discriminant.
	discriminant = (b * b) - (4 * a * c);

	// if discriminant is negative the picking ray missed the sphere, otherwise it intersected the sphere.
	if (discriminant < 0.0f)
	{
		return false;
	}

	return true;
}