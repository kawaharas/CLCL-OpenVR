////////////////////////////////////////////////////////////////////////////////
//
// ovrvision.cpp
//
//   CLCL: CAVELib Compatible Library
//
//     Copyright 2015-2019 Shintaro Kawahara(kawahara@jamstec.go.jp).
//     All rights reserved.
//
//   Please read the file "LICENCE.txt" before you use this software.
//         
////////////////////////////////////////////////////////////////////////////////

#include "ovrvision.h"

#ifdef USE_OVRVISION

OVRVision::OVRVision()
{
	p_OVRVision = nullptr;
	m_Width = m_Height = 0, m_PixelSize = 4;
	m_IsOpen = m_CameraState = false;
#ifdef USE_THREAD_FOR_CAMERA_PROCESS
	m_IsThreadRunning = true;
#endif // USE_THREAD_FOR_CAMERA_PROCESS
}

bool OVRVision::Init()
{
#ifdef USE_OVRVISION_PRO
	p_OVRVision = new OVR::OvrvisionPro();
	int OVRVisionState = p_OVRVision->Open(0, OVR::OV_CAMHD_FULL); // 960x950 @60fps x2
	if (OVRVisionState == 1)
	{
		m_IsOpen = true;
	}
#else
	p_OVRVision = new OVR::Ovrvision();
	int OVRVisionState = p_OVRVision->Open(0, OVR::OV_CAMVGA_FULL);	// 640 x 480 60fps
	if (OVRVisionState == OV_RESULT_OK)
	{
		m_IsOpen = true;
	}
#endif // USE_OVRVISION_PRO

#if defined(USE_OVRVISION_PRO) && defined(USE_THREAD_FOR_CAMERA_PROCESS)
	m_HMutex = CreateMutex(NULL, FALSE, NULL);
	m_HCamera = (HANDLE)_beginthread(&OVRVision::launchThread, 0, this);
#endif // USE_OVRVISION_PRO && USE_THREAD_FOR_CAMERA_PROCESS

	if (m_IsOpen)
	{
#ifdef USE_OVRVISION_PRO
		p_OVRVision->SetCameraSyncMode(false);
		std::cout << "GetCameraWhiteBalanceAuto() = ";
		if (p_OVRVision->GetCameraWhiteBalanceAuto() == true)
			std::cout << "true\n";
		else
			std::cout << "false\n";
		p_OVRVision->SetCameraGain(25);
		std::cout << "GetCameraGain() = ";
		std::cout << p_OVRVision->GetCameraGain() << std::endl;
		m_Width = p_OVRVision->GetCamWidth();
		m_Height = p_OVRVision->GetCamHeight();
		m_PixelSize = p_OVRVision->GetCamPixelsize();
		m_Format = GL_BGRA;
#else
		m_Width = p_OVRVision->GetImageWidth();
		m_Height = p_OVRVision->GetImageHeight();
		m_PixelSize = p_OVRVision->GetPixelSize();
		m_Format = GL_RGB;
#endif // USE_OVRVISION_PRO

		glGenTextures(2, m_TexID);
		for (int i = 0; i < 2; i++)
		{
			glBindTexture(GL_TEXTURE_2D, m_TexID[i]);
			glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
			glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, m_Width, m_Height, 0, m_Format, GL_UNSIGNED_BYTE, NULL);
		}
		glBindTexture(GL_TEXTURE_2D, 0);

		std::cout << "OVRVision: ENABLE" << std::endl;
		std::cout << "OVRVision: Width     : " << m_Width << std::endl;
		std::cout << "OVRVision: Height    : " << m_Height << std::endl;
		std::cout << "OVRVision: PixelSize : " << m_PixelSize << std::endl;
		return true;
	}
	else
	{
		std::cout << "OVRVision: DISABLE" << std::endl;
		delete p_OVRVision;
		return false;
	}
}

void OVRVision::Terminate()
{
	if (m_IsOpen)
	{
#ifdef USE_THREAD_FOR_CAMERA_PROCESS
		CloseHandle(m_HCamera);
		CloseHandle(m_HMutex);
#endif // USE_THREAD_FOR_CAMERA_PROCESS
		glDeleteTextures(2, m_TexID);
		p_OVRVision->Close();
		delete p_OVRVision;
	}
}

void OVRVision::PreStore()
{
	if (m_IsOpen && m_CameraState)
	{
#ifdef USE_OVRVISION_PRO
		p_OVRVision->PreStoreCamData(OVR::Camqt::OV_CAMQT_DMSRMP);
#else
		p_OVRVision->PreStoreCamData();
#endif // USE_OVRVISION_PRO
	}
}

void OVRVision::DrawImege(int eyeIndex)
{
	if (m_IsOpen && m_CameraState)
	{
		glUseProgram(0);

		unsigned char* imagePtr = nullptr;
		if (eyeIndex == 0)
		{
#ifdef USE_OVRVISION_PRO
			imagePtr = p_OVRVision->GetCamImageBGRA(OVR::OV_CAMEYE_LEFT);
#else
			imagePtr = p_OVRVision->GetCamImage(OVR::OV_CAMEYE_LEFT, OVR::OV_PSQT_NONE);
#endif // USE_OVRVISION_PRO
		}
		else
		{
#ifdef USE_OVRVISION_PRO
			imagePtr = p_OVRVision->GetCamImageBGRA(OVR::OV_CAMEYE_RIGHT);
#else
			imagePtr = p_OVRVision->GetCamImage(OVR::OV_CAMEYE_RIGHT, OVR::OV_PSQT_NONE);
#endif // USE_OVRVISION_PRO
		}

		glBindTexture(GL_TEXTURE_2D, m_TexID[eyeIndex]);
		glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, m_Width, m_Height, m_Format, GL_UNSIGNED_BYTE, imagePtr);

		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		glPushMatrix();
		glEnable(GL_TEXTURE_2D);
		glDisable(GL_DEPTH_TEST);
		glDisable(GL_LIGHTING);
		glDepthMask(false);
		glColor3f(1.0f, 1.0f, 1.0f);
		glBegin(GL_QUADS);
#ifdef USE_OVRVISION_PRO
#ifdef USE_SDK_0_5_0_1
		glTexCoord2f(0.0f, 0.0f);  glVertex2f(-1.0f,  1.0f);
		glTexCoord2f(0.0f, 1.0f);  glVertex2f(-1.0f, -1.0f);
		glTexCoord2f(1.0f, 1.0f);  glVertex2f( 1.0f, -1.0f);
		glTexCoord2f(1.0f, 0.0f);  glVertex2f( 1.0f,  1.0f);
#else
		float ipd = 0.3f; // interpupillary distance
		float aspect = static_cast<float>(m_Height) / static_cast<float>(m_Width) * 0.82f;
		float zoom = 1.6f;
		if (eyeIndex == 0)
		{
			glTexCoord2f(0.0f, 0.0f);  glVertex2f(-zoom,  zoom * aspect);
			glTexCoord2f(0.0f, 1.0f);  glVertex2f(-zoom, -zoom * aspect);
			glTexCoord2f(1.0f, 1.0f);  glVertex2f( zoom, -zoom * aspect);
			glTexCoord2f(1.0f, 0.0f);  glVertex2f( zoom,  zoom * aspect);
		}
		else
		{
			glTexCoord2f(0.0f, 0.0f);  glVertex2f(-zoom - ipd,  zoom * aspect);
			glTexCoord2f(0.0f, 1.0f);  glVertex2f(-zoom - ipd, -zoom * aspect);
			glTexCoord2f(1.0f, 1.0f);  glVertex2f( zoom - ipd, -zoom * aspect);
			glTexCoord2f(1.0f, 0.0f);  glVertex2f( zoom - ipd,  zoom * aspect);
		}
#endif // USE_SDK_0_5_0_1
#else
		glTexCoord2f(1.0f, 0.0f);  glVertex2f(-1.0f, -1.0f);
		glTexCoord2f(0.0f, 0.0f);  glVertex2f( 1.0f, -1.0f);
		glTexCoord2f(0.0f, 1.0f);  glVertex2f( 1.0f,  1.0f);
		glTexCoord2f(1.0f, 1.0f);  glVertex2f(-1.0f,  1.0f);
#endif // USE_OVRVISION_PRO
		glEnd();
		glEnable(GL_LIGHTING);
		glEnable(GL_DEPTH_TEST);
		glDisable(GL_TEXTURE_2D);
		glDepthMask(true);
		glPopMatrix();
		glClear(GL_DEPTH_BUFFER_BIT);
		glBindTexture(GL_TEXTURE_2D, 0);

	}
}

#ifdef USE_THREAD_FOR_CAMERA_PROCESS
void OVRVision::CameraThread()
{
	while (m_IsThreadRunning)
	{
		WaitForSingleObject(m_HMutex, INFINITE);

#ifdef USE_OVRVISION_PRO
		p_OVRVision->PreStoreCamData(OVR::Camqt::OV_CAMQT_DMSRMP);
#else
		p_OVRVision->PreStoreCamData();
#endif // USE_OVRVISION_PRO
		ReleaseMutex(m_HMutex);
	}
	_endthread();
}
#endif // USE_THREAD_FOR_CAMERA_PROCESS

#endif // USE_OVRVISION
