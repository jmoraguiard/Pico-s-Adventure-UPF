#ifndef _PARTICLE_SYSTEM_H_
#define _PARTICLE_SYSTEM_H_

#pragma warning( disable : 4005 )

#include <d3d11.h>
#include <xnamath.h>

#include "../Graphics/graphicsManager.h"
#include "../Graphics/textureArrayClass.h"
#include "../Graphics/shader3DClass.h"

#include "../Math/pointClass.h"

class ParticleSystem
{
	struct ParticleType
		{
			Point position;
			float red, green, blue;
			Point velocity;
			bool active;
		};

	public:
		ParticleSystem();
		ParticleSystem(const ParticleSystem&);
		~ParticleSystem();

		bool setup(GraphicsManager* graphicsManager, std::string fileName, Point initialPosition, float fallDistance, float particlesPerSecond, float maxParticles, XMFLOAT4 color, bool randColor);
		void update(float elapsedTime, float emit);
		void draw(ID3D11DeviceContext* deviceContext, XMFLOAT4X4 worldMatrix, XMFLOAT4X4 viewMatrix, XMFLOAT4X4 projectionMatrix, LightClass* light);
		void destroy();

		void setPosition(Point position);
		void setParticleColor(XMFLOAT4 color);
		void setParticlesDeviation(Point deviation);
		void setParticleDistance(float distance);
		void setParticlesVelocity(Point velocity, Point velocityVariation);
		void setParticleSize(float size);

		ID3D11ShaderResourceView* getTexture();
		int getIndexCount();

	private:
		bool loadTexture(ID3D11Device* device, std::string fileName);
		void destroyTexture();

		bool setupParticleSystem(Point initialPosition, float fallDistance, float particlesPerSecond, float maxParticles);
		void destroyParticleSystem();

		bool initializeBuffers(ID3D11Device* device);
		void destroyBuffers();

		void emitParticles(float);
		void updateParticles(float);
		void killParticles();

		bool updateBuffers(ID3D11DeviceContext* deviceContext);
		void renderBuffers(ID3D11DeviceContext* deviceContext);

		Point particleInitialPosition_;
		Point particleDeviation_;
		Point particleVelocity_;
		Point particleVelocityVariation_;
		float particleSize_, particlesPerSecond_;
		int maxParticles_;
		float fallingDistance_;
		XMFLOAT4 particleColor_;
		bool randColor_;

		int currentParticleCount_;
		float accumulatedTime_;

		TextureArrayClass* textureArray_;
		ParticleType* particleList_;

		int vertexCount_, indexCount_;
		ColorVertexType* vertices_;
		ID3D11Buffer* vertexBuffer_;
		ID3D11Buffer* indexBuffer_;

		GraphicsManager* graphicsManager_;
		Shader3DClass*	particlesShader_;
};

#endif //_PARTICLE_SYSTEM_H_