#include "StaticObject3D.h"

StaticObject3D::StaticObject3D()
{
	position_.x = 0.0f;
	position_.y = 0.0f;
	position_.z = 0.0f;

	scaling_.x = 1.0f;
	scaling_.y = 1.0f;
	scaling_.z = 1.0f;
	
	rotX_ = 0.0f;
	rotY_ = 0.0f; 
	rotZ_ = 0.0f;

	textures_ = 0;
}

StaticObject3D::StaticObject3D(const StaticObject3D &)
{
}

StaticObject3D::~StaticObject3D()
{
}

bool StaticObject3D::setup(GraphicsManager* graphicsManager, std::string modelName, std::string objectName)
{
	objectName_ = objectName;

	if(checkModelIsOBJ(modelName))
	{
		model_ = new OBJModelClass;
		if(!model_)
		{
			MessageBox(NULL, L"Could not instantiate the OBJ model.", L"StaticObject3D - Error", MB_ICONERROR | MB_OK);
			return true;
		}
	}
	else
	{
		model_ = new StaticCal3DModelClass;
		if(!model_)
		{
			MessageBox(NULL, L"Could not instantiate the CAL3D model.", L"StaticObject3D - Error", MB_ICONERROR | MB_OK);
			return false;
		}
	}

	// Initialize the model object.
	if(!model_->setup(graphicsManager->getDevice(), modelName))
	{
		MessageBox(NULL, L"Could not initialize the model object.", L"StaticObject3D - Error", MB_ICONERROR | MB_OK);
		return false;
	}

	textures_ = new TextureArrayClass;
	if(!textures_)
	{
		return false;
	}

	// Initialize the texture object.
	std::string diffuseFilePath = "./Data/models/" + modelName + "/d-" + modelName + ".dds";

	TextureClass* diffTemp = new TextureClass;
	bool result = diffTemp->setup(graphicsManager->getDevice(), diffuseFilePath);
	if(!result)
	{
		MessageBoxA(NULL, "Could not load diffuse texture!", "StaticObject3D - Error", MB_ICONERROR | MB_OK);
		return false;
	}

	std::string normalFilePath = "./Data/models/" + modelName + "/n-" + modelName + ".dds";
	TextureClass* normTemp = new TextureClass;
	result = normTemp->setup(graphicsManager->getDevice(), normalFilePath);
	if(!result)
	{
		result = textures_->setup(graphicsManager->getDevice(), diffuseFilePath, "", "", 1);
		if(!result)
		{
			MessageBoxA(NULL, "Could not load the textures (diffuse)!", "StaticObject3D - Error", MB_ICONERROR | MB_OK);
			return false;
		}
	}
	else
	{
		result = textures_->setup(graphicsManager->getDevice(), diffuseFilePath, normalFilePath, "", 2);
		if(!result)
		{
			MessageBoxA(NULL, "Could not load the textures (diffuse and normal)!", "StaticObject3D - Error", MB_ICONERROR | MB_OK);
			return false;
		}
	}

	modelShader_ = graphicsManager->getDiffuseShader3D();

	return true;
}

void StaticObject3D::update(float dt)
{
	// DO NOTHING
}

void StaticObject3D::draw(ID3D11Device* device, ID3D11DeviceContext* deviceContext, XMFLOAT4X4 worldMatrix, XMFLOAT4X4 viewMatrix, XMFLOAT4X4 projectionMatrix, LightClass* light)
{
	XMFLOAT4X4 rotatingMatrixZ;
	XMStoreFloat4x4(&rotatingMatrixZ, XMMatrixRotationZ(rotZ_));
	XMStoreFloat4x4(&worldMatrix, XMMatrixMultiply(XMLoadFloat4x4(&worldMatrix), XMLoadFloat4x4(&rotatingMatrixZ)));

	XMFLOAT4X4 rotatingMatrixY;
	XMStoreFloat4x4(&rotatingMatrixY, XMMatrixRotationY(rotY_));
	XMStoreFloat4x4(&worldMatrix, XMMatrixMultiply(XMLoadFloat4x4(&worldMatrix), XMLoadFloat4x4(&rotatingMatrixY)));

	XMFLOAT4X4 rotatingMatrixX;
	XMStoreFloat4x4(&rotatingMatrixX, XMMatrixRotationX(rotX_));
	XMStoreFloat4x4(&worldMatrix, XMMatrixMultiply(XMLoadFloat4x4(&worldMatrix), XMLoadFloat4x4(&rotatingMatrixX)));

	XMFLOAT4X4 scalingMatrix;
	XMStoreFloat4x4(&scalingMatrix, XMMatrixScaling(scaling_.x, scaling_.y, scaling_.z));
	XMStoreFloat4x4(&worldMatrix, XMMatrixMultiply(XMLoadFloat4x4(&worldMatrix), XMLoadFloat4x4(&scalingMatrix)));

	XMFLOAT4X4 movingMatrix;
	XMStoreFloat4x4(&movingMatrix, XMMatrixTranslation(position_.x, position_.y, position_.z));
	XMStoreFloat4x4(&worldMatrix, XMMatrixMultiply(XMLoadFloat4x4(&worldMatrix), XMLoadFloat4x4(&movingMatrix)));

	model_->draw(device, deviceContext);
	modelShader_->draw(deviceContext, model_->getIndexCount(), worldMatrix, viewMatrix, projectionMatrix, textures_->getTexturesArray(), light);
}

void StaticObject3D::destroy()
{
	if(textures_)
	{
		textures_->destroy();
		delete textures_;
		textures_ = 0;
	}

	if(model_)
	{
		model_->destroy();
		delete model_;
		model_ = 0;
	}

	/*if(modelShader_)
	{
		modelShader_->destroy();
		delete modelShader_;
		modelShader_ = 0;
	}*/
}

bool StaticObject3D::checkModelIsOBJ(const std::string &modelName)
{
	HANDLE dir;
    WIN32_FIND_DATAA file_data;

	// We specify in which folder we want to check whether there are animations or not
	std::string modelFolder = "./Data/models/" + modelName + "/*";
	bool isOBJ = false;

	// If we can access to the folder, we will start checking for the folder "anims"
	if ((dir = FindFirstFileA(modelFolder.c_str(), &file_data)) != INVALID_HANDLE_VALUE)
	{
		do {
    		const std::string file_name = file_data.cFileName;
    		const bool is_directory = (file_data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0;

    		if(file_name[0] == '.')
    			continue;

			// If we find a file, check if it is called modelName.obj
    		if(!is_directory)
			{
				std::string expectedNameMin = modelName+".obj";
				std::string expectedNameMaj = modelName+".OBJ";
				if(file_name == expectedNameMin || file_name == expectedNameMaj)
				{
					isOBJ = true;
				}
			}

			const std::string s( file_name.begin(), file_name.end() );
		} while (FindNextFileA(dir, &file_data));
	}

	// Return true in cas hasAnimations bool is set to true
	if(isOBJ)
	{
		return true;
	}
	
	return false;
}

Object3D* __stdcall StaticObject3D::Create(GraphicsManager* graphicsManager, std::string modelName, std::string objectName)
{
	StaticObject3D* staticObject3DTemp = new StaticObject3D();

	if(!staticObject3DTemp->setup(graphicsManager, modelName, objectName))
	{
		return NULL;
	}

	return staticObject3DTemp;
}