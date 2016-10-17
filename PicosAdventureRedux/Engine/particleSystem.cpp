#include "particleSystem.h"

ParticleSystem::ParticleSystem()
{
	textureArray_ = 0;
	particleList_ = 0;
	vertices_ = 0;
	vertexBuffer_ = 0;
	indexBuffer_ = 0;
}

ParticleSystem::ParticleSystem(const ParticleSystem& other)
{

}

ParticleSystem::~ParticleSystem()
{

}
bool ParticleSystem::setup(GraphicsManager* graphicsManager, std::string textureFilename, Point initialPosition, 
						   float fallDistance, float particlesPerSecond, float maxParticles, XMFLOAT4 color, bool randColor)
{
	bool result;

	graphicsManager_ = graphicsManager;

	particleColor_ = color;
	randColor_ = randColor;

	// Load the texture that is used for the particles.
	result = loadTexture(graphicsManager->getDevice(), textureFilename);
	if(!result)
	{
		MessageBoxA(NULL, "Could not load texture.", "ParticleSystem - Error", MB_ICONERROR | MB_OK);
		return false;
	}

	// Initialize the particle system.
	result = setupParticleSystem(initialPosition, fallDistance, particlesPerSecond, maxParticles);
	if(!result)
	{
		MessageBoxA(NULL, "Could not setup particle system.", "ParticleSystem - Error", MB_ICONERROR | MB_OK);
		return false;
	}

	// Create the buffers that will be used to render the particles with.
	result = initializeBuffers(graphicsManager->getDevice());
	if(!result)
	{
		MessageBoxA(NULL, "Could not initialize buffers.", "ParticleSystem - Error", MB_ICONERROR | MB_OK);
		return false;
	}

	particlesShader_ = graphicsManager->getColorShader3D();

	return true;
}

void ParticleSystem::update(float elapsedTime, float emit)
{
	bool result;

	// Release old particles.
	killParticles();

	// Emit new particles.
	if(emit)
	{
		emitParticles(elapsedTime);
	}
	
	// Update the position of the particles.
	updateParticles(elapsedTime);

	// Update the dynamic vertex buffer with the new position of each particle.
	result = updateBuffers(graphicsManager_->getDeviceContext());
}

void ParticleSystem::draw(ID3D11DeviceContext* deviceContext, XMFLOAT4X4 worldMatrix, XMFLOAT4X4 viewMatrix, XMFLOAT4X4 projectionMatrix, LightClass* light)
{
	// Put the vertex and index buffers on the graphics pipeline to prepare them for drawing.
	renderBuffers(deviceContext);

	particlesShader_->draw(deviceContext, getIndexCount(), worldMatrix, viewMatrix, projectionMatrix, textureArray_->getTexturesArray(), light);

	return;
}

void ParticleSystem::destroy()
{
	// Release the buffers.
	destroyBuffers();

	// Release the particle system.
	destroyParticleSystem();

	// Release the texture used for the particles.
	destroyTexture();

	return;
}

void ParticleSystem::setPosition(Point position)
{
	particleInitialPosition_.x = position.x;
	particleInitialPosition_.y = position.y;
	particleInitialPosition_.z = position.z;
}

void ParticleSystem::setParticleColor(XMFLOAT4 color)
{
	particleColor_ = color;

	for(int i=0; i < currentParticleCount_; i++)
	{
		particleList_[i].red = color.x;
		particleList_[i].green = color.y;
		particleList_[i].blue = color.z;
	}
}

void ParticleSystem::setParticlesDeviation(Point deviation)
{
	particleDeviation_ = deviation;
}

void ParticleSystem::setParticleDistance(float distance)
{
	fallingDistance_ = distance;
}

void ParticleSystem::setParticlesVelocity(Point velocity, Point velocityVariation)
{
	particleVelocity_ = velocity;
	particleVelocityVariation_ = velocityVariation;
}

void ParticleSystem::setParticleSize(float size)
{
	particleSize_ = size;
}

ID3D11ShaderResourceView* ParticleSystem::getTexture()
{
	return textureArray_->getTexturesArray()[0];
}

int ParticleSystem::getIndexCount()
{
	return indexCount_;
}

bool ParticleSystem::loadTexture(ID3D11Device* device, std::string fileName)
{
	bool result;


	// Create the texture object.
	textureArray_ = new TextureArrayClass;
	if(!textureArray_)
	{
		MessageBoxA(NULL, "Could not create the texture array instance.", "ParticleSystem - Error", MB_OK);
		return false;
	}

	std::string filePath = "./Data/images/" + fileName + ".dds";

	// Initialize the texture object.
	result = textureArray_->setup(device, filePath, "", "", 1);
	if(!result)
	{
		MessageBoxA(NULL, "Could not load the texture.", "ParticleSystem - Error", MB_OK);
		return false;
	}

	return true;
}

void ParticleSystem::destroyTexture()
{
	// Release the texture object.
	if(textureArray_)
	{
		textureArray_->destroy();
		delete textureArray_;
		textureArray_ = 0;
	}

	return;
}

bool ParticleSystem::setupParticleSystem(Point initialPosition, float fallDistance, float particlesPerSecond, float maxParticles)
{
	int i;

	// Set initial position of the particles
	particleInitialPosition_.x = initialPosition.x;
	particleInitialPosition_.y = initialPosition.y;
	particleInitialPosition_.z = initialPosition.z;

	// Set the random deviation of where the particles can be located when emitted.
	particleDeviation_.x = 0.1f;
	particleDeviation_.y = 0.05f;
	particleDeviation_.z = 0.5f;

	// Set the speed and speed variation of particles.
	particleVelocity_.x = 0.0f;
	particleVelocity_.y = 1.0f;
	particleVelocity_.z = 0.0f;

	particleVelocityVariation_.x = 0.0f;
	particleVelocityVariation_.y = 0.2f;
	particleVelocityVariation_.z = 0.0f;

	// Set the physical size of the particles.
	particleSize_ = 0.12f;

	// Set the number of particles to emit per second.
	particlesPerSecond_ = particlesPerSecond_;

	// Set the maximum falling distance for particles
	fallingDistance_ = fallDistance;

	// Set the maximum number of particles allowed in the particle system.
	maxParticles_ = maxParticles;

	// Create the particle list.
	particleList_ = new ParticleType[maxParticles_];
	if(!particleList_)
	{
		MessageBoxA(NULL, "Could not load create the particle array.", "ParticleSystem - Error", MB_OK);
		return false;
	}

	// Initialize the particle list.
	for(i=0; i<maxParticles_; i++)
	{
		particleList_[i].active = false;
	}

	// Initialize the current particle count to zero since none are emitted yet.
	currentParticleCount_ = 0;

	// Clear the initial accumulated time for the particle per second emission rate.
	accumulatedTime_ = 0.0f;

	return true;
}

void ParticleSystem::destroyParticleSystem()
{
	// Release the particle list.
	if(particleList_)
	{
		delete [] particleList_;
		particleList_ = 0;
	}

	return;
}

bool ParticleSystem::initializeBuffers(ID3D11Device* device)
{
	unsigned long* indices;
	int i;
	D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
	D3D11_SUBRESOURCE_DATA vertexData, indexData;
	HRESULT result;


	// Set the maximum number of vertices in the vertex array.
	vertexCount_ = maxParticles_ * 6;

	// Set the maximum number of indices in the index array.
	indexCount_ = vertexCount_;

	// Create the vertex array for the particles that will be rendered.
	vertices_ = new ColorVertexType[vertexCount_];
	if(!vertices_)
	{
		MessageBoxA(NULL, "Could not load create the vertices array.", "ParticleSystem - Error", MB_OK);
		return false;
	}

	// Create the index array.
	indices = new unsigned long[indexCount_];
	if(!indices)
	{
		MessageBoxA(NULL, "Could not load create the indices array.", "ParticleSystem - Error", MB_OK);
		return false;
	}

	// Initialize vertex array to zeros at first.
	memset(vertices_, 0, (sizeof(ColorVertexType) * vertexCount_));

	// Initialize the index array.
	for(i=0; i<indexCount_; i++)
	{
		indices[i] = i;
	}

	// Set up the description of the dynamic vertex buffer.
	vertexBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	vertexBufferDesc.ByteWidth = sizeof(ColorVertexType) * vertexCount_;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	// Give the subresource structure a pointer to the vertex data.
	vertexData.pSysMem = vertices_;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	// Now finally create the vertex buffer.
	result = device->CreateBuffer(&vertexBufferDesc, &vertexData, &vertexBuffer_);
	if(FAILED(result))
	{
		return false;
	}

	// Set up the description of the static index buffer.
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(unsigned long) * indexCount_;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;

	// Give the subresource structure a pointer to the index data.
	indexData.pSysMem = indices;
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;

	// Create the index buffer.
	result = device->CreateBuffer(&indexBufferDesc, &indexData, &indexBuffer_);
	if(FAILED(result))
	{
		return false;
	}

	// Release the index array since it is no longer needed.
	delete [] indices;
	indices = 0;

	return true;
}

void ParticleSystem::destroyBuffers()
{
	// Release the index buffer.
	if(vertexBuffer_)
	{
		vertexBuffer_->Release();
		vertexBuffer_ = 0;
	}

	// Release the vertex buffer.
	if(indexBuffer_)
	{
		indexBuffer_->Release();
		indexBuffer_ = 0;
	}

	return;
}

void ParticleSystem::emitParticles(float elapsedTime)
{
	bool emitParticle, found;
	float positionX, positionY, positionZ, red, green, blue;
	Point velocity;
	int index, i, j;

	// Increment the frame time.
	accumulatedTime_ += elapsedTime;

	// Set emit particle to false for now.
	emitParticle = false;
	
	// Check if it is time to emit a new particle or not.
	if(accumulatedTime_ > (1000/particlesPerSecond_))
	{
		accumulatedTime_ = 0.0f;
		emitParticle = true;
	}

	// If there are particles to emit then emit one per frame.
	if((emitParticle == true) && (currentParticleCount_ < (maxParticles_ - 1)))
	{
		currentParticleCount_++;

		// Now generate the randomized particle properties.
		positionX = ((((float)rand()-(float)rand())/RAND_MAX) * particleDeviation_.x)+particleInitialPosition_.x;
		positionY = ((((float)rand()-(float)rand())/RAND_MAX) * particleDeviation_.y)+particleInitialPosition_.y;
		positionZ = ((((float)rand()-(float)rand())/RAND_MAX) * particleDeviation_.z)+particleInitialPosition_.z;

		velocity.x = particleVelocity_.x + (((float)rand()-(float)rand())/RAND_MAX) * particleVelocityVariation_.x;
		velocity.y = particleVelocity_.y + (((float)rand()-(float)rand())/RAND_MAX) * particleVelocityVariation_.y;
		velocity.z = particleVelocity_.z + (((float)rand()-(float)rand())/RAND_MAX) * particleVelocityVariation_.z;

		if(randColor_)
		{
			red   = (((float)rand()-(float)rand())/RAND_MAX) + particleColor_.x;
			green = (((float)rand()-(float)rand())/RAND_MAX) + particleColor_.y;
			blue  = (((float)rand()-(float)rand())/RAND_MAX) + particleColor_.z;
		}
		else
		{
			red   = particleColor_.x;
			green = particleColor_.y;
			blue  = particleColor_.z;
		}

		// Now since the particles need to be rendered from back to front for blending we have to sort the particle array.
		// We will sort using Z depth so we need to find where in the list the particle should be inserted.
		index = 0;
		found = false;
		while(!found)
		{
			if((particleList_[index].active == false) || (particleList_[index].position.z < positionZ))
			{
				found = true;
			}
			else
			{
				index++;
			}
		}

		// Now that we know the location to insert into we need to copy the array over by one position from the index to make room for the new particle.
		i = currentParticleCount_;
		j = i - 1;

		while(i != index)
		{
			particleList_[i].position.x = particleList_[j].position.x;
			particleList_[i].position.y = particleList_[j].position.y;
			particleList_[i].position.z = particleList_[j].position.z;
			particleList_[i].red        = particleList_[j].red;
			particleList_[i].green      = particleList_[j].green;
			particleList_[i].blue       = particleList_[j].blue;
			particleList_[i].velocity   = particleList_[j].velocity;
			particleList_[i].active     = particleList_[j].active;
			i--;
			j--;
		}

		// Now insert it into the particle array in the correct depth order.
		particleList_[index].position.x = positionX;
		particleList_[index].position.y = positionY;
		particleList_[index].position.z = positionZ;
		particleList_[index].red        = red;
		particleList_[index].green      = green;
		particleList_[index].blue       = blue;
		particleList_[index].velocity   = velocity;
		particleList_[index].active     = true;
	}

	return;
}

void ParticleSystem::updateParticles(float elapsedTime)
{
	int i;


	// Each frame we update all the particles by making them move downwards using their position, velocity, and the frame time.
	for(i=0; i<currentParticleCount_; i++)
	{
		particleList_[i].position.x = particleList_[i].position.x - (particleList_[i].velocity.x * elapsedTime);
		particleList_[i].position.y = particleList_[i].position.y - (particleList_[i].velocity.y * elapsedTime);
		particleList_[i].position.z = particleList_[i].position.z - (particleList_[i].velocity.z * elapsedTime);
	}

	return;
}

void ParticleSystem::killParticles()
{
	int i, j;


	// Kill all the particles that have gone below a certain height range.
	for(i=0; i<maxParticles_; i++)
	{
		float dist2 = pow(particleList_[i].position.x - particleInitialPosition_.x, 2)+pow(particleList_[i].position.y - particleInitialPosition_.y, 2);

		if((particleList_[i].active == true) && ((dist2 > pow(fallingDistance_, 2))))
		{
			particleList_[i].active = false;
			currentParticleCount_--;

			// Now shift all the live particles back up the array to erase the destroyed particle and keep the array sorted correctly.
			for(j=i; j<maxParticles_-1; j++)
			{
				particleList_[j].position.x = particleList_[j+1].position.x;
				particleList_[j].position.y = particleList_[j+1].position.y;
				particleList_[j].position.z = particleList_[j+1].position.z;
				particleList_[j].red        = particleList_[j+1].red;
				particleList_[j].green      = particleList_[j+1].green;
				particleList_[j].blue       = particleList_[j+1].blue;
				particleList_[j].velocity   = particleList_[j+1].velocity;
				particleList_[j].active     = particleList_[j+1].active;
			}
		}
	}

	return;
}

bool ParticleSystem::updateBuffers(ID3D11DeviceContext* deviceContext)
{
	int index, i;
	HRESULT result;
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	ColorVertexType* verticesPtr;

	// Initialize vertex array to zeros at first.
	memset(vertices_, 0, (sizeof(ColorVertexType) * vertexCount_));

	// Now build the vertex array from the particle list array.  Each particle is a quad made out of two triangles.
	index = 0;

	for(i=0; i<currentParticleCount_; i++)
	{
		// Bottom left.
		vertices_[index].position = XMFLOAT3(particleList_[i].position.x - particleSize_, particleList_[i].position.y - particleSize_, particleList_[i].position.z);
		vertices_[index].texture = XMFLOAT2(0.0f, 1.0f);
		vertices_[index].color = XMFLOAT4(particleList_[i].red, particleList_[i].green, particleList_[i].blue, 1.0f);
		index++;

		// Top left.
		vertices_[index].position = XMFLOAT3(particleList_[i].position.x - particleSize_, particleList_[i].position.y + particleSize_, particleList_[i].position.z);
		vertices_[index].texture = XMFLOAT2(0.0f, 0.0f);
		vertices_[index].color = XMFLOAT4(particleList_[i].red, particleList_[i].green, particleList_[i].blue, 1.0f);
		index++;

		// Bottom right.
		vertices_[index].position = XMFLOAT3(particleList_[i].position.x + particleSize_, particleList_[i].position.y - particleSize_, particleList_[i].position.z);
		vertices_[index].texture = XMFLOAT2(1.0f, 1.0f);
		vertices_[index].color = XMFLOAT4(particleList_[i].red, particleList_[i].green, particleList_[i].blue, 1.0f);
		index++;

		// Bottom right.
		vertices_[index].position = XMFLOAT3(particleList_[i].position.x + particleSize_, particleList_[i].position.y - particleSize_, particleList_[i].position.z);
		vertices_[index].texture = XMFLOAT2(1.0f, 1.0f);
		vertices_[index].color = XMFLOAT4(particleList_[i].red, particleList_[i].green, particleList_[i].blue, 1.0f);
		index++;

		// Top left.
		vertices_[index].position = XMFLOAT3(particleList_[i].position.x - particleSize_, particleList_[i].position.y + particleSize_, particleList_[i].position.z);
		vertices_[index].texture = XMFLOAT2(0.0f, 0.0f);
		vertices_[index].color = XMFLOAT4(particleList_[i].red, particleList_[i].green, particleList_[i].blue, 1.0f);
		index++;

		// Top right.
		vertices_[index].position = XMFLOAT3(particleList_[i].position.x + particleSize_, particleList_[i].position.y + particleSize_, particleList_[i].position.z);
		vertices_[index].texture = XMFLOAT2(1.0f, 0.0f);
		vertices_[index].color = XMFLOAT4(particleList_[i].red, particleList_[i].green, particleList_[i].blue, 1.0f);
		index++;
	}
	
	// Lock the vertex buffer.
	result = deviceContext->Map(vertexBuffer_, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if(FAILED(result))
	{
		return false;
	}

	// Get a pointer to the data in the vertex buffer.
	verticesPtr = (ColorVertexType*)mappedResource.pData;

	// Copy the data into the vertex buffer.
	memcpy(verticesPtr, (void*)vertices_, (sizeof(ColorVertexType) * vertexCount_));

	// Unlock the vertex buffer.
	deviceContext->Unmap(vertexBuffer_, 0);

	return true;
}

void ParticleSystem::renderBuffers(ID3D11DeviceContext* deviceContext)
{
	unsigned int stride;
	unsigned int offset;


	// Set vertex buffer stride and offset.
	stride = sizeof(ColorVertexType); 
	offset = 0;
    
	// Set the vertex buffer to active in the input assembler so it can be rendered.
	deviceContext->IASetVertexBuffers(0, 1, &vertexBuffer_, &stride, &offset);

	// Set the index buffer to active in the input assembler so it can be rendered.
	deviceContext->IASetIndexBuffer(indexBuffer_, DXGI_FORMAT_R32_UINT, 0);

	// Set the type of primitive that should be rendered from this vertex buffer.
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	return;
}