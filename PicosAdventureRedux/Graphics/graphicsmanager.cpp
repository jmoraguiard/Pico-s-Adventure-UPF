#include "graphicsManager.h"

GraphicsManager::GraphicsManager()
{
	swapChain_ = 0;
	d3dDevice_ = 0;
	d3dDeviceContext_ = 0;

	renderTargetView_ = 0;

	depthStencilBuffer_ = 0;
	depthStencilState_ = 0;
	depthStencilView_ = 0;

	solidRasterizerState_ = 0;
	wireframeRasterizerState_ = 0;

	alphaEnableBlendingState_ = 0;
	alphaDisableBlendingState_ = 0;

	shader2D_ = 0;
	colorShader3D_ = 0;
	diffuseShader3D_ = 0;
	multitextureShader3D_ = 0;
}

GraphicsManager::GraphicsManager(const GraphicsManager& other)
{

}

GraphicsManager::~GraphicsManager()
{

}

bool GraphicsManager::setup(HWND windowHandler, bool vsync, bool fullscreen, float screenDepthPlane, float screenNearPlane)
{
	windowHandler_ = windowHandler;

	//Rectangle of the active region of the screen window
	RECT dimensions; 
    GetClientRect( windowHandler, &dimensions );

    screenWidth_ = dimensions.right - dimensions.left;
    screenHeight_ = dimensions.bottom - dimensions.top;

	HRESULT result;

	//All the driver types we want to try to set when initializing i preference order
    D3D_DRIVER_TYPE driverTypes[] = 
    {
		D3D_DRIVER_TYPE_HARDWARE, D3D_DRIVER_TYPE_WARP, D3D_DRIVER_TYPE_REFERENCE
    };

    unsigned int totalDriverTypes = ARRAYSIZE( driverTypes );

	//Feature levels we may accept in order of preference
    D3D_FEATURE_LEVEL featureLevels[] = 
    {
        D3D_FEATURE_LEVEL_11_0,
        D3D_FEATURE_LEVEL_10_1,
        D3D_FEATURE_LEVEL_10_0,
		D3D_FEATURE_LEVEL_9_3
    };

    unsigned int totalFeatureLevels = ARRAYSIZE( featureLevels );

    DXGI_SWAP_CHAIN_DESC swapChainDesc; //Creating the descriptor for the swap chain, collection of rendering destinations
    ZeroMemory( &swapChainDesc, sizeof( swapChainDesc ) );
    swapChainDesc.BufferCount = 1; //A value that describes the number of buffers in the swap chain. 
    swapChainDesc.BufferDesc.Width = screenWidth_; //Resolution width
    swapChainDesc.BufferDesc.Height = screenHeight_; //Resolution height
    swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM; //Format of the buffer in 32bit per pixel with alpha channel
    swapChainDesc.BufferDesc.RefreshRate.Numerator = 0; //The refresh rate for the change of buffer
    swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
    swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT; //This allows the swap chain to be used as rendering output
	swapChainDesc.OutputWindow = windowHandler;
    swapChainDesc.Windowed = true; //Specify if the application works in windowed mode
	swapChainDesc.SampleDesc.Count = 1; 
	swapChainDesc.SampleDesc.Quality = 0;
	swapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	swapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

	unsigned int creationFlags = D3D11_CREATE_DEVICE_BGRA_SUPPORT | D3D11_CREATE_DEVICE_SINGLETHREADED; //Create a bitwise ORed list of flags to activate when creating the device and context

/*#ifdef _DEBUG
    creationFlags |= D3D11_CREATE_DEVICE_DEBUG; //If in debug mode we set the runtime layer to debug.
#endif*/

    unsigned int driver = 0;

    for(driver = 0; driver < totalDriverTypes; ++driver) //For each one of the possible specified objective drivers we try to create the device, context and swap chain
    {
        result = D3D11CreateDeviceAndSwapChain(0, //A pointer to the adapter to use. Pass NULL to use the default , the first adapter enumerated by IDXGIFactory1::EnumAdapters.
											   driverTypes[driver], //The driver type to create.
											   0, //A handle to a DLL if we were specifying a propietary software driver.
											   creationFlags, //The runtime layers to enable.
                                               featureLevels, //An array with the feature levels we will atempt to create in order.
											   totalFeatureLevels, //How many feature levels we will try to create.
                                               D3D11_SDK_VERSION, //The SDK version; use D3D11_SDK_VERSION.
											   &swapChainDesc, //A reference to the swap chain descriptor
											   &swapChain_, //Returns the address of a pointer to the IDXGISwapChain object that represents the swap chain used for rendering.
                                               &d3dDevice_, //Returns the address of a pointer to an ID3D11Device object that represents the device created.
											   &featureLevel_, //Returns a pointer to a D3D_FEATURE_LEVEL, which represents the first element of feature levels supported.
											   &d3dDeviceContext_ ); //Returns the address of a pointer to an ID3D11DeviceContext object that represents the device context.

        if(SUCCEEDED(result))
        {
            driverType_ = driverTypes[driver]; //If we succeed we save the driver type and exit the loop for creating the device, context and swap chain.
            break;
        }
    }

    if(FAILED( result)) //If creation given the feature levels and driver types fail for all of them, we quit with a false message.
    {
		MessageBox(NULL, L"Failed to create the Direct3D device.", L"GraphicsManager - Error", MB_ICONERROR | MB_OK);
        return false;
    }

	// Creating the Render Target View
    ID3D11Texture2D* backBufferTexture;

	// We obtain a pointer to the swap chain's back buffer
	result = swapChain_->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&backBufferTexture));

    if(FAILED(result))
    {
		MessageBox(NULL, L"Failed to get the swap chain back buffer.", L"GraphicsManager - Error", MB_ICONERROR | MB_OK);
        return false;
    }

	// We create a view to access the pipeline resource because DirectX does not allow to bind
	// resources directly to the pipeline, we need to access through the views
    result = d3dDevice_->CreateRenderTargetView(backBufferTexture, 0, &renderTargetView_);

    if(backBufferTexture)
        backBufferTexture->Release();

    if(FAILED(result))
    {
		MessageBox(NULL, L"Failed to create the render target view.", L"GraphicsManager - Error", MB_ICONERROR | MB_OK);
        return false;
    }

	// Initialize the description of the depth buffer.
	D3D11_TEXTURE2D_DESC depthBufferDescription;
	ZeroMemory(&depthBufferDescription, sizeof(depthBufferDescription));

	// Set up the description of the depth buffer.
	depthBufferDescription.Width = screenWidth_;
	depthBufferDescription.Height = screenHeight_;
	depthBufferDescription.MipLevels = 1;
	depthBufferDescription.ArraySize = 1;
	depthBufferDescription.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthBufferDescription.SampleDesc.Count = 1;
	depthBufferDescription.SampleDesc.Quality = 0;
	depthBufferDescription.Usage = D3D11_USAGE_DEFAULT;
	depthBufferDescription.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	depthBufferDescription.CPUAccessFlags = 0;
	depthBufferDescription.MiscFlags = 0;

	// Create the texture for the depth buffer using the filled out description.
	result = d3dDevice_->CreateTexture2D(&depthBufferDescription, NULL, &depthStencilBuffer_);
	if(FAILED(result))
	{
		return false;
	}

	// Initialize the description of the stencil state.
	D3D11_DEPTH_STENCIL_DESC depthStencilDescription;
	ZeroMemory(&depthStencilDescription, sizeof(depthStencilDescription));

	// Set up the description of the stencil state.
	depthStencilDescription.DepthEnable = true;
	depthStencilDescription.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	depthStencilDescription.DepthFunc = D3D11_COMPARISON_LESS;

	depthStencilDescription.StencilEnable = true;
	depthStencilDescription.StencilReadMask = 0xFF;
	depthStencilDescription.StencilWriteMask = 0xFF;

	// Stencil operations if pixel is front-facing.
	depthStencilDescription.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDescription.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
	depthStencilDescription.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDescription.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	// Stencil operations if pixel is back-facing.
	depthStencilDescription.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDescription.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
	depthStencilDescription.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDescription.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	// Create the depth stencil state.
	result = d3dDevice_->CreateDepthStencilState(&depthStencilDescription, &depthStencilState_);
	if(FAILED(result))
	{
		return false;
	}

	// Set the depth stencil state.
	d3dDeviceContext_->OMSetDepthStencilState(depthStencilState_, 1);

	// Initailze the depth stencil view.
	D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDescription;
	ZeroMemory(&depthStencilViewDescription, sizeof(depthStencilViewDescription));

	// Set up the depth stencil view description.
	depthStencilViewDescription.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthStencilViewDescription.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	depthStencilViewDescription.Texture2D.MipSlice = 0;

	// Create the depth stencil view.
	result = d3dDevice_->CreateDepthStencilView(depthStencilBuffer_, &depthStencilViewDescription, &depthStencilView_);
	if(FAILED(result))
	{
		return false;
	}

	// Initailze the depth stencil view.
	ZeroMemory(&depthStencilViewDescription, sizeof(depthStencilViewDescription));

	// Set up the depth stencil view description.
	depthStencilViewDescription.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthStencilViewDescription.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	depthStencilViewDescription.Texture2D.MipSlice = 0;

	// Create the depth stencil view.
	result = d3dDevice_->CreateDepthStencilView(depthStencilBuffer_, &depthStencilViewDescription, &depthStencilView_);
	if(FAILED(result))
	{
		return false;
	}

	// Clear the second depth stencil state before setting the parameters.
	D3D11_DEPTH_STENCIL_DESC depthDisabledStencilDesc;
	ZeroMemory(&depthDisabledStencilDesc, sizeof(depthDisabledStencilDesc));

	// Now create a second depth stencil state which turns off the Z buffer for 2D rendering.  The only difference is 
	// that DepthEnable is set to false, all other parameters are the same as the other depth stencil state.
	depthDisabledStencilDesc.DepthEnable = false;
	depthDisabledStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	depthDisabledStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;
	depthDisabledStencilDesc.StencilEnable = true;
	depthDisabledStencilDesc.StencilReadMask = 0xFF;
	depthDisabledStencilDesc.StencilWriteMask = 0xFF;
	depthDisabledStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthDisabledStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
	depthDisabledStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthDisabledStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
	depthDisabledStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthDisabledStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
	depthDisabledStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthDisabledStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	// Create the state using the device.
	result = d3dDevice_->CreateDepthStencilState(&depthDisabledStencilDesc, &depthDisabledStencilState_);
	if(FAILED(result))
	{
		return false;
	}

	// Clear the blend state description.
	D3D11_BLEND_DESC blendStateDescription;
	ZeroMemory(&blendStateDescription, sizeof(D3D11_BLEND_DESC));

	// Create an alpha enabled blend state description.
	blendStateDescription.RenderTarget[0].BlendEnable = TRUE;
	blendStateDescription.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;
	blendStateDescription.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	blendStateDescription.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	blendStateDescription.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	blendStateDescription.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	blendStateDescription.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	blendStateDescription.RenderTarget[0].RenderTargetWriteMask = 0x0f;

	// Create the blend state using the description.
	result = d3dDevice_->CreateBlendState(&blendStateDescription, &alphaEnableBlendingState_);
	if(FAILED(result))
	{
		return false;
	}

	// Modify blend for the particles
	blendStateDescription.RenderTarget[0].DestBlend = D3D11_BLEND_ONE;//D3D11_BLEND_INV_SRC_ALPHA;

	// Create the blend state using the description.
	result = d3dDevice_->CreateBlendState(&blendStateDescription, &alphaEnableParticlesBlendingState_);
	if(FAILED(result))
	{
		return false;
	}

	// Modify the description to create an alpha disabled blend state description.
	blendStateDescription.RenderTarget[0].BlendEnable = FALSE;

	// Create the blend state using the description.
	result = d3dDevice_->CreateBlendState(&blendStateDescription, &alphaDisableBlendingState_);
	if(FAILED(result))
	{
		return false;
	}

	// Bind the render target view and depth stencil buffer to the output render pipeline.
	d3dDeviceContext_->OMSetRenderTargets(1, &renderTargetView_, depthStencilView_);

	// Clear the raster description.
	D3D11_RASTERIZER_DESC rasterDescription;
	ZeroMemory(&rasterDescription, sizeof(D3D11_RASTERIZER_DESC));

	// Setup the raster description which will determine how and what polygons will be drawn.
	rasterDescription.AntialiasedLineEnable = false;
	rasterDescription.CullMode = D3D11_CULL_BACK;
	rasterDescription.DepthBias = 0;
	rasterDescription.DepthBiasClamp = 0.0f;
	rasterDescription.DepthClipEnable = true;
	rasterDescription.FillMode = D3D11_FILL_SOLID;
	rasterDescription.FrontCounterClockwise = false;
	rasterDescription.MultisampleEnable = false;
	rasterDescription.ScissorEnable = false;
	rasterDescription.SlopeScaledDepthBias = 0.0f;

	// Create the rasterizer state from the description we just filled out.
	result = d3dDevice_->CreateRasterizerState(&rasterDescription, &solidRasterizerState_);
	if(FAILED(result))
	{
		return false;
	}

	// Now set the rasterizer state.
	d3dDeviceContext_->RSSetState(solidRasterizerState_);

	// Clear the raster description.
	ZeroMemory(&rasterDescription, sizeof(D3D11_RASTERIZER_DESC));

	// Setup the raster description which will determine how and what polygons will be drawn.
	rasterDescription.AntialiasedLineEnable = false;
	rasterDescription.CullMode = D3D11_CULL_BACK;
	rasterDescription.DepthBias = 0;
	rasterDescription.DepthBiasClamp = 0.0f;
	rasterDescription.DepthClipEnable = true;
	rasterDescription.FillMode = D3D11_FILL_WIREFRAME;
	rasterDescription.FrontCounterClockwise = false;
	rasterDescription.MultisampleEnable = false;
	rasterDescription.ScissorEnable = false;
	rasterDescription.SlopeScaledDepthBias = 0.0f;

	// Create the rasterizer state from the description we just filled out.
	result = d3dDevice_->CreateRasterizerState(&rasterDescription, &wireframeRasterizerState_);
	if(FAILED(result))
	{
		return false;
	}

	// Bind the render target view and depth stencil buffer to the output render pipeline.
	d3dDeviceContext_->OMSetRenderTargets(1, &renderTargetView_, depthStencilView_);

	//Creating the viewport
    D3D11_VIEWPORT viewport;
	viewport.Width = static_cast<float>(screenWidth_);
	viewport.Height = static_cast<float>(screenHeight_);
    viewport.MinDepth = 0.0f;
    viewport.MaxDepth = 1.0f;
    viewport.TopLeftX = 0.0f;
    viewport.TopLeftY = 0.0f;

    d3dDeviceContext_->RSSetViewports(1, &viewport);

	// Setup the projection matrix.
	float fieldOfView = (float)XM_PIDIV4;
	float screenAspect = (float)screenWidth_ / (float)screenHeight_;

	// Create the projection matrix for 3D rendering.
	XMStoreFloat4x4(&projectionMatrix_, XMMatrixPerspectiveFovLH(fieldOfView, screenAspect, screenNearPlane, screenDepthPlane));

	// Initialize the world matrix to the identity matrix.
	XMStoreFloat4x4(&worldMatrix_, XMMatrixIdentity());

	// Create an orthographic projection matrix for 2D rendering.
	XMStoreFloat4x4(&orthoMatrix_, XMMatrixOrthographicLH((float)screenWidth_, (float)screenHeight_, screenNearPlane, screenDepthPlane));

	// We call the function to setup the shaders
	if(!setupShaders())
	{
		MessageBoxA(windowHandler, "Could not setup shaders.", "Error", MB_ICONERROR | MB_OK);
		return false;
	}

	return true;
}

void GraphicsManager::destroy()
{
	// Release the 2D shader object.
	if(shader2D_)
	{
		shader2D_->destroy();
		delete shader2D_;
		shader2D_ = 0;
	}

	// Release the 3D shader object.
	if(colorShader3D_)
	{
		colorShader3D_->destroy();
		delete colorShader3D_;
		colorShader3D_ = 0;
	}

	if(diffuseShader3D_)
	{
		diffuseShader3D_->destroy();
		delete diffuseShader3D_;
		diffuseShader3D_ = 0;
	}

	if(multitextureShader3D_)
	{
		multitextureShader3D_->destroy();
		delete multitextureShader3D_;
		multitextureShader3D_ = 0;
	}

	// Before shutting down set to windowed mode or when you release the swap chain it will throw an exception.
	if(depthStencilView_)
	{
		depthStencilView_->Release();
		depthStencilView_ = 0;
	}

	if(depthStencilState_)
	{
		depthStencilState_->Release();
		depthStencilState_ = 0;
	}

	if(depthStencilBuffer_)
	{
		depthStencilBuffer_->Release();
		depthStencilBuffer_ = 0;
	}

	if(swapChain_)
	{
		swapChain_->SetFullscreenState(false, NULL);
	}

	if(renderTargetView_)
	{
		renderTargetView_->Release();
		renderTargetView_ = 0;
	}

	if(d3dDeviceContext_)
	{
		d3dDeviceContext_->Release();
		d3dDeviceContext_ = 0;
	}

	if(d3dDevice_)
	{
		d3dDevice_->Release();
		d3dDevice_ = 0;
	}

	if(swapChain_)
	{
		swapChain_->Release();
		swapChain_ = 0;
	}

	return;
}

void GraphicsManager::beginDraw(float red, float green, float blue, float alpha)
{
	// Setup the color to clear the buffer to.
	float color[] = {red, green, blue, alpha};

	// Clear the back buffer.
	d3dDeviceContext_->ClearRenderTargetView(renderTargetView_, color);

	// Clear the depth buffer.
	d3dDeviceContext_->ClearDepthStencilView(depthStencilView_, D3D11_CLEAR_DEPTH, 1.0f, 0);

	return;
}

void GraphicsManager::endDraw()
{
	// Present the back buffer to the screen since rendering is complete.
	if(vsyncEnabled_)
	{
		// Lock to screen refresh rate.
		swapChain_->Present(1, 0);
	}
	else
	{
		// Present as fast as possible.
		swapChain_->Present(0, 0);
	}

	return;
}

bool GraphicsManager::setupShaders()
{
	// Create the 2D shader object.
	shader2D_ = new Shader2DClass;
	if(!shader2D_)
	{
		return false;
	}

	// Initialize the color shader object.
	if(!shader2D_->setup(d3dDevice_))
	{
		MessageBoxA(NULL, "Could not initialize the 2D shader object.", "Error", MB_ICONERROR | MB_OK);
		return false;
	}

	// Create the 3D shader object.
	colorShader3D_ = Shader3DFactory::Instance()->CreateShader3D("ColorShader3D", this);
	diffuseShader3D_ = Shader3DFactory::Instance()->CreateShader3D("DiffuseShader3D", this);
	multitextureShader3D_ = Shader3DFactory::Instance()->CreateShader3D("MultiTextureShader3D", this);

	return true;
}

ID3D11Device* GraphicsManager::getDevice()
{
	return d3dDevice_;
}

ID3D11DeviceContext* GraphicsManager::getDeviceContext()
{
	return d3dDeviceContext_;
}

IDXGISwapChain* GraphicsManager::getSwapChain()
{
	return swapChain_;
}

Shader2DClass* GraphicsManager::getShader2D()
{
	return shader2D_;
}

Shader3DClass* GraphicsManager::getShader3D(std::string type)
{
	return Shader3DFactory::Instance()->CreateShader3D(type, this);;
}

Shader3DClass* GraphicsManager::getColorShader3D()
{
	return colorShader3D_;
}

Shader3DClass* GraphicsManager::getDiffuseShader3D()
{
	return diffuseShader3D_;
}

Shader3DClass* GraphicsManager::getMultitextureShader3D()
{
	return multitextureShader3D_;
}

void GraphicsManager::getProjectionMatrix(XMFLOAT4X4& projectionMatrix)
{
	projectionMatrix = projectionMatrix_;
	return;
}

void GraphicsManager::getWorldMatrix(XMFLOAT4X4& worldMatrix)
{
	worldMatrix = worldMatrix_;
	return;
}

void GraphicsManager::getOrthoMatrix(XMFLOAT4X4& orthoMatrix)
{
	orthoMatrix = orthoMatrix_;
	return;
}

void GraphicsManager::getScreenSize(int& width, int& height)
{
	width = screenWidth_;
	height = screenHeight_;
	return;
}

HWND GraphicsManager::getWindowHandler()
{
	return windowHandler_;
}

void GraphicsManager::turnOnSolidRasterizer()
{
	d3dDeviceContext_->RSSetState(solidRasterizerState_);
}

void GraphicsManager::turnOnWireframeRasterizer()
{
	d3dDeviceContext_->RSSetState(wireframeRasterizerState_);
}

void GraphicsManager::turnZBufferOn()
{
	d3dDeviceContext_->OMSetDepthStencilState(depthStencilState_, 1);
	return;
}


void GraphicsManager::turnZBufferOff()
{
	d3dDeviceContext_->OMSetDepthStencilState(depthDisabledStencilState_, 1);
	return;
}

void GraphicsManager::turnOnAlphaBlending()
{
	float blendFactor[4];
	

	// Setup the blend factor.
	blendFactor[0] = 0.0f;
	blendFactor[1] = 0.0f;
	blendFactor[2] = 0.0f;
	blendFactor[3] = 0.0f;
	
	// Turn on the alpha blending.
	d3dDeviceContext_->OMSetBlendState(alphaEnableBlendingState_, blendFactor, 0xffffffff);

	return;
}

void GraphicsManager::turnOnParticlesAlphaBlending()
{
	float blendFactor[4];
	

	// Setup the blend factor.
	blendFactor[0] = 0.0f;
	blendFactor[1] = 0.0f;
	blendFactor[2] = 0.0f;
	blendFactor[3] = 0.0f;
	
	// Turn on the alpha blending.
	d3dDeviceContext_->OMSetBlendState(alphaEnableParticlesBlendingState_, blendFactor, 0xffffffff);

	return;
}

void GraphicsManager::turnOffAlphaBlending()
{
	float blendFactor[4];
	

	// Setup the blend factor.
	blendFactor[0] = 0.0f;
	blendFactor[1] = 0.0f;
	blendFactor[2] = 0.0f;
	blendFactor[3] = 0.0f;
	
	// Turn off the alpha blending.
	d3dDeviceContext_->OMSetBlendState(alphaDisableBlendingState_, blendFactor, 0xffffffff);

	return;
}
