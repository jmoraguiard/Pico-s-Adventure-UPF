#include "picoFourthClass.h"

PicoFourthClass::PicoFourthClass()
{
	body_ = 0;
	tips_ = 0;

	eyes_ = 0;

	hat_ = 0;

	waitedTime_ = 0.0f;
	eatingWaitTime_ = 0.0f;
	celebratingWaitTime_ = 0.0f;

	position_.x = 0.0f;
	position_.y = 0.0f;
	position_.z = 0.0f;

	velocity_.x = 0.0f;
	velocity_.y = 0.0f;
	velocity_.z = 0.0f;

	objective_.x = 0.0f;
	objective_.y = 0.0f;
	objective_.z = 0.0f;
	
	lookAt_.x = 0.0f;
	lookAt_.y = 0.0f;
	lookAt_.z = 0.0f;

	scaling_.x = 1.0f;
	scaling_.y = 1.0f;
	scaling_.z = 1.0f;
	
	rotX_ = 0.0f;
	rotY_ = 0.0f; 
	newRotY_ = 0.0f;
	rotZ_ = 0.0f;
}

PicoFourthClass::PicoFourthClass(const PicoFourthClass& other)
{

}

PicoFourthClass::~PicoFourthClass()
{

}

bool PicoFourthClass::setup(GraphicsManager* graphicsManager, CameraClass* camera, SoundFourthClass* soundManager, int num)
{
	camera_ = camera;
	soundManager_ = soundManager;

	loadConfiguration(graphicsManager, num);
	loadExpressions(graphicsManager);

	position_.x = 0.0f;
	position_.y = -20.0f;
	position_.z = 0.0f;

	scaling_.x = 0.036f;
	scaling_.y = 0.036f;
	scaling_.z = 0.036f;

	lookAtCamera(false);

	//tipsColor_ = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);

	tipsLight_ = new LightClass;
	tipsLight_->setDiffuseColor(tipsColor_.x, tipsColor_.y, tipsColor_.z, 1.0f);

	faceState_ = NORMAL;

	expressionChangeTime_ = 0.75f;
	expressionPercentage_ = 0.0f;
	actualExpression_ = "normal";
	newExpression_ = "normal";

	MultiTextureShader3DClass* multitextureShader = dynamic_cast<MultiTextureShader3DClass*>(tips_->getShader3D());
	multitextureShader->setPercentage(0.1);

	// Setup clock at the end so it starts when we run
	expressionClock_ = new ClockClass();
	if(!expressionClock_)
	{
		return false;
	}
	expressionClock_->reset();

	inactivityClock_ = new ClockClass();
	if(!inactivityClock_)
	{
		return false;
	}
	inactivityClock_->reset();

	// We set the times to control behaviour flow
	eatingWaitTime_ = 2.0f;
	celebratingWaitTime_ = 1.2f;
	inactivityTime_ = 10.0f;

	int screenWidth, screenHeight;
	graphicsManager->getScreenSize(screenWidth, screenHeight);

	// Debug
	info_ = new TextClass();
	if(!info_)
	{
		return false;
	}

	// Initialize the text object.
	if(!info_->setup(graphicsManager->getDevice(), graphicsManager->getDeviceContext(), graphicsManager->getShader2D(), screenWidth, screenHeight, 20, 70, "Rot: "))
	{
		MessageBoxA(NULL, "Could not initialize the FPS text object.", "GUIFrame - Error", MB_OK);
		return false;
	}

	makeRest(false);

	return true;
}

void PicoFourthClass::update(float elapsedTime)
{
	expressionClock_->tick();
	inactivityClock_->tick();

	body_->update(elapsedTime);
	tips_->update(elapsedTime);

	// Update textures for the tips
	tips_->getTextureArrayClass()->getTexturesArray()[0] = expressions_.at(actualExpression_)->getTexture();
	tips_->getTextureArrayClass()->getTexturesArray()[1] = expressions_.at(newExpression_)->getTexture();

	MultiTextureShader3DClass* multitextureShader = dynamic_cast<MultiTextureShader3DClass*>(tips_->getShader3D());
	multitextureShader->setPercentage(expressionPercentage_);

	eyes_->update(elapsedTime);
	
	hat_->update(elapsedTime);

	switch(picoState_)
	{
		case WAITING:
			{
				
			}
			break;
		case WALKING:
			{
				walk(elapsedTime);
			}
			break;
		case TURNING:
			{
				
			}
			break;
		case CELEBRATING:
			{
				
			}
			break;
		case LEAVING:
			{
				walk(elapsedTime);
			}
			break;
		default:
			{
				
			}
			break;

	}

	switch(faceState_)
	{
		case NORMAL:
			{
				expressionPercentage_ = 0.0f;
			}
			break;
		case CHANGING:
			{
				expressionPercentage_ = expressionClock_->getTime()/expressionChangeTime_;
				if(expressionClock_->getTime() > expressionChangeTime_)
				{
					faceState_ = CHANGED;
				}
			}
			break;
		case CHANGED:
			{
				expressionPercentage_ = 1.0f;
				actualExpression_ = newExpression_;
				faceState_ = NORMAL;
			}
			break;
	}
}

void PicoFourthClass::draw(GraphicsManager* graphicsManager, XMFLOAT4X4 worldMatrix, XMFLOAT4X4 viewMatrix, XMFLOAT4X4 projectionMatrix, LightClass* light, bool debug)
{
	if(debug)
	{
		std::stringstream RotText;
		RotText << "RotY: " << rotY_ << " NewRotY: " << newRotY_;
		info_->setText(RotText.str(), graphicsManager->getDeviceContext());

		XMFLOAT4X4 orthoMatrix;
		graphicsManager->getOrthoMatrix(orthoMatrix);

		graphicsManager->turnZBufferOff();
		graphicsManager->turnOnAlphaBlending();
			info_->draw(graphicsManager->getDeviceContext(), worldMatrix, viewMatrix, orthoMatrix);
		graphicsManager->turnOffAlphaBlending();
		graphicsManager->turnZBufferOn();
	}

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

	body_->draw(graphicsManager->getDevice(), graphicsManager->getDeviceContext(), worldMatrix, viewMatrix, projectionMatrix, light);
	eyes_->draw(graphicsManager->getDevice(), graphicsManager->getDeviceContext(), worldMatrix, viewMatrix, projectionMatrix, light);
	
	hat_->draw(graphicsManager->getDevice(), graphicsManager->getDeviceContext(), worldMatrix, viewMatrix, projectionMatrix, light);

	tipsLight_->setAmbientColor(light->getAmbientColor().x, light->getAmbientColor().y, light->getAmbientColor().z, 1.0f);
	tipsLight_->setDiffuseColor(tipsColor_.x*light->getDiffuseColor().x, tipsColor_.y*light->getDiffuseColor().y, tipsColor_.z*light->getDiffuseColor().z, 1.0f);
	tipsLight_->setDirection(light->getDirection().x, light->getDirection().y, light->getDirection().z);

	tips_->draw(graphicsManager->getDevice(), graphicsManager->getDeviceContext(), worldMatrix, viewMatrix, projectionMatrix, tipsLight_);
}

void PicoFourthClass::destroy()
{
	// Release the model object
	if(body_)
	{
		body_->destroy();
		delete body_;
		body_ = 0;
	}

	// Release the model object
	if(tips_)
	{
		tips_->destroy();
		delete tips_;
		tips_ = 0;
	}

	// Release the model object
	if(eyes_)
	{
		eyes_->destroy();
		delete eyes_;
		eyes_ = 0;
	}

	// Release the model object
	if(hat_)
	{
		hat_->destroy();
		delete hat_;
		hat_ = 0;
	}

	std::map<std::string, TextureClass*>::iterator it;
	for(it = expressions_.begin(); it != expressions_.end(); it++)
	{
		it->second->destroy();
	}
	expressions_.clear();
}

void PicoFourthClass::setLevelState(int level)
{
	levelState_ = level;
}

Point PicoFourthClass::getPosition()
{
	return position_;
}

void PicoFourthClass::setPosition(Point pos)
{
	position_ = pos;
}

bool PicoFourthClass::checkPicoArrivedObjective()
{
	if(position_.x < objective_.x+0.05 && position_.x > objective_.x-0.05 && position_.z < objective_.z+0.05 && position_.z > objective_.z-0.05)
	{
		return true;
	}

	return false;
}

bool PicoFourthClass::isPicoWaiting()
{
	if(picoState_ == WAITING)
	{
		return true;
	}

	return false;
}

void PicoFourthClass::goToPosition(Point position)
{
	changeAnimation("walk", 0.4f);

	objective_.x = position.x;
	objective_.y = position.y;
	objective_.z = position.z;

	picoState_ = WALKING;
}

void PicoFourthClass::makeCelebrate()
{
	changeExpression("feliz");
	executeAnimation("celebration", 0.2f);
	soundManager_->playCelebrate();
	inactivityClock_->reset();
}

void PicoFourthClass::makeDanceAss()
{
	changeExpression("feliz");
	executeAnimation("DanceAss", 0.2f);

	soundManager_->playCelebrate();
	inactivityClock_->reset();
}

void PicoFourthClass::makeGreeting()
{
	if(picoState_ == WAITING)
	{
		changeAnimation("hola", 0.4f);
		soundManager_->playHi();
		inactivityClock_->reset();
	}
}

void PicoFourthClass::makeHappy()
{
	if(picoState_ == WAITING)
	{
		changeExpression("feliz");
		inactivityClock_->reset();
	}
}

void PicoFourthClass::makeLeave()
{
	goToPosition(Point(-35.25f, 0.0f, -3.0f));
	picoState_ = LEAVING;
}

void PicoFourthClass::makeNo()
{
	changeExpression("normal");
	executeAnimation("negation", 0.2f);
	soundManager_->playSad();
	
	inactivityClock_->reset();
}

void PicoFourthClass::makePointing(bool right)
{
	if(right)
	{
		changeAnimation("point_right", 0.4f);
		changeExpression("normal");
	}
	else
	{
		changeAnimation("point_left", 0.4f);
		changeExpression("normal");
	}

	soundManager_->playPointing();

	inactivityClock_->reset();
}

void PicoFourthClass::makeRest(bool lookCamera)
{
	if(lookCamera)
	{
		if(lookAtCamera(false))
		{
			picoState_ = WAITING;
			inactivityClock_->reset();
		}
	}
	else
	{
		changeAnimation("idle", 0.2f);
		picoState_ = WAITING;
		inactivityClock_->reset();
	}
}

float PicoFourthClass::approach(float goal, float current, float dt)
{
	float difference = goal - current;

	if(difference > dt)
		return current + dt;
	if(difference < dt)
		return current - dt;

	return goal;
}

void PicoFourthClass::walk(float elapsedTime)
{
	velocity_.x = objective_.x - position_.x;
	velocity_.y = objective_.y - position_.y;
	velocity_.z = objective_.z - position_.z;

	Vector normalizedVelocity = velocity_.normalize();
	velocity_ = normalizedVelocity * 1.5f;

	float newRotY_ = acos(normalizedVelocity.z);
	if(normalizedVelocity.x < 0)
	{
		newRotY_ += 3.141592f;
	}

	if(rotY_ > newRotY_-0.1f && rotY_ < newRotY_+0.1f)
	{
		rotY_ = newRotY_;

		position_.x += velocity_.x*elapsedTime;
		position_.y += velocity_.y*elapsedTime;
		position_.z += velocity_.z*elapsedTime;
	}
	else
	{
		if(rotY_ < newRotY_)
		{
			rotY_ += 0.04f;
		}
		if(rotY_ > newRotY_)
		{
			rotY_ -= 0.04f;
		}
	}
}

bool PicoFourthClass::lookAtCamera(bool check)
{
	Point cameraPos = Point(camera_->getPosition().x, camera_->getPosition().y, camera_->getPosition().z);

	lookAt_.x = cameraPos.x-position_.x;
	lookAt_.y = cameraPos.y-position_.y;
	lookAt_.z = cameraPos.z-position_.z;

	Vector normalizedLookAt = lookAt_.normalize();

	newRotY_ = acos(normalizedLookAt.x);//3.141592f-1.570796f/4; 

	if(normalizedLookAt.x > 0)
	{
		newRotY_ += XM_PIDIV2;
	}
	else
	{
		newRotY_ += XM_PIDIV2;
	}

	if(check)
	{
		if(rotY_ > newRotY_-0.2f && rotY_ < newRotY_+0.2f)
		{
			rotY_ = newRotY_;

			return true;
		}
		else
		{
			if(rotY_ < newRotY_)
			{
				rotY_ += 0.04f;
			}
			if(rotY_ > newRotY_)
			{
				rotY_ -= 0.04f;
			}
		}

		return false;
	}
	else
	{
		rotY_ = newRotY_;

		return true;
	}
}

void PicoFourthClass::notify(InputManager* notifier, InputStruct arg)
{
	switch(arg.keyPressed){
		default:
			{
				
			}
			break;
	}
}

void PicoFourthClass::notify(FriendSpaceShipClass* notifier, bool arg)
{
	if(arg)
	{
		makeCelebrate();
	}
	else
	{
		makeNo();
	}
}

void PicoFourthClass::changeAnimation(std::string name, float time)
{
	AnimatedObject3D* animatedTemp;
	AnimatedCal3DModelClass* cal3dTemp;
	
	animatedTemp = dynamic_cast<AnimatedObject3D*>(body_);
	cal3dTemp = dynamic_cast<AnimatedCal3DModelClass*>(animatedTemp->getModel());
	cal3dTemp->setAnimationToPlay(name, time);
	animatedTemp = dynamic_cast<AnimatedObject3D*>(tips_);
	cal3dTemp = dynamic_cast<AnimatedCal3DModelClass*>(animatedTemp->getModel());
	cal3dTemp->setAnimationToPlay(name, time);
	animatedTemp = dynamic_cast<AnimatedObject3D*>(eyes_);
	cal3dTemp = dynamic_cast<AnimatedCal3DModelClass*>(animatedTemp->getModel());
	cal3dTemp->setAnimationToPlay(name, time);
	animatedTemp = dynamic_cast<AnimatedObject3D*>(hat_);
	cal3dTemp = dynamic_cast<AnimatedCal3DModelClass*>(animatedTemp->getModel());
	cal3dTemp->setAnimationToPlay(name, time);
}

void PicoFourthClass::executeAnimation(std::string name, float time)
{
	AnimatedObject3D* animatedTemp;
	AnimatedCal3DModelClass* cal3dTemp;

	animatedTemp = dynamic_cast<AnimatedObject3D*>(body_);
	cal3dTemp = dynamic_cast<AnimatedCal3DModelClass*>(animatedTemp->getModel());
	cal3dTemp->setAnimationToExecute(name, time);
	animatedTemp = dynamic_cast<AnimatedObject3D*>(tips_);
	cal3dTemp = dynamic_cast<AnimatedCal3DModelClass*>(animatedTemp->getModel());
	cal3dTemp->setAnimationToExecute(name, time);
	animatedTemp = dynamic_cast<AnimatedObject3D*>(eyes_);
	cal3dTemp = dynamic_cast<AnimatedCal3DModelClass*>(animatedTemp->getModel());
	cal3dTemp->setAnimationToExecute(name, time);
	animatedTemp = dynamic_cast<AnimatedObject3D*>(hat_);
	cal3dTemp = dynamic_cast<AnimatedCal3DModelClass*>(animatedTemp->getModel());
	cal3dTemp->setAnimationToExecute(name, time);
}

void PicoFourthClass::changeExpression(std::string newExpression)
{
	if(faceState_ != CHANGING && picoState_ != WALKING)
	{
		newExpression_ = newExpression;
		faceState_ = CHANGING;
		expressionClock_->reset();
	}
}

void PicoFourthClass::loadExpressions(GraphicsManager* graphicsManager)
{
	TextureClass* temp1 = new TextureClass;
	std::string filePath = "./Data/models/miniBossExtremidades/d-e-normal.dds";
	bool result = temp1->setup(graphicsManager->getDevice(), filePath);
	if(!result)
	{
		MessageBoxA(NULL, "Could not load the normal expression textures!", "Pico - Error", MB_ICONERROR | MB_OK);
	}
	expressions_.insert(std::pair<std::string, TextureClass*>("normal", temp1));

	TextureClass* temp2 = new TextureClass;
	filePath = "./Data/models/miniBossExtremidades/d-e-feliz.dds";
	result = temp2->setup(graphicsManager->getDevice(), filePath);
	if(!result)
	{
		MessageBoxA(NULL, "Could not load the feliz expression textures!", "Pico - Error", MB_ICONERROR | MB_OK);
	}
	expressions_.insert(std::pair<std::string, TextureClass*>("feliz", temp2));

	TextureClass* temp3 = new TextureClass;
	filePath = "./Data/models/miniBossExtremidades/d-e-sorpresa.dds";
	result = temp3->setup(graphicsManager->getDevice(), filePath);
	if(!result)
	{
		MessageBoxA(NULL, "Could not load the sorpresa expression textures!", "Pico - Error", MB_ICONERROR | MB_OK);
	}
	expressions_.insert(std::pair<std::string, TextureClass*>("sorpresa", temp3));

	TextureClass* temp4 = new TextureClass;
	filePath = "./Data/models/miniBossExtremidades/d-e-triste.dds";
	result = temp4->setup(graphicsManager->getDevice(), filePath);
	if(!result)
	{
		MessageBoxA(NULL, "Could not load the triste expression textures!", "Pico - Error", MB_ICONERROR | MB_OK);
	}
	expressions_.insert(std::pair<std::string, TextureClass*>("triste", temp4));

	TextureClass* temp5 = new TextureClass;
	filePath = "./Data/models/miniBossExtremidades/d-e-sorpresa2.dds";
	result = temp5->setup(graphicsManager->getDevice(), filePath);
	if(!result)
	{
		MessageBoxA(NULL, "Could not load the sorpresa2 expression textures!", "Pico - Error", MB_ICONERROR | MB_OK);
	}
	expressions_.insert(std::pair<std::string, TextureClass*>("sorpresa2", temp5));

	TextureClass* temp6 = new TextureClass;
	filePath = "./Data/models/miniBossExtremidades/d-e-superfeliz.dds";
	result = temp6->setup(graphicsManager->getDevice(), filePath);
	if(!result)
	{
		MessageBoxA(NULL, "Could not load the superfeliz expression textures!", "Pico - Error", MB_ICONERROR | MB_OK);
	}
	expressions_.insert(std::pair<std::string, TextureClass*>("superfeliz", temp6));

	TextureClass* temp7 = new TextureClass;
	filePath = "./Data/models/miniBossExtremidades/d-e-cabreo.dds";
	result = temp7->setup(graphicsManager->getDevice(), filePath);
	if(!result)
	{
		MessageBoxA(NULL, "Could not load the enfadado expression textures!", "Pico - Error", MB_ICONERROR | MB_OK);
	}
	expressions_.insert(std::pair<std::string, TextureClass*>("enfadado", temp7));
}

void PicoFourthClass::loadConfiguration(GraphicsManager* graphicsManager, int num)
{
	std::stringstream root;
	root << "./Data/configuration/level4/pico_configuration_" << num << ".xml";

	//Loading animations XML file
	pugi::xml_document picoDoc;
	if(!picoDoc.load_file(root.str().c_str()))
	{
		std::stringstream error;
		error << "Could not load pico .xml file!" << root.str();
		MessageBoxA(NULL, error.str().c_str(), "PicoSecond - Error", MB_ICONERROR | MB_OK);
	}

	pugi::xml_node root_node;
	// Get initial node
	if(!(root_node = picoDoc.child("pico")))
	{
		MessageBoxA(NULL, "Could not find the Pico configuration root node.", "PicoSecond - Error", MB_ICONERROR | MB_OK);
	}

	// Parse body
	pugi::xml_node bodyNode;
	bodyNode = root_node.child("body");

	pugi::xml_node bodyModel;
	bodyModel = bodyNode.child("model");
	pugi::xml_text bodyName = bodyModel.text();

	body_ = Object3DFactory::Instance()->CreateObject3D("AnimatedObject3D", graphicsManager, bodyName.as_string());
	//Shader3DClass* shaderTemp = Shader3DFactory::Instance()->CreateShader3D("PointlightDiffuseShader3D", graphicsManager);
	//body_->setShader3D(shaderTemp);

	pugi::xml_node bodyTexture;
	bodyTexture = bodyNode.child("texture");
	std::string textureName = bodyTexture.text().as_string();
	std::string filePath = "./Data/models/miniBossCuerpo/" + textureName + ".dds";
	TextureClass* temp;
	temp = new TextureClass;
	if(!temp)
	{
		MessageBoxA(NULL, "Could not create object for texture!", "PicoSecond - Error", MB_ICONERROR | MB_OK);
	}
	bool result = temp->setup(graphicsManager->getDevice(), filePath);
	if(!result)
	{
		MessageBoxA(NULL, "Could not load texture!", "PicoSecond - Error", MB_ICONERROR | MB_OK);
	}
	body_->getTextureArrayClass()->getTexturesArray()[0] = temp->getTexture();

	// Parse tips
	pugi::xml_node tipsNode;
	tipsNode = root_node.child("tips");

	pugi::xml_node tipsModel;
	tipsModel = tipsNode.child("model");
	pugi::xml_text tipsName = tipsModel.text();

	tips_ = Object3DFactory::Instance()->CreateObject3D("AnimatedObject3D", graphicsManager, tipsName.as_string());
	// Set specific multitexture shader for tips and increment textures array
	Shader3DClass* shaderTemp = Shader3DFactory::Instance()->CreateShader3D("MultiTextureShader3D", graphicsManager);
	tips_->setShader3D(shaderTemp);

	pugi::xml_node colorNode;
	colorNode = tipsNode.child("color");
	tipsColor_.x = colorNode.attribute("r").as_float();
	tipsColor_.y = colorNode.attribute("g").as_float();
	tipsColor_.z = colorNode.attribute("b").as_float();

	eyes_ = Object3DFactory::Instance()->CreateObject3D("AnimatedObject3D", graphicsManager, "miniBossOjos");
	//shaderTemp = Shader3DFactory::Instance()->CreateShader3D("PointlightDiffuseShader3D", graphicsManager);
	//eyes_->setShader3D(shaderTemp);
	
	// Parse hat
	pugi::xml_node hatModel;
	hatModel = root_node.child("hat");

	pugi::xml_text hatName = hatModel.text();

	hat_ = Object3DFactory::Instance()->CreateObject3D("AnimatedObject3D", graphicsManager, hatName.as_string());
	//shaderTemp = Shader3DFactory::Instance()->CreateShader3D("PointlightDiffuseShader3D", graphicsManager);
	//hat_->setShader3D(shaderTemp);

	tips_->getTextureArrayClass()->setNumberTextures(2);
}
