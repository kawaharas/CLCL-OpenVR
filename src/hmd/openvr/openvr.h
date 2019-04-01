////////////////////////////////////////////////////////////////////////////////
//
// openvr.h
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

#define _CRT_SECURE_NO_WARNINGS
#include "../../settings.h"

#define ENABLE_CONTROLLER_MODEL

const float FEET_PER_METER = 3.280840f;

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX // to use "std::max()"
#include <windows.h>
#endif // _WIN32

#define _USE_MATH_DEFINES
#include <vector>
#include <cmath>
#include <iostream>
#include <algorithm>
#include <atomic>
#include <thread>
#include <process.h>

#define GLEW_STATIC
#include <GL/glew.h>

#define GLFW_EXPOSE_NATIVE_WIN32
#define GLFW_EXPOSE_NATIVE_WGL
#define GLFW_INCLUDE_GLU
#define OVR_OS_WIN32
#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>
#pragma comment(lib, "legacy_stdio_definitions.lib")
#pragma comment(lib, "GLFW3.lib")
#pragma comment(lib, "GLEW32s.lib")
#pragma comment(lib, "opengl32.lib")
#pragma comment(lib, "glu32.lib")
#pragma comment(lib, "winmm.lib")

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/vec3.hpp> // glm::vec3
#include <glm/vec4.hpp> // glm::vec4
#include <glm/mat4x4.hpp> // glm::mat4
#include <glm/gtx/transform.hpp> // glm::scale
#include <glm/gtc/matrix_transform.hpp> // glm::translate, glm::rotate, glm::scale, glm::perspective
#include <glm/gtc/type_ptr.hpp> // glm::value_ptr

#ifdef USE_OVRVISION
#include "../../camera/ovrvision/ovrvision.h"
#endif // USE_OVRVISION

typedef void(*OVRCALLBACK)();
typedef void(*OVRCALLBACK1)(void*);
typedef void(*OVRCALLBACK2)(void*, void*);
typedef void(*OVRCALLBACK3)(void*, void*, void*);
typedef void(*OVRCALLBACK4)(void*, void*, void*, void*);
typedef void(*OVRCALLBACK5)(void*, void*, void*, void*, void*);

#include <openvr.h>
#pragma comment(lib, "openvr_api")

typedef enum {
	VECTOR_UP = 0,
	VECTOR_FRONT,
	VECTOR_RIGHT
} VECTOR_TYPE;

typedef enum {
	HTC_VIVE = 0,
	OCULUS_RIFT_CV1,
	WINDOWS_MR
} DEVICE_TYPE;

#ifdef ENABLE_CONTROLLER_MODEL
class CGLRenderModel
{
public:
	CGLRenderModel(const std::string & sRenderModelName);
	~CGLRenderModel();

	bool BInit(const vr::RenderModel_t & vrModel, const vr::RenderModel_TextureMap_t & vrDiffuseTexture);
	void Cleanup();
	void Draw();
	const std::string & GetName() const { return m_sModelName; }

private:
	GLuint m_glVertBuffer;
	GLuint m_glIndexBuffer;
	GLuint m_glVertArray;
	GLuint m_glTexture;
	GLsizei m_unVertexCount;
	std::string m_sModelName;
};
#endif // ENABLE_CONTROLLER_MODEL

class OpenVR {
public:
	OpenVR();
	~OpenVR();

	GLFWwindow* window() { return m_Window; }

	vr::IVRSystem* hmdSession() { return m_HmdSession; }

	void Init();
	void InitGL();
	void CreateBuffers();
	void Terminate();
	void UpdateTrackingData();
	void PreProcess();
	void PostProcess();
	void SubmitFrame(int eyeIndex);
	void SetMatrix(int eyeIndex);
	void Translate(float x, float y, float z);
	void Rotate(float angle_degree, char axis);
	void Scale(float x, float y, float z);
	void WorldTranslate(float x, float y, float z);
	void WorldRotate(float angle, char axis);
	void WorldScale(float x, float y, float z);
	glm::mat4 GetNavigationMatrix();
	void LoadNavigationMatrix(glm::mat4 matrix);
	void SetNavigationMatrixIdentity();
	void SetNavigationMatrix();
	void SetNavigationInverseMatrix();
	void MultiNavigationMatrix(float matrix[4][4]);
	void PreMultiNavigationMatrix(float matrix[4][4]);
	void StoreNavigationMatrix() { m_NavigationMatrix_Backup = m_NavigationMatrix; }
	void RestoreNavigationMatrix() { m_NavigationMatrix = m_NavigationMatrix_Backup; }
	int  ShouldClose() const { return glfwWindowShouldClose(m_Window); }
	void PollEvents() { glfwPollEvents(); }

	glm::mat4 projectionMatrix(int eyeIndex) { return m_ProjectionMatrix[eyeIndex]; }
	glm::vec3 bodyTranslation() { return m_BodyTranslation; }
	glm::vec3 headTranslation() { return m_HeadTranslation; }
	glm::vec3 headOrientation() { return m_HeadOrientation; }
	glm::vec3 headVector(VECTOR_TYPE type) { return m_HeadVector[type]; }
	glm::vec3 headVectorNav(VECTOR_TYPE type) { return m_HeadVectorNav[type]; }
	glm::vec3 headTranslationNav() { return m_HeadTranslationNav; }
	glm::vec3 headOrientationNav() { return m_HeadOrientationNav; }

	glm::vec3 handTranslation() { return m_HandTranslation; }
	glm::vec3 handTranslationNav() { return m_HandTranslationNav; }
	glm::vec3 handVector(VECTOR_TYPE type) { return m_HandVector[type]; }
	glm::vec3 handVectorNav(VECTOR_TYPE type) { return m_HandVectorNav[type]; }

	int renderTargetWidth() { return m_FrameBufferWidth; }
	int renderTargetHeight() { return m_FrameBufferHeight; }
	ULONG64  frameIndex() { return m_FrameIndex; }

	vr::VRControllerState_t controllerState() { return m_ControllerState; }
	bool IsControllerConnected() { return m_IsControllerConnected; }
	int  GetButtonState(int buttonNumber);
	std::pair<float, float> GetJoyStickValue();

#ifdef ENABLE_CONTROLLER_MODEL
	bool IsControllerModelLoaded() { return m_IsControllerModelLoaded; }
	bool IsControllerModelVisible() { return m_IsControllerModelVisible; }
#endif // ENABLE_CONTROLLER_MODEL

	DEVICE_TYPE GetDeviceType() { return m_DeviceType; }

	void StartThread();
	void StopThread();
	bool IsMainThread();
	bool IsDisplayThread();

	void SetInitFunction(OVRCALLBACK callback, std::vector<void*> arg_list)
	{
		p_InitFunction = callback;
		m_InitFunctionArgs = arg_list;
	}
	void SetStopFunction(OVRCALLBACK callback, std::vector<void*> arg_list)
	{
		p_StopFunction = callback;
		m_StopFunctionArgs = arg_list;
	}
	void SetDrawFunction(OVRCALLBACK callback, std::vector<void*> arg_list)
	{
		p_DrawFunction = callback;
		m_DrawFunctionArgs = arg_list;
	}
	void SetIdleFunction(OVRCALLBACK callback, std::vector<void*> arg_list)
	{
		p_IdleFunction = callback;
		m_IdleFunctionArgs = arg_list;
	}

	bool GetKey(int);
	int  GetMouseButton(int);

#ifdef ENABLE_CONTROLLER_MODEL
	struct ControllerInfo_t
	{
		vr::VRInputValueHandle_t m_source = vr::k_ulInvalidInputValueHandle;
		vr::VRActionHandle_t m_actionPose = vr::k_ulInvalidActionHandle;
		vr::VRActionHandle_t m_actionHaptic = vr::k_ulInvalidActionHandle;
		glm::mat4 m_rmat4Pose;
		CGLRenderModel *m_pRenderModel = nullptr;
		std::string m_sRenderModelName;
		bool m_bShowController;
	};
	enum EHand
	{
		Left = 0,
		Right = 1,
	};
	ControllerInfo_t m_rHand[2];
#endif // ENABLE_CONTROLLER_MODEL

private:
	vr::IVRSystem* m_HmdSession = nullptr;
	int m_NumEyes;

	GLFWwindow *m_Window;
	int   m_WindowWidth;
	int   m_WindowHeight;
	float m_NearPlaneZ;
	float m_FarPlaneZ;
	float m_VerticalFieldOfView;
	uint32_t m_FrameBufferWidth;
	uint32_t m_FrameBufferHeight;
	glm::mat4 m_HeadPose;
	glm::mat4 m_EyePose[2];
	glm::mat4 m_HeadToWorldMatrix;
	glm::mat4 m_ProjectionMatrix[2];
	GLuint   m_FrameBuffer[2];
	GLuint   m_TextureBuffer[2];
	GLuint   m_DepthBuffer[2];
	ULONG64  m_FrameIndex;

	int      m_ButtonState[4];

	glm::vec3 m_HeadTranslation;
	glm::vec3 m_HeadOrientation;
	glm::vec3 m_HeadVector[3];
	glm::vec3 m_HeadTranslationNav;
	glm::vec3 m_HeadOrientationNav;
	glm::vec3 m_HeadVectorNav[3];
	glm::vec3 m_BodyTranslation;
	glm::vec3 m_BodyRotation;
	glm::vec3 m_HandTranslation;
	glm::vec3 m_HandTranslationNav;
	glm::vec3 m_HandVector[3];
	glm::vec3 m_HandVectorNav[3];

	glm::mat4 m_HandPose;

	int m_CurrentEyeIndex;
	glm::mat4 m_NavigationMatrix;
	glm::mat4 m_NavigationMatrix_Backup;
	glm::mat4 m_ModelMatrix;

	vr::VRControllerState_t m_ControllerState;
	bool m_IsControllerConnected;
#ifdef ENABLE_CONTROLLER_MODEL
	void   DrawController(int eyeIndex);
	bool   CreateShader();
	GLuint CompileGLShader(const char *pchShaderName, const char *pchVertexShader, const char *pchFragmentShader);
	bool   m_IsControllerModelLoaded;
	bool   m_IsControllerModelVisible;
	GLuint m_unRenderModelProgramID;
	GLint  m_nRenderModelMatrixLocation;
#endif // ENABLE_CONTROLLER_MODEL

	bool                m_IsInitFunctionExecuted;
	OVRCALLBACK         p_InitFunction;
	OVRCALLBACK         p_StopFunction;
	OVRCALLBACK         p_DrawFunction;
	OVRCALLBACK         p_IdleFunction;
	std::vector<void*>  m_InitFunctionArgs;
	std::vector<void*>  m_StopFunctionArgs;
	std::vector<void*>  m_DrawFunctionArgs;
	std::vector<void*>  m_IdleFunctionArgs;

	HANDLE m_HMutex;
	HANDLE m_HRender;
	bool   m_IsThreadRunning; // flag to stop the thread
	std::atomic<bool>   m_IsInitializedGLFW;
	DWORD  m_MainThreadID;
	DWORD  m_DisplayThreadID;

	DEVICE_TYPE m_DeviceType;

#ifdef USE_OVRVISION
	OVRVision           m_OVRVision;
#endif // USE_OVRVISION

	void ExecInitCallback()
	{
		if (m_IsInitFunctionExecuted) return;

		if (p_InitFunction != nullptr)
		{
			std::vector<void*> args = m_InitFunctionArgs;
			switch (args.size())
			{
				case 0:
					p_InitFunction();
					break;
				case 1:
					((OVRCALLBACK1)p_InitFunction)(args[0]);
					break;
				case 2:
					((OVRCALLBACK2)p_InitFunction)(args[0], args[1]);
					break;
				case 3:
					((OVRCALLBACK3)p_InitFunction)(args[0], args[1], args[2]);
					break;
				case 4:
					((OVRCALLBACK4)p_InitFunction)(args[0], args[1], args[2], args[3]);
					break;
				case 5:
					((OVRCALLBACK5)p_InitFunction)(args[0], args[1], args[2], args[3], args[4]);
					break;
				default:
					break;
			}
			m_IsInitFunctionExecuted = true;
		}
	}

	void ExecStopCallback()
	{
		if (p_StopFunction != nullptr)
		{
			std::vector<void*> args = m_StopFunctionArgs;
			switch (args.size())
			{
				case 0:
					p_StopFunction();
					break;
				case 1:
					((OVRCALLBACK1)p_StopFunction)(args[0]);
					break;
				case 2:
					((OVRCALLBACK2)p_StopFunction)(args[0], args[1]);
					break;
				case 3:
					((OVRCALLBACK3)p_StopFunction)(args[0], args[1], args[2]);
					break;
				case 4:
					((OVRCALLBACK4)p_StopFunction)(args[0], args[1], args[2], args[3]);
					break;
				case 5:
					((OVRCALLBACK5)p_StopFunction)(args[0], args[1], args[2], args[3], args[4]);
					break;
				default:
					break;
			}
		}
	}

	void ExecDrawCallback()
	{
		if (!m_IsInitFunctionExecuted) return;

		if (p_DrawFunction != nullptr)
		{
			std::vector<void*> args = m_DrawFunctionArgs;
			switch (args.size())
			{
				case 0:
					p_DrawFunction();
					break;
				case 1:
					((OVRCALLBACK1)p_DrawFunction)(args[0]);
					break;
				case 2:
					((OVRCALLBACK2)p_DrawFunction)(args[0], args[1]);
					break;
				case 3:
					((OVRCALLBACK3)p_DrawFunction)(args[0], args[1], args[2]);
					break;
				case 4:
					((OVRCALLBACK4)p_DrawFunction)(args[0], args[1], args[2], args[3]);
					break;
				case 5:
					((OVRCALLBACK5)p_DrawFunction)(args[0], args[1], args[2], args[3], args[4]);
					break;
				default:
					break;
			}
		}
	}

	void ExecIdleCallback()
	{
		if (!m_IsInitFunctionExecuted) return;

		if (p_IdleFunction != nullptr)
		{
			std::vector<void*> args = m_IdleFunctionArgs;
			switch (args.size())
			{
				case 0:
					p_IdleFunction();
					break;
				case 1:
					((OVRCALLBACK1)p_IdleFunction)(args[0]);
					break;
				case 2:
					((OVRCALLBACK2)p_IdleFunction)(args[0], args[1]);
					break;
				case 3:
					((OVRCALLBACK3)p_IdleFunction)(args[0], args[1], args[2]);
					break;
				case 4:
					((OVRCALLBACK4)p_IdleFunction)(args[0], args[1], args[2], args[3]);
					break;
				case 5:
					((OVRCALLBACK5)p_IdleFunction)(args[0], args[1], args[2], args[3], args[4]);
					break;
				default:
					break;
			}
		}
	}

	void MainThreadEX();
	static unsigned __stdcall MainThreadLauncherEX(void *obj);

	static void ErrorCallback(int err, const char* description)
	{
		std::cerr << description << std::endl;
	}

	static void MouseCursorPositionCallback
		(GLFWwindow* window, double xpos, double ypos)
	{
		OpenVR* instance = reinterpret_cast<OpenVR*>(glfwGetWindowUserPointer(window));
		if (instance != nullptr)
		{
		}
	}

	static void MouseButtonCallback
		(GLFWwindow* window, int button, int action, int mods)
	{
		OpenVR* instance = reinterpret_cast<OpenVR*>(glfwGetWindowUserPointer(window));
		if (instance != nullptr)
		{
		}
	}

	static void KeyCallback
		(GLFWwindow* window, int key, int scancode, int action, int mods)
	{
		OpenVR* instance = reinterpret_cast<OpenVR*>(glfwGetWindowUserPointer(window));
		if (instance != nullptr)
		{
			if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
			{
				glfwSetWindowShouldClose(window, GL_TRUE);
			}
			if (key == GLFW_KEY_C && action == GLFW_PRESS)
			{
#ifdef USE_OVRVISION
				instance->m_OVRVision.toggleCameraState();
#endif // USE_OVRVISION
			}
			if (key == GLFW_KEY_S && action == GLFW_PRESS)
			{
//				instance->GetSnap();
			}
		}
	}

	static void MouseWheelCallback
		(GLFWwindow* window, double xpos, double ypos)
	{
		OpenVR* instance = reinterpret_cast<OpenVR*>(glfwGetWindowUserPointer(window));
		if (instance != nullptr)
		{
			float SPEED  = 0.2f;
			float delta  = (float)ypos * SPEED;
			float xtrans = delta * instance->m_HeadVector[VECTOR_FRONT].x;
			float ytrans = delta * instance->m_HeadVector[VECTOR_FRONT].y;
			float ztrans = delta * instance->m_HeadVector[VECTOR_FRONT].z;
			instance->Translate(xtrans, ytrans, ztrans);

			static float prevtime = 0;

			float t = static_cast<float>(glfwGetTime());
			float dt = t - prevtime;
			if (fabs(xpos) > 0.2)
				instance->Rotate((float)(-xpos) * 90.0f * dt, 'y');

			prevtime = t;
		}
	}

	static void ResizeCallback
		(GLFWwindow* window, int width, int height)
	{
		OpenVR* instance = reinterpret_cast<OpenVR*>(glfwGetWindowUserPointer(window));
		if (instance != nullptr)
		{
			glViewport(0, 0, width, height);
			glMatrixMode(GL_PROJECTION);
			glLoadIdentity();
			gluPerspective(30.0, (GLdouble)width / (GLdouble)height, 0.001, 10000.0);
			glMatrixMode(GL_MODELVIEW);
		}
	}
protected:
	glm::mat4 ToGLM(vr::HmdMatrix44_t InMatrix);
	glm::mat4 ToGLM(vr::HmdMatrix34_t InMatrix);
	std::string GetHMDString(
		vr::TrackedDeviceIndex_t unDevice,
		vr::TrackedDeviceProperty prop, vr::TrackedPropertyError* peError);
};
