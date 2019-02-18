////////////////////////////////////////////////////////////////////////////////
//
// ovrvision.h
//
//   CLCL: CAVELib Compatible Library
//
//     Copyright 2015-2019 Shintaro Kawahara(kawahara@jamstec.go.jp).
//     All rights reserved.
//
//   Please read the file "LICENCE.txt" before you use this software.
//         
////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "../../settings.h"

#ifdef USE_OVRVISION
#include <iostream>
#include <GL/glew.h>
#include <windows.h>
//#include <GLFW/glfw3.h>
//#include <GLFW/glfw3native.h>

#ifdef USE_OVRVISION_PRO
	#include <ovrvision_pro.h>
	#pragma comment(lib, "ovrvision64.lib")
	#ifdef USE_THREAD_FOR_CAMERA_PROCESS
//		#include <thread>
		#include <process.h>
	#endif // USE_THREAD_FOR_CAMERA_PROCESS
#else
	#include <ovrvision.h>
	#pragma comment(lib, "ovrvision_64.lib")
#endif // USE_OVRVISION_PRO

class OVRVision {
#ifdef USE_OVRVISION_PRO
	OVR::OvrvisionPro* p_OVRVision;
#else
	OVR::Ovrvision* p_OVRVision;
#endif // USE_OVRVISION_PRO

	int    OVRVisionState; // if OvrvisionPro->Open() succeeded, it return a value less than zero.
	bool   m_IsOpen;
	bool   m_CameraState;
	int    m_Width, m_Height, m_PixelSize;
	GLenum m_Format;
	GLuint m_TexID[2];

#ifdef USE_THREAD_FOR_CAMERA_PROCESS
	static void launchThread(void *obj) { reinterpret_cast<OVRVision *>(obj)->CameraThread(); }
	void   CameraThread();
	HANDLE m_HMutex; // handles for the thread
	HANDLE m_HCamera;
	bool   m_IsThreadRunning; // flag to stop the thread for prestore image
#endif // USE_THREAD_FOR_CAMERA_PROCESS
public:
	OVRVision();

	bool   Init();
	void   Terminate();
	void   PreStore();
	void   DrawImege(int eyeIndex);
	bool   IsOpen() { return m_IsOpen; }
	bool   cameraState() { return m_CameraState; }
	void   toggleCameraState() { m_CameraState = !m_CameraState; }
	int    width() { return m_Width; }
	int    height() { return m_Height; }
	int    pixelSize() { return m_PixelSize; }
#ifdef USE_THREAD_FOR_CAMERA_PROCESS
	void   StopThread() { m_IsThreadRunning = false; }
#endif // USE_THREAD_FOR_CAMERA_PROCESS
};

#endif // USE_OVRVISION
