#ifndef _GRAPHICS_MANAGER_H_
#define _GRAPHICS_MANAGER_H_

#pragma warning( disable : 4005 )
//Including DirectX libraries
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "d3dx11.lib")
#pragma comment(lib, "d3dx10.lib")

#include <dxgi.h>
#include <d3d11.h>
#include <xnamath.h>

#include <iostream>
#include <fstream>
#include <sstream>

#include "lightClass.h"

#include "shader2DClass.h"
#include "shader3DFactory.h"

class GraphicsManager
{
	public:
		GraphicsManager();
		GraphicsManager(const GraphicsManager&);
		~GraphicsManager();

		bool setup(HWND windowHandler, bool vsync, bool fullscreen, float screenDepthPlane, float screenNearPlane);
		void destroy();

		void beginDraw(float red, float green, float blue, float alpha);
		void endDraw();

		bool setupShaders();

		ID3D11Device* getDevice();
		ID3D11DeviceContext* getDeviceContext();
		IDXGISwapChain* getSwapChain();

		Shader2DClass* getShader2D();
		Shader3DClass* getShader3D(std::string type);
		Shader3DClass* getColorShader3D();
		Shader3DClass* getDiffuseShader3D();
		Shader3DClass* getMultitextureShader3D();

		void getProjectionMatrix(XMFLOAT4X4& projectionMatrix);
		void getWorldMatrix(XMFLOAT4X4& worldMatrix);
		void getOrthoMatrix(XMFLOAT4X4& orthoMatrix);

		void getScreenSize(int&, int&);

		HWND getWindowHandler();

		void turnOnSolidRasterizer();
		void turnOnWireframeRasterizer();

		void turnZBufferOn();
		void turnZBufferOff();

		void turnOnAlphaBlending();
		void turnOnParticlesAlphaBlending();
		void turnOffAlphaBlending();

	private:
		HINSTANCE instanceHandler_;
		HWND windowHandler_;

		bool vsyncEnabled_;

		int screenWidth_;
		int screenHeight_;

		D3D_DRIVER_TYPE driverType_;
		D3D_FEATURE_LEVEL featureLevel_;

		// Pointer to all the variables, interfaces and structures to manage the rendering
		ID3D11Device* d3dDevice_;
		ID3D11DeviceContext* d3dDeviceContext_;
		IDXGISwapChain* swapChain_;
		ID3D11RenderTargetView* renderTargetView_;

		ID3D11Texture2D* depthStencilBuffer_;
		ID3D11DepthStencilState* depthStencilState_;
		ID3D11DepthStencilView* depthStencilView_;
		ID3D11DepthStencilState* depthDisabledStencilState_;

		ID3D11RasterizerState* solidRasterizerState_;
		ID3D11RasterizerState* wireframeRasterizerState_;
		
		ID3D11BlendState* alphaEnableBlendingState_;
		ID3D11BlendState* alphaEnableParticlesBlendingState_;
		ID3D11BlendState* alphaDisableBlendingState_;

		XMFLOAT4X4 projectionMatrix_;
		XMFLOAT4X4 worldMatrix_;
		XMFLOAT4X4 orthoMatrix_;

		// Pointers to shaders
		Shader2DClass* shader2D_;
		Shader3DClass* colorShader3D_;
		Shader3DClass* diffuseShader3D_;
		Shader3DClass* multitextureShader3D_;
};

#endif
