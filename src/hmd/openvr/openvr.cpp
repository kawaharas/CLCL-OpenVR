////////////////////////////////////////////////////////////////////////////////
//
// openvr.cpp
//
//   CLCL: CAVELib Compatible Library
//
//     Copyright 2015-2019 Shintaro Kawahara(kawahara@jamstec.go.jp).
//     All rights reserved.
//
//   Please read the file "LICENCE.txt" before you use this software.
//
////////////////////////////////////////////////////////////////////////////////

#include "openvr.h"

#ifdef ENABLE_CONTROLLER_MODEL

#if defined(_WIN32)
#define stricmp _stricmp
#define strnicmp _strnicmp
#endif

void ThreadSleep(unsigned long nMilliseconds)
{
#if defined(_WIN32)
	::Sleep(nMilliseconds);
#elif defined(POSIX)
	usleep(nMilliseconds * 1000);
#endif
}

CGLRenderModel::CGLRenderModel(const std::string & sRenderModelName)
	: m_sModelName(sRenderModelName)
{
	m_glIndexBuffer = 0;
	m_glVertArray = 0;
	m_glVertBuffer = 0;
	m_glTexture = 0;
}

CGLRenderModel::~CGLRenderModel()
{
	Cleanup();
}

bool CGLRenderModel::BInit(const vr::RenderModel_t & vrModel, const vr::RenderModel_TextureMap_t & vrDiffuseTexture)
{
	// create and bind a VAO to hold state for this model
	glGenVertexArrays(1, &m_glVertArray);
	glBindVertexArray(m_glVertArray);

	// Populate a vertex buffer
	glGenBuffers(1, &m_glVertBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, m_glVertBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vr::RenderModel_Vertex_t) * vrModel.unVertexCount, vrModel.rVertexData, GL_STATIC_DRAW);

	// Identify the components in the vertex buffer
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vr::RenderModel_Vertex_t), (void *)offsetof(vr::RenderModel_Vertex_t, vPosition));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(vr::RenderModel_Vertex_t), (void *)offsetof(vr::RenderModel_Vertex_t, vNormal));
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(vr::RenderModel_Vertex_t), (void *)offsetof(vr::RenderModel_Vertex_t, rfTextureCoord));

	// Create and populate the index buffer
	glGenBuffers(1, &m_glIndexBuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_glIndexBuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint16_t) * vrModel.unTriangleCount * 3, vrModel.rIndexData, GL_STATIC_DRAW);

	glBindVertexArray(0);

	// create and populate the texture
	glGenTextures(1, &m_glTexture);
	glBindTexture(GL_TEXTURE_2D, m_glTexture);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, vrDiffuseTexture.unWidth, vrDiffuseTexture.unHeight,
		0, GL_RGBA, GL_UNSIGNED_BYTE, vrDiffuseTexture.rubTextureMapData);

	// If this renders black ask McJohn what's wrong.
	glGenerateMipmap(GL_TEXTURE_2D);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

	GLfloat fLargest;
	glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &fLargest);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, fLargest);

	glBindTexture(GL_TEXTURE_2D, 0);

	m_unVertexCount = vrModel.unTriangleCount * 3;

	return true;
}

void CGLRenderModel::Cleanup()
{
	if (m_glVertBuffer)
	{
		glDeleteBuffers(1, &m_glIndexBuffer);
		glDeleteVertexArrays(1, &m_glVertArray);
		glDeleteBuffers(1, &m_glVertBuffer);
		m_glIndexBuffer = 0;
		m_glVertArray = 0;
		m_glVertBuffer = 0;
	}
}
void CGLRenderModel::Draw()
{
	glBindVertexArray(m_glVertArray);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_glTexture);
	glDrawElements(GL_TRIANGLES, m_unVertexCount, GL_UNSIGNED_SHORT, 0);
//	glBindTexture(GL_TEXTURE_2D, 0);
//	glActiveTexture(0);
	glBindVertexArray(0);
}

std::vector< CGLRenderModel * > m_vecRenderModels;

std::string GetTrackedDeviceString(vr::TrackedDeviceIndex_t unDevice, vr::TrackedDeviceProperty prop, vr::TrackedPropertyError *peError = NULL)
{
	uint32_t unRequiredBufferLen = vr::VRSystem()->GetStringTrackedDeviceProperty(unDevice, prop, NULL, 0, peError);
	if (unRequiredBufferLen == 0)
		return "";

	char *pchBuffer = new char[unRequiredBufferLen];
	unRequiredBufferLen = vr::VRSystem()->GetStringTrackedDeviceProperty(unDevice, prop, pchBuffer, unRequiredBufferLen, peError);
	std::string sResult = pchBuffer;
	delete[] pchBuffer;
	return sResult;
}

CGLRenderModel *FindOrLoadRenderModel(const char *pchRenderModelName)
{
	CGLRenderModel *pRenderModel = NULL;
	for (std::vector< CGLRenderModel * >::iterator i = m_vecRenderModels.begin(); i != m_vecRenderModels.end(); i++)
	{
		if (!stricmp((*i)->GetName().c_str(), pchRenderModelName))
		{
			pRenderModel = *i;
			break;
		}
	}

	if (!pRenderModel)
	{
		vr::RenderModel_t *pModel;
		vr::EVRRenderModelError error;
		while (1)
		{
			error = vr::VRRenderModels()->LoadRenderModel_Async(pchRenderModelName, &pModel);
			if (error != vr::VRRenderModelError_Loading) break;
			ThreadSleep(1);
		}
		if (error != vr::VRRenderModelError_None)
		{
			printf("Unable to load render model %s - %s\n", pchRenderModelName, vr::VRRenderModels()->GetRenderModelErrorNameFromEnum(error));
			return NULL; // move on to the next tracked device
		}

		vr::RenderModel_TextureMap_t *pTexture;
		while (1)
		{
			error = vr::VRRenderModels()->LoadTexture_Async(pModel->diffuseTextureId, &pTexture);
			if (error != vr::VRRenderModelError_Loading)
				break;

			ThreadSleep(1);
		}
		if (error != vr::VRRenderModelError_None)
		{
			printf("Unable to load render texture id:%d for render model %s\n", pModel->diffuseTextureId, pchRenderModelName);
			vr::VRRenderModels()->FreeRenderModel(pModel);
			return NULL; // move on to the next tracked device
		}

		pRenderModel = new CGLRenderModel(pchRenderModelName);
		if (!pRenderModel->BInit(*pModel, *pTexture))
		{
			printf("Unable to create GL model from render model %s\n", pchRenderModelName);
			delete pRenderModel;
			pRenderModel = NULL;
		}
		else
		{
			m_vecRenderModels.push_back(pRenderModel);
		}
		vr::VRRenderModels()->FreeRenderModel(pModel);
		vr::VRRenderModels()->FreeTexture(pTexture);
	}
	return pRenderModel;
}

bool OpenVR::CreateShader()
{
	m_unRenderModelProgramID = CompileGLShader(
		"render model",

		// vertex shader
		"#version 410\n"
		"uniform mat4 matrix;\n"
		"layout(location = 0) in vec4 position;\n"
		"layout(location = 1) in vec3 v3NormalIn;\n"
		"layout(location = 2) in vec2 v2TexCoordsIn;\n"
		"out vec2 v2TexCoord;\n"
		"void main()\n"
		"{\n"
		"	v2TexCoord = v2TexCoordsIn;\n"
		"	gl_Position = matrix * vec4(position.xyz, 1);\n"
		"}\n",

		//fragment shader
		"#version 410 core\n"
		"uniform sampler2D diffuse;\n"
		"in vec2 v2TexCoord;\n"
		"out vec4 outputColor;\n"
		"void main()\n"
		"{\n"
		"   outputColor = texture( diffuse, v2TexCoord);\n"
		"}\n"

	);
	m_nRenderModelMatrixLocation = glGetUniformLocation(m_unRenderModelProgramID, "matrix");
	if (m_nRenderModelMatrixLocation == -1)
	{
		printf("Unable to find matrix uniform in render model shader\n");
		return false;
	}

	return true;
}

GLuint OpenVR::CompileGLShader(const char *pchShaderName, const char *pchVertexShader, const char *pchFragmentShader)
{
	GLuint unProgramID = glCreateProgram();

	GLuint nSceneVertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(nSceneVertexShader, 1, &pchVertexShader, NULL);
	glCompileShader(nSceneVertexShader);

	GLint vShaderCompiled = GL_FALSE;
	glGetShaderiv(nSceneVertexShader, GL_COMPILE_STATUS, &vShaderCompiled);
	if (vShaderCompiled != GL_TRUE)
	{
		printf("%s - Unable to compile vertex shader %d!\n", pchShaderName, nSceneVertexShader);
		glDeleteProgram(unProgramID);
		glDeleteShader(nSceneVertexShader);
		return 0;
	}
	glAttachShader(unProgramID, nSceneVertexShader);
	glDeleteShader(nSceneVertexShader);

	GLuint  nSceneFragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(nSceneFragmentShader, 1, &pchFragmentShader, NULL);
	glCompileShader(nSceneFragmentShader);

	GLint fShaderCompiled = GL_FALSE;
	glGetShaderiv(nSceneFragmentShader, GL_COMPILE_STATUS, &fShaderCompiled);
	if (fShaderCompiled != GL_TRUE)
	{
		printf("%s - Unable to compile fragment shader %d!\n", pchShaderName, nSceneFragmentShader);
		glDeleteProgram(unProgramID);
		glDeleteShader(nSceneFragmentShader);
		return 0;
	}

	glAttachShader(unProgramID, nSceneFragmentShader);
	glDeleteShader(nSceneFragmentShader);

	glLinkProgram(unProgramID);

	GLint programSuccess = GL_TRUE;
	glGetProgramiv(unProgramID, GL_LINK_STATUS, &programSuccess);
	if (programSuccess != GL_TRUE)
	{
		printf("%s - Error linking program %d!\n", pchShaderName, unProgramID);
		glDeleteProgram(unProgramID);
		return 0;
	}

	glUseProgram(unProgramID);
	glUseProgram(0);

	return unProgramID;
}

void OpenVR::DrawController(int eyeIndex)
{
	if (!m_IsControllerModelLoaded) return;

	glUseProgram(m_unRenderModelProgramID);
	int eHand = 0; // reft:1, right:0
	glm::mat4 MVPMatrix = m_ProjectionMatrix[eyeIndex] * glm::inverse(m_EyePose[eyeIndex] * m_HeadPose) * m_HandPose;
	glUniformMatrix4fv(m_nRenderModelMatrixLocation, 1, GL_FALSE, &(MVPMatrix[0][0]));
	m_rHand[eHand].m_pRenderModel->Draw();
	glUseProgram(0);
}
#endif // ENABLE_CONTROLLER_MODEL

OpenVR::OpenVR()
{
	m_HmdSession = nullptr;

	m_NumEyes = 2;
	m_WindowHeight = 1080;
	m_NearPlaneZ   = 0.01f;
	m_FarPlaneZ    = 10000.0f;
	m_VerticalFieldOfView = static_cast<float>(45.0 * M_PI / 180.0);
	m_FrameBufferWidth  = 1920;
	m_FrameBufferHeight = 1080;

	m_BodyTranslation = glm::vec3(0.0f, 0.0f, 0.0f);
	m_BodyRotation = glm::vec3(0.0f, 0.0f, 0.0f);

	for (int i = 0; i < 3; i++)
	{
		m_HeadVector[i] = glm::vec3(0.0f, 0.0f, 0.0f);
	}
	m_HeadVector[VECTOR_FRONT] = glm::vec3(0.0f, 0.0f, -1.0f);

	for (int i = 0; i < m_NumEyes; i++)
	{
		m_FrameBuffer[i] = 0;
		m_TextureBuffer[i] = 0;
		m_DepthBuffer[i] = 0;
	}

	m_FrameIndex = 0;
//	m_SnapNo = 0;
	m_NavigationMatrix = glm::mat4(1.0f);
	m_ModelMatrix = glm::mat4(1.0f);

	for (int i = 0; i < 4; i++)
	{
		m_ButtonState[i] = -1;
	}

	m_IsThreadRunning = true;
	m_IsInitializedGLFW.store(false);
	m_HMutex = nullptr;
	m_HRender = nullptr;
	p_InitFunction = nullptr;
	p_StopFunction = nullptr;
	p_DrawFunction = nullptr;
	p_IdleFunction = nullptr;

	m_IsInitFunctionExecuted = false;

	m_DeviceType = HTC_VIVE;
	m_IsControllerConnected = false;

#ifdef ENABLE_CONTROLLER_MODEL
	m_IsControllerModelLoaded = false;
	m_IsControllerModelVisible = false;
#endif // ENABLE_CONTROLLER_MODEL
}

OpenVR::~OpenVR()
{
}

void OpenVR::Init()
{
	vr::EVRInitError eError = vr::VRInitError_None;
	m_HmdSession = vr::VR_Init(&eError, vr::VRApplication_Scene);

	if (eError != vr::VRInitError_None)
	{
		std::cerr << "ERROR: Could not initialize OpenVR: "
			<< vr::VR_GetVRInitErrorAsEnglishDescription(eError) << std::endl;
	}
	m_HmdSession->GetRecommendedRenderTargetSize(&m_FrameBufferWidth, &m_FrameBufferHeight);

	const std::string& driver = GetHMDString(vr::k_unTrackedDeviceIndex_Hmd, vr::Prop_TrackingSystemName_String, nullptr);
	const std::string& model  = GetHMDString(vr::k_unTrackedDeviceIndex_Hmd, vr::Prop_ModelNumber_String, nullptr);
	const std::string& serial = GetHMDString(vr::k_unTrackedDeviceIndex_Hmd, vr::Prop_SerialNumber_String, nullptr);
	const float freq = m_HmdSession->GetFloatTrackedDeviceProperty(vr::k_unTrackedDeviceIndex_Hmd, vr::Prop_DisplayFrequency_Float);
	fprintf(stderr, "HMD: %s '%s' #%s (%d x %d @ %g Hz)\n", driver.c_str(), model.c_str(), serial.c_str(), m_FrameBufferWidth, m_FrameBufferHeight, freq);

	if (model.find("Oculus") != std::string::npos)
	{
		m_DeviceType = OCULUS_RIFT_CV1;
	}
	else if (serial.find("WindowsHolographic") != std::string::npos)
	{
		m_DeviceType = WINDOWS_MR;
	}

	if (!vr::VRCompositor())
	{
		std::cerr << "OpenVR Compositor initialization failed. See log file for details\n";
		vr::VR_Shutdown();
		exit(EXIT_FAILURE);
	}

//	vr::VRCompositor()->SetTrackingSpace(vr::ETrackingUniverseOrigin::TrackingUniverseStanding);
	vr::VRCompositor()->SetTrackingSpace(vr::ETrackingUniverseOrigin::TrackingUniverseSeated);
}

void OpenVR::InitGL()
{
	m_WindowWidth = (m_FrameBufferWidth * m_WindowHeight) / m_FrameBufferHeight;

	glfwSetErrorCallback(ErrorCallback);
	if (!glfwInit())
	{
		exit(EXIT_FAILURE);
	}
	glfwWindowHint(GLFW_DECORATED, GL_FALSE);
	m_Window = glfwCreateWindow(
		m_WindowWidth, m_WindowHeight, "CLCL", NULL, NULL);
	if (!m_Window)
	{
		glfwTerminate();
		exit(EXIT_FAILURE);
	}

	glfwSetWindowUserPointer(m_Window, this); // technique for registering member functions as callback functions

	glfwMakeContextCurrent(m_Window);
	glfwSwapInterval(1);
	glfwSetKeyCallback(m_Window, KeyCallback);
	glfwSetMouseButtonCallback(m_Window, MouseButtonCallback);
	glfwSetCursorPosCallback(m_Window, MouseCursorPositionCallback);
	glfwSetScrollCallback(m_Window, MouseWheelCallback);
	glfwSetFramebufferSizeCallback(m_Window, ResizeCallback);
	glfwSetInputMode(m_Window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);

	glewExperimental = GL_TRUE;
	GLenum err = glewInit();
	if (err != GLEW_OK) {
		std::cout << "glewInit failed, aborting." << std::endl;
		exit(EXIT_FAILURE);
	}

#ifdef USE_OVRVISION
	m_OVRVision.Init();
//	m_OVRVision.toggleCameraState(); // change value from "false" to "true" (default: false)
#endif // USE_OVRVISION
}

std::string OpenVR::GetHMDString(
	vr::TrackedDeviceIndex_t unDevice,
	vr::TrackedDeviceProperty prop, vr::TrackedPropertyError* peError = nullptr)
{
	uint32_t unRequiredBufferLen = m_HmdSession->GetStringTrackedDeviceProperty(unDevice, prop, nullptr, 0, peError);
	if (unRequiredBufferLen == 0)
	{
		return "";
	}

	char* pchBuffer = new char[unRequiredBufferLen];
	unRequiredBufferLen = m_HmdSession->GetStringTrackedDeviceProperty(unDevice, prop, pchBuffer, unRequiredBufferLen, peError);
	std::string sResult = pchBuffer;
	delete[] pchBuffer;

	return sResult;
}

glm::mat4 OpenVR::ToGLM(vr::HmdMatrix44_t InMatrix)
{
	glm::mat4 OutMatrix(
		InMatrix.m[0][0], InMatrix.m[1][0], InMatrix.m[2][0], InMatrix.m[3][0],
		InMatrix.m[0][1], InMatrix.m[1][1], InMatrix.m[2][1], InMatrix.m[3][1],
		InMatrix.m[0][2], InMatrix.m[1][2], InMatrix.m[2][2], InMatrix.m[3][2],
		InMatrix.m[0][3], InMatrix.m[1][3], InMatrix.m[2][3], InMatrix.m[3][3]
	);
	return OutMatrix;
}

glm::mat4 OpenVR::ToGLM(vr::HmdMatrix34_t InMatrix)
{
	glm::mat4 OutMatrix(
		InMatrix.m[0][0], InMatrix.m[1][0], InMatrix.m[2][0], 0.0,
		InMatrix.m[0][1], InMatrix.m[1][1], InMatrix.m[2][1], 0.0,
		InMatrix.m[0][2], InMatrix.m[1][2], InMatrix.m[2][2], 0.0,
		InMatrix.m[0][3], InMatrix.m[1][3], InMatrix.m[2][3], 1.0f
	);
	return OutMatrix;
}

void OpenVR::CreateBuffers()
{
	for (int eyeIndex = 0; eyeIndex < m_NumEyes; eyeIndex++)
	{
		m_ProjectionMatrix[eyeIndex] = ToGLM(m_HmdSession->GetProjectionMatrix(vr::EVREye(eyeIndex), m_NearPlaneZ, m_FarPlaneZ)); // openvr 1.0.5
	}

	glGenFramebuffers(m_NumEyes, m_FrameBuffer);
	glGenTextures(m_NumEyes, m_TextureBuffer);
	glGenTextures(m_NumEyes, m_DepthBuffer);
	for (int eyeIndex = 0; eyeIndex < m_NumEyes; eyeIndex++)
	{
		glBindTexture(GL_TEXTURE_2D, m_TextureBuffer[eyeIndex]);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, m_FrameBufferWidth, m_FrameBufferHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);

		glBindTexture(GL_TEXTURE_2D, m_DepthBuffer[eyeIndex]);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, m_FrameBufferWidth, m_FrameBufferHeight, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_INT, nullptr);

		glBindFramebuffer(GL_FRAMEBUFFER, m_FrameBuffer[eyeIndex]);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_TextureBuffer[eyeIndex], 0);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_DepthBuffer[eyeIndex], 0);
	}
	glBindRenderbuffer(GL_RENDERBUFFER, 0);
	glBindTexture(GL_TEXTURE_2D, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
	{
		glDeleteFramebuffers(m_NumEyes, m_FrameBuffer);
		glDeleteTextures(m_NumEyes, m_TextureBuffer);
		glDeleteRenderbuffers(m_NumEyes, m_DepthBuffer);
		exit(EXIT_FAILURE);
	}

#ifdef ENABLE_CONTROLLER_MODEL
	if (!CreateShader())
		std::cout << "create shader failed." << std::endl;
#endif // ENABLE_CONTROLLER_MODEL
}

void OpenVR::Terminate()
{
	glDeleteFramebuffers(m_NumEyes, m_FrameBuffer);
	glDeleteTextures(2, m_TextureBuffer);
	glDeleteRenderbuffers(2, m_DepthBuffer);

	glfwDestroyWindow(m_Window);
	glfwTerminate();

	if (m_HmdSession != nullptr)
	{
		vr::VR_Shutdown();
	}
}

void OpenVR::UpdateTrackingData()
{
	m_FrameIndex++;

	vr::TrackedDevicePose_t trackedDevicePose[vr::k_unMaxTrackedDeviceCount];
	vr::VRCompositor()->WaitGetPoses(trackedDevicePose, vr::k_unMaxTrackedDeviceCount, nullptr, 0);

	for (int nDevice = 0; nDevice < vr::k_unMaxTrackedDeviceCount; ++nDevice)
	{
		if (trackedDevicePose[nDevice].bPoseIsValid)
		{
			switch (m_HmdSession->GetTrackedDeviceClass(nDevice))
			{
				case vr::TrackedDeviceClass_HMD:
					vr::HmdMatrix34_t headPose_SteamVR = trackedDevicePose[vr::k_unTrackedDeviceIndex_Hmd].mDeviceToAbsoluteTracking;
					m_HeadPose = ToGLM(headPose_SteamVR);
					glm::mat4 newMatrix = glm::transpose(glm::mat4(
						m_HeadPose[0][0], m_HeadPose[1][0], m_HeadPose[2][0], m_HeadPose[3][0] / FEET_PER_METER,
						m_HeadPose[0][1], m_HeadPose[1][1], m_HeadPose[2][1], m_HeadPose[3][1] / FEET_PER_METER,
						m_HeadPose[0][2], m_HeadPose[1][2], m_HeadPose[2][2], m_HeadPose[3][2] / FEET_PER_METER,
						m_HeadPose[0][3], m_HeadPose[1][3], m_HeadPose[2][3], m_HeadPose[3][3]));
					m_HeadTranslation = glm::vec3(
						m_HeadPose[3][0] / FEET_PER_METER,
						m_HeadPose[3][1] / FEET_PER_METER,
						m_HeadPose[3][2] / FEET_PER_METER);
					m_HeadVector[VECTOR_RIGHT] = glm::normalize(glm::vec3(newMatrix[0][0], newMatrix[0][1], newMatrix[0][2]));
					m_HeadVector[VECTOR_UP] = glm::normalize(glm::vec3(newMatrix[1][0], newMatrix[1][1], newMatrix[1][2]));
					m_HeadVector[VECTOR_FRONT] = glm::normalize(glm::vec3(-newMatrix[2][0], -newMatrix[2][1], -newMatrix[2][2]));
//					float angle_x, angle_y, angle_z;
//					finalRollPitchYaw.ToEulerAngles<OVR::Axis_Y, OVR::Axis_X, OVR::Axis_Z, OVR::Rotate_CCW, OVR::Handed_R>(&angle_x, &angle_y, &angle_z);
//					m_HeadOrientation = glm::vec3(angle_x, angle_y, angle_z);

					glm::mat4 navMatrix = glm::inverse(m_NavigationMatrix) * newMatrix;
					m_HeadTranslationNav = glm::vec3(navMatrix[3][0], navMatrix[3][1], navMatrix[3][2]);
					m_HeadVectorNav[VECTOR_RIGHT] = glm::normalize(glm::vec3( navMatrix[0][0],  navMatrix[0][1],  navMatrix[0][2]));
					m_HeadVectorNav[VECTOR_UP]    = glm::normalize(glm::vec3( navMatrix[1][0],  navMatrix[1][1],  navMatrix[1][2]));
					m_HeadVectorNav[VECTOR_FRONT] = glm::normalize(glm::vec3(-navMatrix[2][0], -navMatrix[2][1], -navMatrix[2][2]));
					break;
				case vr::TrackedDeviceClass_Controller:
					vr::ETrackedControllerRole trackedControllerRoll;
					trackedControllerRoll = vr::VRSystem()->GetControllerRoleForTrackedDeviceIndex(nDevice);
					if (trackedControllerRoll == vr::TrackedControllerRole_RightHand)
					{
						vr::VRSystem()->GetControllerState(nDevice, &m_ControllerState, sizeof(vr::VRControllerState_t));

						float offset_angle = 0.0;
						if (m_DeviceType == OCULUS_RIFT_CV1)
						{
							offset_angle = static_cast<float>(-30.0 / 180.0 * M_PI);
						}
						m_HandPose = ToGLM(trackedDevicePose[nDevice].mDeviceToAbsoluteTracking);
						glm::mat4 newMatrix = glm::rotate(glm::transpose(glm::mat4(
							m_HandPose[0][0], m_HandPose[1][0], m_HandPose[2][0], m_HandPose[3][0] / FEET_PER_METER,
							m_HandPose[0][1], m_HandPose[1][1], m_HandPose[2][1], m_HandPose[3][1] / FEET_PER_METER,
							m_HandPose[0][2], m_HandPose[1][2], m_HandPose[2][2], m_HandPose[3][2] / FEET_PER_METER,
							m_HandPose[0][3], m_HandPose[1][3], m_HandPose[2][3], m_HandPose[3][3])),
							offset_angle, glm::vec3(1.0f, 0.0f, 0.0f));
						m_HandTranslation = glm::vec3(
							m_HandPose[3][0] / FEET_PER_METER,
							m_HandPose[3][1] / FEET_PER_METER,
							m_HandPose[3][2] / FEET_PER_METER);
						m_HandVector[VECTOR_RIGHT] = glm::normalize(glm::vec3( newMatrix[0][0],  newMatrix[0][1],  newMatrix[0][2]));
						m_HandVector[VECTOR_UP]    = glm::normalize(glm::vec3( newMatrix[1][0],  newMatrix[1][1],  newMatrix[1][2]));
						m_HandVector[VECTOR_FRONT] = glm::normalize(glm::vec3(-newMatrix[2][0], -newMatrix[2][1], -newMatrix[2][2]));
						glm::mat4 navMatrix = glm::inverse(m_NavigationMatrix) * newMatrix;
						m_HandTranslationNav = glm::vec3(navMatrix[3][0], navMatrix[3][1], navMatrix[3][2]);
						m_HandVectorNav[VECTOR_RIGHT] = glm::normalize(glm::vec3( navMatrix[0][0],  navMatrix[0][1],  navMatrix[0][2]));
						m_HandVectorNav[VECTOR_UP]    = glm::normalize(glm::vec3( navMatrix[1][0],  navMatrix[1][1],  navMatrix[1][2]));
						m_HandVectorNav[VECTOR_FRONT] = glm::normalize(glm::vec3(-navMatrix[2][0], -navMatrix[2][1], -navMatrix[2][2]));

						m_IsControllerConnected = true;

#ifdef ENABLE_CONTROLLER_MODEL
						if (!m_IsControllerModelLoaded)
						{
							const std::string& renderModelName = GetHMDString(
								nDevice, vr::Prop_RenderModelName_String, nullptr);
							int eHand = 0; // reft:1, right:0
							m_rHand[eHand].m_pRenderModel = FindOrLoadRenderModel(renderModelName.c_str());
							if (m_rHand[eHand].m_pRenderModel == NULL)
								std::cout << "ERROR: m_rHand[eHand].m_pRenderModel = NULL" << std::endl;
							m_rHand[eHand].m_sRenderModelName = renderModelName;
							m_IsControllerModelLoaded = true;
						}
#endif // ENABLE_CONTROLLER_MODEL
					}
					break;
				default:
					break;
			}
		}
	}
}

void OpenVR::PreProcess()
{
	for (int eyeIndex = 0; eyeIndex < m_NumEyes; eyeIndex++)
	{
		m_EyePose[eyeIndex] = ToGLM(m_HmdSession->GetEyeToHeadTransform(vr::EVREye(eyeIndex)));
	}

#ifdef USE_OVRVISION
	m_OVRVision.PreStore();
#endif // USE_OVRVISION
}

void OpenVR::SubmitFrame(int eyeIndex)
{
//	const vr::Texture_t tex = { reinterpret_cast<void*>(intptr_t(m_TextureBuffer[eyeIndex])), vr::API_OpenGL, vr::ColorSpace_Gamma }; // openvr 1.0.3
	const vr::Texture_t tex = { reinterpret_cast<void*>(intptr_t(m_TextureBuffer[eyeIndex])), vr::TextureType_OpenGL, vr::ColorSpace_Gamma }; // openvr 1.0.5
	vr::VRCompositor()->Submit(vr::EVREye(eyeIndex), &tex);
}

void OpenVR::PostProcess()
{
	vr::VRCompositor()->PostPresentHandoff();

	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, GL_NONE);
	glViewport(0, 0, m_WindowWidth, m_WindowHeight);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glBlitFramebuffer(0, 0, m_FrameBufferWidth, m_FrameBufferHeight, 0, 0, m_WindowWidth, m_WindowHeight, GL_COLOR_BUFFER_BIT, GL_LINEAR);
	glBindFramebuffer(GL_READ_FRAMEBUFFER, GL_NONE);

	glfwSwapBuffers(m_Window);
	glfwPollEvents();
}

void OpenVR::SetMatrix(int eyeIndex)
{
	m_CurrentEyeIndex = eyeIndex;

	glBindFramebuffer(GL_FRAMEBUFFER, m_FrameBuffer[eyeIndex]);
	glViewport(0, 0, m_FrameBufferWidth, m_FrameBufferHeight);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

#ifdef USE_OVRVISION
	m_OVRVision.DrawImege(eyeIndex);
#endif // USE_OVRVISION

	glEnable(GL_DEPTH_TEST);

	glUseProgram(0);
	glMatrixMode(GL_PROJECTION);
	glLoadMatrixf(&(m_ProjectionMatrix[eyeIndex][0][0]));
	glMatrixMode(GL_MODELVIEW);
	glLoadMatrixf(&(glm::inverse(m_EyePose[eyeIndex] * m_HeadPose))[0][0]);
}

void OpenVR::Translate(float x, float y, float z)
{
	glm::mat4 currentMatrix = m_NavigationMatrix;
	m_NavigationMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(-x, -y, -z)) * currentMatrix;
}

void OpenVR::Rotate(float angle_degree, char axis)
{
	float angle_radian = -angle_degree * (float)M_PI / 180.0f;
	glm::mat4 currentMatrix = m_NavigationMatrix;
	switch (tolower(axis))
	{
		case 'x':
			m_NavigationMatrix = glm::rotate(glm::mat4(1.0f), angle_radian, glm::vec3(1.0f, 0.0f, 0.0f)) * currentMatrix;
			break;
		case 'y':
			m_NavigationMatrix = glm::rotate(glm::mat4(1.0f), angle_radian, glm::vec3(0.0f, 1.0f, 0.0f)) * currentMatrix;
			break;
		case 'z':
			m_NavigationMatrix = glm::rotate(glm::mat4(1.0f), angle_radian, glm::vec3(0.0f, 0.0f, 1.0f)) * currentMatrix;
			break;
		default:
			break;
	}
}

void OpenVR::Scale(float x, float y, float z)
{
	glm::mat4 currentMatrix = m_NavigationMatrix;
	m_NavigationMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(1.0f / x, 1.0f / y, 1.0f / z)) * currentMatrix;
}

void OpenVR::WorldTranslate(float x, float y, float z)
{
	m_NavigationMatrix = glm::translate(m_NavigationMatrix, glm::vec3(-x, -y, -z));
}

void OpenVR::WorldRotate(float angle_degree, char axis)
{
	float angle_radian = -angle_degree * (float)M_PI / 180.0f;
	switch (tolower(axis))
	{
		case 'x':
			m_NavigationMatrix = glm::rotate(m_NavigationMatrix, angle_radian, glm::vec3(1.0f, 0.0f, 0.0f));
			break;
		case 'y':
			m_NavigationMatrix = glm::rotate(m_NavigationMatrix, angle_radian, glm::vec3(0.0f, 1.0f, 0.0f));
			break;
		case 'z':
			m_NavigationMatrix = glm::rotate(m_NavigationMatrix, angle_radian, glm::vec3(0.0f, 0.0f, 1.0f));
			break;
		default:
			break;
	}
}

void OpenVR::WorldScale(float x, float y, float z)
{
	m_NavigationMatrix *= glm::scale(glm::mat4(1.0f), glm::vec3(1.0f / x, 1.0f / y, 1.0f / z));
}

glm::mat4 OpenVR::GetNavigationMatrix()
{
	return m_NavigationMatrix;
}

void OpenVR::LoadNavigationMatrix(glm::mat4 matrix)
{
	m_NavigationMatrix = matrix;
}

void OpenVR::SetNavigationMatrix()
{
	glMatrixMode(GL_MODELVIEW);
	glMultMatrixf(&m_NavigationMatrix[0][0]);
}

void OpenVR::SetNavigationInverseMatrix()
{
	glMatrixMode(GL_MODELVIEW);
	glMultMatrixf(&(glm::inverse(m_NavigationMatrix))[0][0]);
}

void OpenVR::SetNavigationMatrixIdentity()
{
	m_NavigationMatrix = glm::mat4(1.0f);
}

void OpenVR::MultiNavigationMatrix(float matrix[4][4])
{
	glm::mat4 InMatrix = glm::mat4(
		matrix[0][0], matrix[1][0], matrix[2][0], matrix[3][0],
		matrix[0][1], matrix[1][1], matrix[2][1], matrix[3][1],
		matrix[0][2], matrix[1][2], matrix[2][2], matrix[3][2],
		matrix[0][3], matrix[1][3], matrix[2][3], matrix[3][3]);
	glm::mat4 currentMatrix = m_NavigationMatrix;
	m_NavigationMatrix = InMatrix * currentMatrix;
}

void OpenVR::PreMultiNavigationMatrix(float matrix[4][4])
{
	glm::mat4 InMatrix = glm::mat4(
		matrix[0][0], matrix[1][0], matrix[2][0], matrix[3][0],
		matrix[0][1], matrix[1][1], matrix[2][1], matrix[3][1],
		matrix[0][2], matrix[1][2], matrix[2][2], matrix[3][2],
		matrix[0][3], matrix[1][3], matrix[2][3], matrix[3][3]);
	m_NavigationMatrix *= InMatrix;
}

bool OpenVR::GetKey(int key)
{
	OpenVR* instance = reinterpret_cast<OpenVR*>(glfwGetWindowUserPointer(m_Window));
	bool result = false;
	if (instance != nullptr)
	{
		result = glfwGetKey(m_Window, key);
	}
	return result;
}

int OpenVR::GetMouseButton(int button)
{
	OpenVR* instance = reinterpret_cast<OpenVR*>(glfwGetWindowUserPointer(m_Window));
	int result = GLFW_RELEASE;
	if (instance != nullptr)
	{
		result = glfwGetMouseButton(m_Window, button);
	}
	return result;
}

void OpenVR::StartThread()
{
	DWORD m_MainThreadID = GetCurrentThreadId();

	m_HMutex = CreateMutex(NULL, FALSE, NULL);
	m_HRender = (HANDLE)_beginthreadex(0, 0, MainThreadLauncherEX, reinterpret_cast<void*>(this), 0, 0);

	while (!m_IsInitializedGLFW.load()); // waiting the initialization of GLFW
}

void OpenVR::StopThread()
{
	CloseHandle(m_HRender);
	CloseHandle(m_HMutex);
}

void OpenVR::MainThreadEX()
{
	DWORD m_DisplayThreadID = GetCurrentThreadId();

	Init();
	InitGL();
	CreateBuffers();

	m_IsInitializedGLFW.store(true);

	while (m_IsThreadRunning)
	{
		ExecInitCallback();
		UpdateTrackingData();
		ExecIdleCallback();
		PreProcess();
		for (int eyeIndex = 0; eyeIndex < m_NumEyes; eyeIndex++)
		{
			SetMatrix(eyeIndex);
			glPushMatrix();
			glScalef(FEET_PER_METER, FEET_PER_METER, FEET_PER_METER);
			ExecDrawCallback();
			glPopMatrix();

#ifdef ENABLE_CONTROLLER_MODEL
			DrawController(eyeIndex);
#endif // ENABLE_CONTROLLER_MODEL 

			SubmitFrame(eyeIndex);
		}
		PostProcess();
	}

	ExecStopCallback();
	Terminate();
}

unsigned __stdcall OpenVR::MainThreadLauncherEX(void *obj)
{
	reinterpret_cast<OpenVR*>(obj)->MainThreadEX();
	_endthreadex(0);
	return 0;
}

bool OpenVR::IsMainThread()
{
	if (GetCurrentThreadId() == m_MainThreadID)
	{
		return true;
	}
	else
	{
		return false;
	}
}

bool OpenVR::IsDisplayThread()
{
	if (GetCurrentThreadId() == m_DisplayThreadID)
	{
		return true;
	}
	else
	{
		return false;
	}
}

int OpenVR::GetButtonState(int buttonNumber)
{
	int state = -1;

	uint64_t buttonState = m_ControllerState.ulButtonPressed;
	uint64_t buttonMask;

	switch (buttonNumber)
	{
		case 1:
			if (m_DeviceType == OCULUS_RIFT_CV1)
			{
				buttonMask = vr::ButtonMaskFromId(vr::EVRButtonId::k_EButton_A); // for Oculus
			}
			else
			{
				buttonMask = vr::ButtonMaskFromId(vr::EVRButtonId::k_EButton_Grip); // for HTC VIVE (and Microsoft Mixed Reality)
			}
			if (buttonState & buttonMask) state = 1;
			else state = 0;
			break;
		case 2:
			if (m_ControllerState.rAxis[1].x > 0.5f) state = 1;
			else state = 0;
			break;
		case 3:
			buttonMask = vr::ButtonMaskFromId(vr::EVRButtonId::k_EButton_ApplicationMenu);
			if (buttonState & buttonMask) state = 1;
			else state = 0;
			break;
		default:
			break;
	}

	return state;
}

std::pair<float, float> OpenVR::GetJoyStickValue()
{
	if (m_DeviceType == OCULUS_RIFT_CV1)
	{
		return std::pair<float, float>(m_ControllerState.rAxis[0].x, m_ControllerState.rAxis[0].y);
	}
	else
	{
		if (!(m_ControllerState.ulButtonPressed &
			(vr::ButtonMaskFromId(vr::EVRButtonId::k_EButton_Axis0))))
		{
			if (m_ControllerState.ulButtonTouched &
				vr::ButtonMaskFromId(vr::EVRButtonId::k_EButton_SteamVR_Touchpad))
			{
				return std::pair<float, float>(m_ControllerState.rAxis[0].x, m_ControllerState.rAxis[0].y);
			}
		}
	}

	return std::pair<float, float>(0.0f, 0.0f);
}
