#include "kinectClass.h"

#include "applicationManager.h"

KinectClass::KinectClass()
{
	nextDepthFrameEvent_ = INVALID_HANDLE_VALUE;
    nextColorFrameEvent_ = INVALID_HANDLE_VALUE;
    nextSkeletonFrameEvent_ = INVALID_HANDLE_VALUE;
    nextBackgroundRemovedFrameEvent_ = INVALID_HANDLE_VALUE;
	D2DFactory_ = 0;
    depthStreamHandle_ = INVALID_HANDLE_VALUE;
    colorStreamHandle_ = INVALID_HANDLE_VALUE;
    nearMode_ = false;
    nuiSensor_ = 0;
    backgroundRemovalStream_[0] = 0;
	backgroundRemovalStream_[1] = 0;
    trackedSkeleton_[0] = NUI_SKELETON_INVALID_TRACKING_ID;
	trackedSkeleton_[1] = NUI_SKELETON_INVALID_TRACKING_ID;

	D2DFactory_ = 0;
    renderTarget_ = 0;
    bitmap_[0] = 0;
	bitmap_[1] = 0;

	drawUser1_ = true;

	risedHand = false;
	countSwipe = 0;
	lastHandPosition = 1;
	handPosition = 1;
	handsDist = 100;
	lastNumPlayer= 0;
	numPlayer=0;
}


KinectClass::KinectClass(const KinectClass& other)
{

}


KinectClass::~KinectClass()
{

}

bool KinectClass::setup(HWND windowHandler, IDXGISwapChain* swapChain)
{
	DWORD width = 0;
	DWORD height = 0;

	NuiImageResolutionToSize(NUI_IMAGE_RESOLUTION_320x240, width, height);
	depthWidth_  = static_cast<LONG>(width);
	depthHeight_ = static_cast<LONG>(height);

	NuiImageResolutionToSize(NUI_IMAGE_RESOLUTION_640x480, width, height);
	colorWidth_  = static_cast<LONG>(width);
	colorHeight_ = static_cast<LONG>(height);

	// create heap storage for depth pixel data in RGBX format
	outputRGBX_ = new BYTE[colorWidth_ * colorHeight_ * 4];
	backgroundRGBX_ = new BYTE[colorWidth_ * colorHeight_ * 4];

	// Create an event that will be signaled when depth data is available
	nextDepthFrameEvent_ = CreateEvent(NULL, TRUE, FALSE, NULL);

	// Create an event that will be signaled when color data is available
	nextColorFrameEvent_ = CreateEvent(NULL, TRUE, FALSE, NULL);

	// Create an event that will be signaled when skeleton frame is available
	nextSkeletonFrameEvent_ = CreateEvent(NULL, TRUE, FALSE, NULL);

	// Create an event that will be signaled when the segmentation frame is ready
	nextBackgroundRemovedFrameEvent_ = CreateEvent(NULL, TRUE, FALSE, NULL);

	hEvents[0] = nextDepthFrameEvent_;
	hEvents[1] = nextColorFrameEvent_;
	hEvents[2] = nextSkeletonFrameEvent_;
	hEvents[3] = nextBackgroundRemovedFrameEvent_;

	HRESULT hr;
    // Bind application window handle
    hWnd_ = windowHandler;

    // Create NuiSensorChooser UI control
    RECT rc;
    GetClientRect(hWnd_, &rc);

    POINT ptCenterTop;
    ptCenterTop.x = (rc.right - rc.left)/2;
    ptCenterTop.y = 0;

    INuiSensor * nuiSensor;

    int iSensorCount = 0;
    hr = NuiGetSensorCount(&iSensorCount);
    if(FAILED(hr))
    {
        return false;
    }

    // Look at each Kinect sensor
    for (int i = 0; i < iSensorCount; ++i)
    {
        // Create the sensor so we can check status, if we can't create it, move on to the next
        hr = NuiCreateSensorByIndex(i, &nuiSensor);
        if (FAILED(hr))
        {
            continue;
        }

        // Get the status of the sensor, and if connected, then we can initialize it
        hr = nuiSensor->NuiStatus();
        if (S_OK == hr)
        {
            nuiSensor_ = nuiSensor;
            break;
        }

        // This sensor wasn't OK, so release it since we're not using it
        nuiSensor->Release();
    }

    if (nuiSensor_ != 0)
    {
        // Initialize the Kinect and specify that we'll be using skeleton
		hr = nuiSensor_->NuiInitialize(NUI_INITIALIZE_FLAG_USES_DEPTH_AND_PLAYER_INDEX|NUI_INITIALIZE_FLAG_USES_COLOR|NUI_INITIALIZE_FLAG_USES_SKELETON); 
        if (SUCCEEDED(hr))
        {
			// Open a depth stream to receive depth data
			hr = nuiSensor_->NuiImageStreamOpen(NUI_IMAGE_TYPE_DEPTH_AND_PLAYER_INDEX,
											    NUI_IMAGE_RESOLUTION_320x240,
												0,
												2,
												nextDepthFrameEvent_,
												&depthStreamHandle_);

			if (FAILED(hr))
			{
				MessageBox(NULL, L"Could not open depth tracking!", L"Error", MB_ICONERROR | MB_OK);
				return false;
			}

			// Open a color stream to receive color image data
			hr = nuiSensor_->NuiImageStreamOpen(NUI_IMAGE_TYPE_COLOR,
												NUI_IMAGE_RESOLUTION_640x480,
												0,
												2,
												nextColorFrameEvent_,
												&colorStreamHandle_);

			if (FAILED(hr))
			{
				MessageBox(NULL, L"Could not open color tracking!", L"Error", MB_ICONERROR | MB_OK);
				return false;
			}
			
            // Open a skeleton stream to receive skeleton data
            hr = nuiSensor_->NuiSkeletonTrackingEnable(nextSkeletonFrameEvent_, NUI_SKELETON_TRACKING_FLAG_ENABLE_IN_NEAR_RANGE); 

			if (FAILED(hr))
			{
				MessageBox(NULL, L"Could not open skeleton tracking!", L"Error", MB_ICONERROR | MB_OK);
				return false;
			}

			hr = NuiCreateBackgroundRemovedColorStream(nuiSensor_, &backgroundRemovalStream_[0]);
			if (FAILED(hr))
			{
				return false;
			}

			hr = backgroundRemovalStream_[0]->Enable(NUI_IMAGE_RESOLUTION_640x480, NUI_IMAGE_RESOLUTION_320x240, nextBackgroundRemovedFrameEvent_);

			hr = NuiCreateBackgroundRemovedColorStream(nuiSensor_, &backgroundRemovalStream_[1]);
			if (FAILED(hr))
			{
				return false;
			}

			hr = backgroundRemovalStream_[1]->Enable(NUI_IMAGE_RESOLUTION_640x480, NUI_IMAGE_RESOLUTION_320x240, nextBackgroundRemovedFrameEvent_);

        }
    }
	else
    {
        MessageBox(NULL, L"No ready Kinect found!", L"Error", MB_ICONERROR | MB_OK);
        return false;
    }

	D2D1_FACTORY_OPTIONS options;
	options.debugLevel = D2D1_DEBUG_LEVEL_INFORMATION;
	D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, options, &D2DFactory_);
	
    D2D1_SIZE_U size = D2D1::SizeU(colorWidth_, colorHeight_);

	IDXGISurface *dxgiBackbuffer;
	swapChain->GetBuffer(0, IID_PPV_ARGS(&dxgiBackbuffer));
	D2D1_RENDER_TARGET_PROPERTIES props = D2D1::RenderTargetProperties(D2D1_RENDER_TARGET_TYPE_HARDWARE,
																       D2D1::PixelFormat(DXGI_FORMAT_R8G8B8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED));
	props.usage = D2D1_RENDER_TARGET_USAGE_FORCE_BITMAP_REMOTING;
	D2DFactory_->CreateDxgiSurfaceRenderTarget(dxgiBackbuffer, props, &renderTarget_);
	dxgiBackbuffer->Release();
	dxgiBackbuffer = 0;

    if(FAILED(hr))
    {
		MessageBox(NULL, L"Could not create render target.", L"Draw Error", MB_OK);
        return false;
    }

    // Create a bitmap that we can copy image data into and then render to the target
    hr = renderTarget_->CreateBitmap(size, 
									 D2D1::BitmapProperties(D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED)),
									 &bitmap_[0]);
	hr = renderTarget_->CreateBitmap(size, 
									 D2D1::BitmapProperties(D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED)),
									 &bitmap_[1]);

    if(FAILED(hr))
    {
		if(renderTarget_ != NULL)
		{
			renderTarget_->Release();
			renderTarget_ = NULL;
		}
		MessageBox(NULL, L"Could not create bitmap.", L"Draw Error", MB_OK);
        return false;
    }

	userColor_  = XMFLOAT4(1.0f, 1.0f, 1.0f, 0.5f);

	return true;
}

void KinectClass::update()
{
	if(nuiSensor_ != 0)
    {
		MsgWaitForMultipleObjects(_countof(hEvents), hEvents, FALSE, INFINITE, QS_ALLINPUT);
    }
	else
	{
		return;
	}

    if(WAIT_OBJECT_0 == WaitForSingleObject(nextBackgroundRemovedFrameEvent_, 0))
    {
        ComposeImage();
    }

    if( WAIT_OBJECT_0 == WaitForSingleObject(nextDepthFrameEvent_, 0) )
    {
        ProcessDepth();
    }

    if( WAIT_OBJECT_0 == WaitForSingleObject(nextColorFrameEvent_, 0) )
    {
        ProcessColor();
    }

    if(WAIT_OBJECT_0 == WaitForSingleObject(nextSkeletonFrameEvent_, 0) )
    {
        ProcessSkeleton();
    }
}

void KinectClass::draw()
{
	HRESULT hr = S_OK;
       
    renderTarget_->BeginDraw();

	//RECTANGLE SCREEN SIZE
	D2D1_SIZE_F screenSize = renderTarget_->GetSize();
	D2D1_RECT_F rec = D2D1::RectF(0, 0, screenSize.width, screenSize.height);

    // Draw the bitmap stretched to the size of the window
    renderTarget_->DrawBitmap(bitmap_[0], rec);
	renderTarget_->DrawBitmap(bitmap_[1], rec);

	//renderTarget_->DrawEllipse();
            
    hr = renderTarget_->EndDraw();
}

void KinectClass::destroy()
{
	// clean up arrays
    delete[] outputRGBX_;
    delete[] backgroundRGBX_;

	if(nextBackgroundRemovedFrameEvent_ != INVALID_HANDLE_VALUE)
    {
        CloseHandle(nextBackgroundRemovedFrameEvent_);
    }

	if(nextColorFrameEvent_ != INVALID_HANDLE_VALUE)
    {
        CloseHandle(nextColorFrameEvent_);
    }

	if(nextDepthFrameEvent_ != INVALID_HANDLE_VALUE)
    {
        CloseHandle(nextDepthFrameEvent_);
    }

	if(nextSkeletonFrameEvent_ != INVALID_HANDLE_VALUE)
    {
        CloseHandle(nextSkeletonFrameEvent_);
    }

	if(bitmap_[1] != 0)
    {
        bitmap_[1]->Release();
        bitmap_[1] = 0;
    }

	if(bitmap_[0] != 0)
    {
        bitmap_[0]->Release();
        bitmap_[0] = 0;
    }

	if(renderTarget_ != 0)
	{
		renderTarget_->Release();
		renderTarget_ = 0;
	}

	if(D2DFactory_ != 0)
    {
        D2DFactory_->Release();
        D2DFactory_ = 0;
    }

	if(backgroundRemovalStream_[1] != 0)
    {
        backgroundRemovalStream_[1]->Release();
        backgroundRemovalStream_[1] = 0;
    }

	if(backgroundRemovalStream_[0] != 0)
    {
        backgroundRemovalStream_[0]->Release();
        backgroundRemovalStream_[0] = 0;
    }

	if(nuiSensor_ != 0)
    {
		nuiSensor_->NuiShutdown();
        nuiSensor_->Release();
        nuiSensor_ = 0;
    }

}

void KinectClass::setUserColor(XMFLOAT4 color)
{
	userColor_ = color;
}

HRESULT KinectClass::ProcessDepth()
{
    HRESULT hr;
	HRESULT bghr = S_OK;
    NUI_IMAGE_FRAME imageFrame;

    // Attempt to get the depth frame
    LARGE_INTEGER depthTimeStamp;
    hr = nuiSensor_->NuiImageStreamGetNextFrame(depthStreamHandle_, 0, &imageFrame);
    if(FAILED(hr))
    {
        return hr;
    }
    depthTimeStamp = imageFrame.liTimeStamp;
    INuiFrameTexture* pTexture;

    // Attempt to get the extended depth texture
    hr = nuiSensor_->NuiImageFrameGetDepthImagePixelFrameTexture(depthStreamHandle_, &imageFrame, &nearMode_, &pTexture);
    if(FAILED(hr))
    {
        return hr;
    }
    NUI_LOCKED_RECT LockedRect;

    // Lock the frame data so the Kinect knows not to modify it while we're reading it
    pTexture->LockRect(0, &LockedRect, NULL, 0);

    // Make sure we've received valid data, and then present it to the background removed color stream. 
	if(LockedRect.Pitch != 0)
	{
		bghr = backgroundRemovalStream_[0]->ProcessDepth(depthWidth_ * depthHeight_ * 4, LockedRect.pBits, depthTimeStamp);
		bghr = backgroundRemovalStream_[1]->ProcessDepth(depthWidth_ * depthHeight_ * 4, LockedRect.pBits, depthTimeStamp);
	}

    // We're done with the texture so unlock it. Even if above process failed, we still need to unlock and release.
    pTexture->UnlockRect(0);
    pTexture->Release();

    // Release the frame
    hr = nuiSensor_->NuiImageStreamReleaseFrame(depthStreamHandle_, &imageFrame);

	if(FAILED(bghr))
	{
		return bghr;
	}

    return hr;
}

HRESULT KinectClass::ProcessColor()
{
    HRESULT hr;
	HRESULT bghr = S_OK;

    NUI_IMAGE_FRAME imageFrame;

    // Attempt to get the depth frame
    LARGE_INTEGER colorTimeStamp;
    hr = nuiSensor_->NuiImageStreamGetNextFrame(colorStreamHandle_, 0, &imageFrame);
    if(FAILED(hr))
    {
        return hr;
    }
    colorTimeStamp = imageFrame.liTimeStamp;

    INuiFrameTexture * pTexture = imageFrame.pFrameTexture;
    NUI_LOCKED_RECT LockedRect;

    // Lock the frame data so the Kinect knows not to modify it while we're reading it
    pTexture->LockRect(0, &LockedRect, NULL, 0);

	// Make sure we've received valid data. Then save a copy of color frame.
	if(LockedRect.Pitch != 0)
	{
		bghr = backgroundRemovalStream_[0]->ProcessColor(colorWidth_ * colorHeight_ * 4, LockedRect.pBits, colorTimeStamp);
		bghr = backgroundRemovalStream_[1]->ProcessColor(colorWidth_ * colorHeight_ * 4, LockedRect.pBits, colorTimeStamp);
    }

    // We're done with the texture so unlock it
    pTexture->UnlockRect(0);

    // Release the frame
    hr = nuiSensor_->NuiImageStreamReleaseFrame(colorStreamHandle_, &imageFrame);

	if(FAILED(bghr))
	{
		return bghr;
	}

    return hr;
}

HRESULT KinectClass::ProcessSkeleton()
{
    HRESULT hr;

	NUI_SKELETON_FRAME skeletonFrame;
    hr = nuiSensor_->NuiSkeletonGetNextFrame(0, &skeletonFrame);
    if(FAILED(hr))
    {
        return hr;
    }

	NUI_SKELETON_DATA* pSkeletonData = skeletonFrame.SkeletonData;
    // Background Removal Stream requires us to specifically tell it what skeleton ID to use as the foreground
	hr = ChooseSkeleton(pSkeletonData);
	if(FAILED(hr))
    {
        return hr;
    }

    hr = backgroundRemovalStream_[0]->ProcessSkeleton(NUI_SKELETON_COUNT, pSkeletonData, skeletonFrame.liTimeStamp);
	hr = backgroundRemovalStream_[1]->ProcessSkeleton(NUI_SKELETON_COUNT, pSkeletonData, skeletonFrame.liTimeStamp);

	detectSekeltonsJoints(skeletonFrame);

	if(numPlayer<lastNumPlayer){
		KinectStruct kinectStruct = {SKELETON_LOST};
	}

	jointsToScreen();

	//There is one player
	if(numPlayer==1){
		KinectStruct kinectStruct = {FIRST_RIGHT_HAND_ROT, players[0].rightHandScreenCoord, players[0].rightElbowScreenCoord, Point(0,0,0), players[0].rightHandRot};
		notifyListeners(kinectStruct);

		KinectStruct kinectStruct2 = {FIRST_LEFT_HAND_ROT, players[0].leftHandScreenCoord, players[0].rightElbowScreenCoord, Point(0,0,0), players[0].rightHandRot};
		notifyListeners(kinectStruct2);

		LogClass::Instance()->addEntry("Player_1_Head_Pos", players[0].headScreenCoord.x, players[0].headScreenCoord.y);
		LogClass::Instance()->addEntry("Player_1_Torso_Pos", players[0].torsoScreenCoord.x, players[0].torsoScreenCoord.y);
		LogClass::Instance()->addEntry("Player_1_Left_Hand_Pos", players[0].leftHandScreenCoord.x, players[0].leftHandScreenCoord.y);
		LogClass::Instance()->addEntry("Player_1_Right_Hand_Pos", players[0].rightHandScreenCoord.x, players[0].rightHandScreenCoord.y);
		LogClass::Instance()->addEntry("Player_1_Left_Foot_Pos", players[0].leftFootScreenCoord.x, players[0].leftFootScreenCoord.y);
		LogClass::Instance()->addEntry("Player_1_Right_Foot_Pos", players[0].rightHandScreenCoord.x, players[0].rightHandScreenCoord.y);
	}

	//There are two players, compute distance between closest hands and define active region.
	if(numPlayer==2){
		if(players[0].hipCenter.x > players[1].hipCenter.x){

			Point leftHandPoint = Point(players[0].leftHand.x, players[0].leftHand.y, players[0].leftHand.z);
			Point rightHandPoint = Point(players[1].rightHand.x,players[1].rightHand.y,players[1].rightHand.z);

			handsDist =leftHandPoint.dist(rightHandPoint);
			
			KinectStruct kinectStruct = {HOLD_HANDS,players[0].leftHandScreenCoord,players[0].leftShoulderScreenCoord,players[1].rightShoulderScreenCoord,TRUE};
			//Check distance between hands 
			if (handsDist < 0.3)
			{
				kinectStruct.boolean = TRUE;
			}
			else 
			{
				kinectStruct.boolean = FALSE;
			}

			notifyListeners(kinectStruct);
		}
		else
		{
			Point leftHandPoint = Point( players[1].leftHand.x, players[1].leftHand.y, players[1].leftHand.z);
			Point rightHandPoint = Point(players[0].rightHand.x,players[0].rightHand.y,players[0].rightHand.z);

			handsDist = leftHandPoint.dist(rightHandPoint);

			KinectStruct kinectStruct = {HOLD_HANDS,players[0].rightHandScreenCoord,players[1].leftShoulderScreenCoord,players[0].rightShoulderScreenCoord,TRUE};
			if (handsDist < 0.1)
			{
				kinectStruct.boolean = TRUE;
			}
			else 
			{
				kinectStruct.boolean = FALSE;
			}

			notifyListeners(kinectStruct);
		}

		//Send both right hands
		KinectStruct kinectStruct1 ={FIRST_RIGHT_HAND_ROT, players[0].rightHandScreenCoord, players[0].rightElbowScreenCoord, Point(0,0,0), players[0].rightHandRot};
		notifyListeners(kinectStruct1);

		KinectStruct kinectStruct2 = {SECOND_RIGHT_HAND_ROT, players[1].rightHandScreenCoord, players[1].rightElbowScreenCoord, Point(0,0,0), players[1].rightHandRot};
		notifyListeners(kinectStruct2);

		LogClass::Instance()->addEntry("Player_1_Head_Pos", players[0].headScreenCoord.x, players[0].headScreenCoord.y);
		LogClass::Instance()->addEntry("Player_1_Torso_Pos", players[0].torsoScreenCoord.x, players[0].torsoScreenCoord.y);
		LogClass::Instance()->addEntry("Player_1_Left_Hand_Pos", players[0].leftHandScreenCoord.x, players[0].leftHandScreenCoord.y);
		LogClass::Instance()->addEntry("Player_1_Right_Hand_Pos", players[0].rightHandScreenCoord.x, players[0].rightHandScreenCoord.y);
		LogClass::Instance()->addEntry("Player_1_Left_Foot_Pos", players[0].leftFootScreenCoord.x, players[0].leftFootScreenCoord.y);
		LogClass::Instance()->addEntry("Player_1_Right_Foot_Pos", players[0].rightHandScreenCoord.x, players[0].rightHandScreenCoord.y);

		LogClass::Instance()->addEntry("Player_2_Head_Pos", players[1].headScreenCoord.x, players[1].headScreenCoord.y);
		LogClass::Instance()->addEntry("Player_2_Torso_Pos", players[1].torsoScreenCoord.x, players[1].torsoScreenCoord.y);
		LogClass::Instance()->addEntry("Player_2_Left_Hand_Pos", players[1].leftHandScreenCoord.x, players[1].leftHandScreenCoord.y);
		LogClass::Instance()->addEntry("Player_2_Right_Hand_Pos", players[1].rightHandScreenCoord.x, players[1].rightHandScreenCoord.y);
		LogClass::Instance()->addEntry("Player_2_Left_Foot_Pos", players[1].leftFootScreenCoord.x, players[1].leftFootScreenCoord.y);
		LogClass::Instance()->addEntry("Player_2_Right_Foot_Pos", players[1].rightHandScreenCoord.x, players[1].rightHandScreenCoord.y);
	}

    return hr;
}

void KinectClass::jointsToScreen(){

	D2D1_SIZE_F screenSize = renderTarget_->GetSize();

	for(int i=0; i < numPlayer; i++)
	{
		players[i].rightHandScreenCoord = SkeletonToScreen(players[i].rightHand, screenSize.width, screenSize.height);
		players[i].leftHandScreenCoord = SkeletonToScreen(players[i].leftHand, screenSize.width, screenSize.height);
		players[i].rightShoulderScreenCoord = SkeletonToScreen(players[i].rightShoulder, screenSize.width, screenSize.height);
		players[i].leftShoulderScreenCoord = SkeletonToScreen(players[i].leftShoulder, screenSize.width, screenSize.height);
		players[i].leftElbowScreenCoord = SkeletonToScreen(players[i].leftElbow, screenSize.width, screenSize.height);
		players[i].rightElbowScreenCoord = SkeletonToScreen(players[i].rightElbow, screenSize.width, screenSize.height);
		players[i].leftFootScreenCoord = SkeletonToScreen(players[i].leftFoot, screenSize.width, screenSize.height);
		players[i].rightFootScreenCoord = SkeletonToScreen(players[i].rightFoot, screenSize.width, screenSize.height);
		players[i].torsoScreenCoord = SkeletonToScreen(players[i].hipCenter, screenSize.width, screenSize.height);
		players[i].headScreenCoord = SkeletonToScreen(players[i].headCenter, screenSize.width, screenSize.height);

		Vector armDirection;
		armDirection.x = players[i].rightHandScreenCoord.x - players[i].rightElbowScreenCoord.x;
		armDirection.y = players[i].rightHandScreenCoord.y - players[i].rightElbowScreenCoord.y;

		// Calculate the angle of the forearm in radians
		players[i].rightHandRot = atan2(armDirection.y, armDirection.x);// * 180 / XM_PI;
		// Adjust value for having angle in polar coordinates
		if(players[i].rightHandRot <0)
		{
			players[i].rightHandRot *= -1;
		}
		else
		{
			players[i].rightHandRot = XM_2PI - players[i].rightHandRot;
		}
	}
}

void KinectClass::detectSekeltonsJoints(NUI_SKELETON_FRAME myFrame)
{
	lastNumPlayer = numPlayer;
	numPlayer = 0;

	players[0].skeletonID = NUI_SKELETON_INVALID_TRACKING_ID;
	players[1].skeletonID = NUI_SKELETON_INVALID_TRACKING_ID;

	for (int i = 0; i < NUI_SKELETON_COUNT; i++){

		NUI_SKELETON_TRACKING_STATE trackingState = myFrame.SkeletonData[i].eTrackingState;

		if (NUI_SKELETON_TRACKED == trackingState){
			players[numPlayer].skeletonID = myFrame.SkeletonData[i].dwTrackingID;

			//First Skeleton on the LEFT, holds RIGHT hand
			players[numPlayer].leftHand.x = myFrame.SkeletonData[i].SkeletonPositions[NUI_SKELETON_POSITION_HAND_LEFT].x;
			players[numPlayer].leftHand.y = myFrame.SkeletonData[i].SkeletonPositions[NUI_SKELETON_POSITION_HAND_LEFT].y;
			players[numPlayer].leftHand.z = myFrame.SkeletonData[i].SkeletonPositions[NUI_SKELETON_POSITION_HAND_LEFT].z;
			players[numPlayer].leftHand.w = myFrame.SkeletonData[i].SkeletonPositions[NUI_SKELETON_POSITION_HAND_LEFT].w;

			players[numPlayer].rightHand.x = myFrame.SkeletonData[i].SkeletonPositions[NUI_SKELETON_POSITION_HAND_RIGHT].x;
			players[numPlayer].rightHand.y = myFrame.SkeletonData[i].SkeletonPositions[NUI_SKELETON_POSITION_HAND_RIGHT].y;
			players[numPlayer].rightHand.z = myFrame.SkeletonData[i].SkeletonPositions[NUI_SKELETON_POSITION_HAND_RIGHT].z;
			players[numPlayer].rightHand.w = myFrame.SkeletonData[i].SkeletonPositions[NUI_SKELETON_POSITION_HAND_RIGHT].w;

			players[numPlayer].leftShoulder.x = myFrame.SkeletonData[i].SkeletonPositions[NUI_SKELETON_POSITION_SHOULDER_LEFT].x;
			players[numPlayer].leftShoulder.y = myFrame.SkeletonData[i].SkeletonPositions[NUI_SKELETON_POSITION_SHOULDER_LEFT].y;
			players[numPlayer].leftShoulder.z = myFrame.SkeletonData[i].SkeletonPositions[NUI_SKELETON_POSITION_SHOULDER_LEFT].z;
			players[numPlayer].leftShoulder.w = myFrame.SkeletonData[i].SkeletonPositions[NUI_SKELETON_POSITION_SHOULDER_LEFT].w;

			players[numPlayer].rightShoulder.x = myFrame.SkeletonData[i].SkeletonPositions[NUI_SKELETON_POSITION_SHOULDER_RIGHT].x;
			players[numPlayer].rightShoulder.y = myFrame.SkeletonData[i].SkeletonPositions[NUI_SKELETON_POSITION_SHOULDER_RIGHT].y;
			players[numPlayer].rightShoulder.z = myFrame.SkeletonData[i].SkeletonPositions[NUI_SKELETON_POSITION_SHOULDER_RIGHT].z;
			players[numPlayer].rightShoulder.w = myFrame.SkeletonData[i].SkeletonPositions[NUI_SKELETON_POSITION_SHOULDER_RIGHT].w;

			players[numPlayer].leftElbow.x =  myFrame.SkeletonData[i].SkeletonPositions[NUI_SKELETON_POSITION_ELBOW_LEFT].x;
			players[numPlayer].leftElbow.y =  myFrame.SkeletonData[i].SkeletonPositions[NUI_SKELETON_POSITION_ELBOW_LEFT].y;
			players[numPlayer].leftElbow.z =  myFrame.SkeletonData[i].SkeletonPositions[NUI_SKELETON_POSITION_ELBOW_LEFT].z;
			players[numPlayer].leftElbow.w =  myFrame.SkeletonData[i].SkeletonPositions[NUI_SKELETON_POSITION_ELBOW_LEFT].w;

			players[numPlayer].rightElbow.x =  myFrame.SkeletonData[i].SkeletonPositions[NUI_SKELETON_POSITION_ELBOW_RIGHT].x;
			players[numPlayer].rightElbow.y =  myFrame.SkeletonData[i].SkeletonPositions[NUI_SKELETON_POSITION_ELBOW_RIGHT].y;
			players[numPlayer].rightElbow.z =  myFrame.SkeletonData[i].SkeletonPositions[NUI_SKELETON_POSITION_ELBOW_RIGHT].z;
			players[numPlayer].rightElbow.w =  myFrame.SkeletonData[i].SkeletonPositions[NUI_SKELETON_POSITION_ELBOW_RIGHT].w;

			players[numPlayer].leftFoot.x = myFrame.SkeletonData[i].SkeletonPositions[NUI_SKELETON_POSITION_FOOT_LEFT].x;
			players[numPlayer].leftFoot.y = myFrame.SkeletonData[i].SkeletonPositions[NUI_SKELETON_POSITION_FOOT_LEFT].y;
			players[numPlayer].leftFoot.z = myFrame.SkeletonData[i].SkeletonPositions[NUI_SKELETON_POSITION_FOOT_LEFT].z;
			players[numPlayer].leftFoot.w = myFrame.SkeletonData[i].SkeletonPositions[NUI_SKELETON_POSITION_FOOT_LEFT].w;

			players[numPlayer].rightFoot.x = myFrame.SkeletonData[i].SkeletonPositions[NUI_SKELETON_POSITION_FOOT_RIGHT].x;
			players[numPlayer].rightFoot.y = myFrame.SkeletonData[i].SkeletonPositions[NUI_SKELETON_POSITION_FOOT_RIGHT].y;
			players[numPlayer].rightFoot.z = myFrame.SkeletonData[i].SkeletonPositions[NUI_SKELETON_POSITION_FOOT_RIGHT].z;
			players[numPlayer].rightFoot.w = myFrame.SkeletonData[i].SkeletonPositions[NUI_SKELETON_POSITION_FOOT_RIGHT].w;

			players[numPlayer].hipCenter.x = myFrame.SkeletonData[i].SkeletonPositions[NUI_SKELETON_POSITION_HIP_CENTER].x;
			players[numPlayer].hipCenter.y = myFrame.SkeletonData[i].SkeletonPositions[NUI_SKELETON_POSITION_HIP_CENTER].y;
			players[numPlayer].hipCenter.z = myFrame.SkeletonData[i].SkeletonPositions[NUI_SKELETON_POSITION_HIP_CENTER].z;
			players[numPlayer].hipCenter.w = myFrame.SkeletonData[i].SkeletonPositions[NUI_SKELETON_POSITION_HIP_CENTER].w;

			players[numPlayer].headCenter.x = myFrame.SkeletonData[i].SkeletonPositions[NUI_SKELETON_POSITION_HEAD].x;
			players[numPlayer].headCenter.y = myFrame.SkeletonData[i].SkeletonPositions[NUI_SKELETON_POSITION_HEAD].y;
			players[numPlayer].headCenter.z = myFrame.SkeletonData[i].SkeletonPositions[NUI_SKELETON_POSITION_HEAD].z;
			players[numPlayer].headCenter.w = myFrame.SkeletonData[i].SkeletonPositions[NUI_SKELETON_POSITION_HEAD].w;

			numPlayer++;
		}
	}
}

HRESULT KinectClass::ChooseSkeleton(NUI_SKELETON_DATA* pSkeletonData)
{
	HRESULT hr = S_OK;

	// First we go through the stream to find the closest skeleton, and also check whether our current tracked
	// skeleton is still visibile in the stream
	float closestSkeletonDistance = FLT_MAX;
	float secondClosestSkeletonDistance = FLT_MAX;

	DWORD closestSkeleton = NUI_SKELETON_INVALID_TRACKING_ID;
	DWORD secondClosestSkeleton = NUI_SKELETON_INVALID_TRACKING_ID;

	BOOL isTrackedSkeletonVisible = false;
	BOOL isSecondTrackedSkeletonVisible = false;

	for(int i = 0; i < NUI_SKELETON_COUNT; ++i)
	{
		NUI_SKELETON_DATA skeleton = pSkeletonData[i];
		if(NUI_SKELETON_TRACKED == skeleton.eTrackingState)
		{
			/*if(trackedSkeleton_[0] == skeleton.dwTrackingID)
			{
				isTrackedSkeletonVisible = true;
				//break;
			}
			if(trackedSkeleton_[1] == skeleton.dwTrackingID)
			{
				isSecondTrackedSkeletonVisible = true;
				//break;
			}*/

			if(skeleton.Position.z < closestSkeletonDistance)
			{
				closestSkeleton = skeleton.dwTrackingID;
				closestSkeletonDistance = skeleton.Position.z;
			}
			if(closestSkeleton != skeleton.dwTrackingID)
			{
				secondClosestSkeleton = skeleton.dwTrackingID;
				secondClosestSkeletonDistance = skeleton.Position.z;
			}
		}
	}

	// Now we choose a new skeleton unless the currently tracked skeleton is still visible
	if(/*!isTrackedSkeletonVisible && */players[0].skeletonID != NUI_SKELETON_INVALID_TRACKING_ID)
	{
		hr = backgroundRemovalStream_[0]->SetTrackedPlayer(players[0].skeletonID);
		if (FAILED(hr))
		{
			return hr;
		}

		trackedSkeleton_[0] = players[0].skeletonID;
	}

	if(/*!isSecondTrackedSkeletonVisible && */players[1].skeletonID != NUI_SKELETON_INVALID_TRACKING_ID)
	{
		hr = backgroundRemovalStream_[1]->SetTrackedPlayer(players[1].skeletonID);
		if (FAILED(hr))
		{
			return hr;
		}

		trackedSkeleton_[1] = players[1].skeletonID;
	}

	return hr;
}

HRESULT KinectClass::ComposeImage()
{
    HRESULT hr;
    NUI_BACKGROUND_REMOVED_COLOR_FRAME bgRemovedFrame;

	if(drawUser1_)
	{
		// We get the pointer to the background removal stream fro the second user
		//const BYTE* pBackgroundRemovedColor = bgRemovedFrame.pBackgroundRemovedColorData;

		// Create an integer variable that keeps the size in pixels of images
		int dataLength = static_cast<int>(colorWidth_) * static_cast<int>(colorHeight_);

		// We lock the frame for the first background removal
		hr = backgroundRemovalStream_[0]->GetNextFrame(0, &bgRemovedFrame);
		if (FAILED(hr))
		{
			return hr;
		}

		const BYTE* pBackgroundRemovedColor = bgRemovedFrame.pBackgroundRemovedColorData;

		for (int i = 0; i < dataLength; ++i)
		{
			if(pBackgroundRemovedColor[i*4+3] < 64)
			{
				// We don't erase the other data to avoid deleting the previous drawn user
				// If we put the pixels to alpha 0 then we would be deleting the previous drawn pixels
				outputRGBX_[i*4] = 0;
				outputRGBX_[i*4+1] = 0;
				outputRGBX_[i*4+2] = 0;
				outputRGBX_[i*4+3] = 0;
			}
			else
			{
				outputRGBX_[i*4] = pBackgroundRemovedColor[i*4]*userColor_.x;
				outputRGBX_[i*4+1] = pBackgroundRemovedColor[i*4+1]*userColor_.y;
				outputRGBX_[i*4+2] = pBackgroundRemovedColor[i*4+2]*userColor_.z;
				outputRGBX_[i*4+3] = 256*userColor_.w;
			}
		}

		// We remove the locks to the images of the backroung removal streams
		hr = backgroundRemovalStream_[0]->ReleaseFrame(&bgRemovedFrame);
		if (FAILED(hr))
		{
			return hr;
		}

		// We copy the generated image to the bitmap we will display to the screen
		hr = bitmap_[0]->CopyFromMemory(NULL, outputRGBX_, colorWidth_*4);

		if ( FAILED(hr) )
		{
			MessageBox(NULL, L"Could not copy data to bitmap.", L"Draw Error", MB_OK);
			return hr;
		}

		drawUser1_ = false;
	}
	else
	{
		// We get the pointer to the background removal stream fro the second user
		//const BYTE* pBackgroundRemovedColor = bgRemovedFrame.pBackgroundRemovedColorData;

		// Create an integer variable that keeps the size in pixels of images
		int dataLength = static_cast<int>(colorWidth_) * static_cast<int>(colorHeight_);

		// We lock the frame for the first background removal
		hr = backgroundRemovalStream_[1]->GetNextFrame(0, &bgRemovedFrame);
		if (FAILED(hr))
		{
			return hr;
		}

		const BYTE* pBackgroundRemovedColor = bgRemovedFrame.pBackgroundRemovedColorData;

		for (int i = 0; i < dataLength; ++i)
		{
			if(pBackgroundRemovedColor[i*4+3] < 64)
			{
				// We don't erase the other data to avoid deleting the previous drawn user
				// If we put the pixels to alpha 0 then we would be deleting the previous drawn pixels
				outputRGBX_[i*4] = 0;
				outputRGBX_[i*4+1] = 0;
				outputRGBX_[i*4+2] = 0;
				outputRGBX_[i*4+3] = 0;
			}
			else
			{
				outputRGBX_[i*4] = pBackgroundRemovedColor[i*4]*userColor_.x;
				outputRGBX_[i*4+1] = pBackgroundRemovedColor[i*4+1]*userColor_.y;
				outputRGBX_[i*4+2] = pBackgroundRemovedColor[i*4+2]*userColor_.z;
				outputRGBX_[i*4+3] = 256*userColor_.w;
			}
		}

		// We remove the locks to the images of the backroung removal streams
		hr = backgroundRemovalStream_[1]->ReleaseFrame(&bgRemovedFrame);
		if (FAILED(hr))
		{
			return hr;
		}

		// We copy the generated image to the bitmap we will display to the screen
		hr = bitmap_[1]->CopyFromMemory(NULL, outputRGBX_, colorWidth_*4);

		if ( FAILED(hr) )
		{
			MessageBox(NULL, L"Could not copy data to bitmap.", L"Draw Error", MB_OK);
			return hr;
		}

		drawUser1_ = true;
	}

    //hr = drawBackgroundRemovalBasics->Draw(outputRGBX_, colorWidth_ * colorHeight_ * 4);

    return hr;
}

Point KinectClass::SkeletonToScreen(Vector4 skeletonPoint, int width, int height)
{
	LONG x, y;
	USHORT depth;

	// Calculate the skeleton's position on the screen
	// NuiTransformSkeletonToDepthImage returns coordinates in NUI_IMAGE_RESOLUTION_320x240 space
	NuiTransformSkeletonToDepthImage(skeletonPoint, &x, &y, &depth);

	float screenPointX = static_cast<float>(x * width) / 320;
	float screenPointY = static_cast<float>(y * height) / 240;

	return Point(screenPointX, screenPointY);
}
