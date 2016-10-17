////////////////////////////////////////////////////////////////////////////////
// Filename: cameraclass.cpp
////////////////////////////////////////////////////////////////////////////////
#include "cameraClass.h"

CameraClass::CameraClass()
{
	positionX_ = 0.0f;
	positionY_ = 0.0f;
	positionZ_ = 0.0f;

	rotationX_ = 0.0f;
	rotationY_ = 0.0f;
	rotationZ_ = 0.0f;
}

CameraClass::CameraClass(const CameraClass& camera)
{
}

CameraClass::~CameraClass()
{
}

void CameraClass::setup(XMFLOAT3 position, XMFLOAT3 rotation)
{
	XMVECTOR up, lookAt, positionVector;
	float yaw, pitch, roll;
	XMFLOAT4X4 rotationMatrix;

	// Setup the vector that points upwards.
	up = XMVectorSet( 0.0f, 1.0f, 0.0f, 1.0f);

	// Setup the position of the camera in the world.
	positionX_ = position.x;
	positionY_ = position.y;
	positionZ_ = position.z;

	// Setup the position into a XMVECTOR
	positionVector = XMVectorSet( position.x, position.y, position.z, 1.0f);

	// Setup where the camera is looking by default.
	lookAt = XMVectorSet( 0.0f, 0.0f, 1.0f, 1.0f);

	// Set the yaw (Y axis), pitch (X axis), and roll (Z axis) rotations in radians.
	pitch = rotation.x * 0.0174532925f;
	yaw   = rotation.y * 0.0174532925f;
	roll  = rotation.z * 0.0174532925f;

	rotationX_ = rotation.x;
	rotationY_ = rotation.y;
	rotationZ_ = rotation.z;

	// Create the rotation matrix from the yaw, pitch, and roll values.
	XMStoreFloat4x4(&rotationMatrix, XMMatrixRotationRollPitchYaw(pitch, yaw, roll));

	// Transform the lookAt and up vector by the rotation matrix so the view is correctly rotated at the origin.
	lookAt = XMVector3TransformCoord(lookAt, XMLoadFloat4x4(&rotationMatrix));
	up = XMVector3TransformCoord(up, XMLoadFloat4x4(&rotationMatrix));

	// Translate the rotated camera position to the location of the viewer.
	lookAt = positionVector + lookAt;

	// Finally create the view matrix from the three updated vectors.
	XMStoreFloat4x4(&viewMatrix_, XMMatrixLookAtLH(positionVector, lookAt, up));

	return;
}

void CameraClass::update()
{
	XMVECTOR up, position, lookAt;
	float yaw, pitch, roll;
	XMFLOAT4X4 rotationMatrix;

	// Setup the vector that points upwards.
	up = XMVectorSet( 0.0f, 1.0f, 0.0f, 1.0f);

	// Setup the position into a XMVECTOR
	position = XMVectorSet( positionX_, positionY_, positionZ_, 1.0f);

	// Setup where the camera is looking by default.
	lookAt = XMVectorSet( 0.0f, 0.0f, 1.0f, 1.0f);

	// Set the yaw (Y axis), pitch (X axis), and roll (Z axis) rotations in radians.
	pitch = rotationX_ * 0.0174532925f;
	yaw   = rotationY_ * 0.0174532925f;
	roll  = rotationZ_ * 0.0174532925f;

	// Create the rotation matrix from the yaw, pitch, and roll values.
	XMStoreFloat4x4(&rotationMatrix, XMMatrixRotationRollPitchYaw(pitch, yaw, roll));

	// Transform the lookAt and up vector by the rotation matrix so the view is correctly rotated at the origin.
	lookAt = XMVector3TransformCoord(lookAt, XMLoadFloat4x4(&rotationMatrix));
	up = XMVector3TransformCoord(up, XMLoadFloat4x4(&rotationMatrix));

	// Translate the rotated camera position to the location of the viewer.
	lookAt = position + lookAt;

	// Finally create the view matrix from the three updated vectors.
	XMStoreFloat4x4(&viewMatrix_, XMMatrixLookAtLH(position, lookAt, up));

	return;
}

void CameraClass::setPosition(float x, float y, float z)
{
	positionX_ = x;
	positionY_ = y;
	positionZ_ = z;
	return;
}

void CameraClass::setRotation(float x, float y, float z)
{
	rotationX_ = x;
	rotationY_ = y;
	rotationZ_ = z;
	return;
}

XMFLOAT3 CameraClass::getPosition()
{
	return XMFLOAT3(positionX_, positionY_, positionZ_);
}

XMFLOAT3 CameraClass::getRotation()
{
	return XMFLOAT3(rotationX_, rotationY_, rotationZ_);
}

void CameraClass::getViewMatrix(XMFLOAT4X4& viewMatrix)
{
	viewMatrix = viewMatrix_;
	return;
}