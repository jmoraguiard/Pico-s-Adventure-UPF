#include "Object3D.h"

void Object3D::setName(std::string newName)
{
	objectName_ = newName;
}

std::string Object3D::getName()
{
	return objectName_;
}

std::string Object3D::getModelName()
{
	return model_->getModelName();
}

void Object3D::setPosition(Point position)
{
	position_.x = position.x;
	position_.y = position.y;
	position_.z = position.z;
}

Point Object3D::getPosition()
{
	return position_;
}

void Object3D::setScale(Vector scale)
{
	scaling_.x = scale.x;
	scaling_.y = scale.y;
	scaling_.z = scale.z;
}

Vector Object3D::getScale()
{
	return scaling_;
}

void Object3D::setRotationX(float rotX)
{
	rotX_ = rotX;
}

float Object3D::getRotationX()
{
	return rotX_;
}

void Object3D::setRotationY(float rotY)
{
	rotY_ = rotY;
}

float Object3D::getRotationY()
{
	return rotY_;
}

void Object3D::setRotationZ(float rotZ)
{
	rotZ_ = rotZ;
}

float Object3D::getRotationZ()
{
	return rotZ_;
}

ModelClass* Object3D::getModel()
{
	return model_;
}

TextureArrayClass* Object3D::getTextureArrayClass()
{
	return textures_;
}

void Object3D::setTextureArrayClass(TextureArrayClass* textureArray)
{
	textures_ = textureArray;
}

Shader3DClass* Object3D::getShader3D()
{
	return modelShader_;
}

void Object3D::setShader3D(Shader3DClass* shader)
{
	modelShader_ = shader;
}