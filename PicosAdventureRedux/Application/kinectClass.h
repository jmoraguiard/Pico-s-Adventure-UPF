#ifndef _KINECT_CLASS_H_
#define _KINECT_CLASS_H_

#include <windows.h>

#pragma warning( disable : 4005 )

#include <D2D1.h>
#include <xnamath.h>

#include "NuiApi.h"
#include <KinectBackgroundRemoval.h>

#include "inputManager.h"

#include "../Game/logClass.h"

#include "../Math/pointClass.h"
#include "../Math/vectorClass.h"

#include "../Utils/notifierClass.h"
#include "../Utils/listenerClass.h"

#pragma comment(lib, "d2d1")

class ApplicationManager;

enum messageType
	{
		GREETINGS_KINECT,
		FIRST_RIGHT_HAND_ROT,
		SECOND_RIGHT_HAND_ROT,
		FIRST_LEFT_HAND_ROT,
		SECOND_LEFT_HAND_ROT,
		HOLD_HANDS,
		SKELETON_LOST
	};

struct KinectStruct
	{
		messageType type;
		Point handPos;
		Point secondJoint;
		Point thirdJoint;	
		float handRot;
		bool boolean;
	};

struct Player
	{
		DWORD skeletonID;
		Vector4 leftHand, rightHand;
		Vector4 leftShoulder, rightShoulder;
		Vector4 leftElbow, rightElbow;
		Vector4 leftFoot, rightFoot;
		Vector4 hipCenter, headCenter;
		Point leftHandScreenCoord, rightHandScreenCoord;
		Point leftElbowScreenCoord, rightElbowScreenCoord;
		Point leftShoulderScreenCoord, rightShoulderScreenCoord;
		Point leftFootScreenCoord, rightFootScreenCoord;
		Point torsoScreenCoord, headScreenCoord;
		float rightHandRot;
	};

class KinectClass : public Notifier<KinectClass, KinectStruct>
{
	public:
		KinectClass();
		KinectClass(const KinectClass&);
		~KinectClass();

		bool setup(HWND, IDXGISwapChain*);
		void update();
		void draw();
		void destroy();

		void setUserColor(XMFLOAT4 color);

	private:
		Point SkeletonToScreen(Vector4 skeletonPoint, int width, int height);
		void detectSekeltonsJoints(NUI_SKELETON_FRAME myFrame);
		void jointsToScreen();

		HRESULT			ProcessDepth();
		HRESULT			ProcessColor();
		HRESULT			ProcessSkeleton();
		HRESULT			ComposeImage();

		HRESULT         ChooseSkeleton(NUI_SKELETON_DATA* pSkeletonData);

		HWND            hWnd_;
		BOOL			nearMode_;

		INuiSensor*     nuiSensor_;

		ID2D1Factory*   D2DFactory_;
		ID2D1RenderTarget* renderTarget_;
		ID2D1Bitmap*    bitmap_[2];
		bool			drawUser1_;

		HANDLE			hEvents[4];
		HANDLE          depthStreamHandle_;
		HANDLE          nextDepthFrameEvent_;
		HANDLE          colorStreamHandle_;
		HANDLE          nextColorFrameEvent_;
		HANDLE          nextSkeletonFrameEvent_;
		HANDLE          nextBackgroundRemovedFrameEvent_;

		BYTE*           backgroundRGBX_;
		BYTE*           outputRGBX_;

		UINT            colorWidth_;
		UINT            colorHeight_;
		UINT            depthWidth_;
		UINT            depthHeight_;

		INuiBackgroundRemovedColorStream*  backgroundRemovalStream_[2];
		
		DWORD           trackedSkeleton_[2];

		XMFLOAT4		userColor_;

		Player players[2];
		int numPlayer,lastNumPlayer;
		bool risedHand; 
		int countSwipe;
		int lastHandPosition,handPosition;
		double handsDist;
		KinectStruct kinectStruct;
};

#endif