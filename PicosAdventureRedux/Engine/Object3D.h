#ifndef _OBJECT_3D_H_
#define _OBJECT_3D_H_

#include "../Graphics/graphicsManager.h"
#include "../Graphics/modelClass.h"
#include "../Graphics/animatedCal3DModelClass.h"
#include "../Graphics/staticCal3DModelClass.h"
#include "../Graphics/OBJModelClass.h"
#include "../Graphics/textureArrayClass.h"
#include "../Graphics/textureClass.h"
#include "../Graphics/lightClass.h"

#include "../Math/pointClass.h"
#include "../Math/vectorClass.h"

class Object3D
{
	public:
		virtual bool setup(GraphicsManager* graphicsManager, std::string modelName, std::string objectName) = 0;
		virtual void update(float dt) = 0;
		virtual void draw(ID3D11Device* device, ID3D11DeviceContext* deviceContext, XMFLOAT4X4 worldMatrix, XMFLOAT4X4 viewMatrix, XMFLOAT4X4 projectionMatrix, LightClass* light) = 0;
		virtual void destroy() = 0;

		void setName(std::string newName);
		std::string getName();

		std::string getModelName();

		void setPosition(Point newPosition);
		Point getPosition();

		void setScale(Vector newScale);
		Vector getScale();

		void setRotationX(float rotX);
		float getRotationX();
		void setRotationY(float rotY);
		float getRotationY();
		void setRotationZ(float rotZ);
		float getRotationZ();

		ModelClass* getModel();

		TextureArrayClass* getTextureArrayClass();
		void setTextureArrayClass(TextureArrayClass* textureArray);

		Shader3DClass* getShader3D();
		void setShader3D(Shader3DClass* shader);

	protected:
		ModelClass*		model_;
		TextureArrayClass*	textures_;

		Shader3DClass*	modelShader_;

		std::string		objectName_;

		Point			position_;
		Vector			scaling_; 
		float			rotX_;
		float			rotY_; 
		float			rotZ_;
};

// Create function pointer
typedef Object3D* (__stdcall *CreateObject3DFn)(GraphicsManager*, std::string, std::string);

#endif //_OBJECT_3D_H_