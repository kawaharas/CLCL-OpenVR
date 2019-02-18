////////////////////////////////////////////////////////////////////////////////
//
// clcl.cpp
//
//   CLCL: CAVELib Compatible Library
//
//     Copyright 2015-2019 Shintaro Kawahara(kawahara@jamstec.go.jp).
//     All rights reserved.
//
//   Please read the file "LICENCE.txt" before you use this software.
//
////////////////////////////////////////////////////////////////////////////////

#include "hmd/openvr/openvr.h"

#include "clcl.h"

CLCL *p_CLCL = nullptr;

const int CONTROLLER_BUTTON1 = GLFW_MOUSE_BUTTON_LEFT;
const int CONTROLLER_BUTTON2 = GLFW_MOUSE_BUTTON_MIDDLE;
const int CONTROLLER_BUTTON3 = GLFW_MOUSE_BUTTON_RIGHT;
const int CONTROLLER_BUTTON4 = GLFW_MOUSE_BUTTON_4;

class CLCL::Impl
{
public:
	OpenVR* p_HMD;
	int   m_ButtonState[4];

	bool   m_IsThreadRunning;

	OpenVR* hmd() { return p_HMD; }
	llong frameIndex() { return p_HMD->frameIndex(); }
	int   buttonState(int button) { return m_ButtonState[button - 1]; }
	void  SetButtonState(int button, int state) { m_ButtonState[button - 1] = state; }

	void  StartThread();
	void  StopThread();

	void  SetInitFunc(CAVECALLBACK callback, std::vector<void *> arg_list);
	void  SetStopFunc(CAVECALLBACK callback, std::vector<void *> arg_list);
	void  SetDrawFunc(CAVECALLBACK callback, std::vector<void *> arg_list);
	void  SetIdleFunc(CAVECALLBACK callback, std::vector<void *> arg_list);
};

float CAVENear = 0.1f;
float CAVEFar  = 100.0f;
CAVE_SYNC *CAVESync;

bool CAVEMasterWall() { return true; }
bool CAVEMasterDisplay() { return true; }
bool CAVEDistribMaster() { return true; }
void CAVEDisplayBarrier() {}
int  CAVEUniqueIndex() { return 0; }
int  CAVENumPipes() { return 1; }
int  CAVEDistribNumNodes() { return 1; }

void CAVEGetWindowGeometry(int *origX, int *origY, int *width, int *height)
{
	*origX  = 0;
	*origY  = 0;
	*width  = p_CLCL->p_Impl->hmd()->renderTargetWidth();
	*height = p_CLCL->p_Impl->hmd()->renderTargetHeight();
}

void CAVENavGetMatrix(float matrix[4][4])
{
	glm::mat4 tmpMat4 = p_CLCL->p_Impl->hmd()->GetNavigationMatrix();
	for (int j = 0; j < 4; j++)
	{
		for (int i = 0; i < 4; i++)
		{
			matrix[j][i] = tmpMat4[j][i];
		}
	}
}

void CAVENavLoadIdentity()
{
	p_CLCL->p_Impl->hmd()->SetNavigationMatrixIdentity();
}

void CAVENavLoadMatrix(float matrix[4][4])
{
	glm::mat4 tmpMat4;
	for (int j = 0; j < 4; j++)
	{
		for (int i = 0; i < 4; i++)
		{
			tmpMat4[j][i] = matrix[j][i];
		}
	}
	p_CLCL->p_Impl->hmd()->LoadNavigationMatrix(tmpMat4);
}

void CAVENavTranslate(float xtrans, float ytrans, float ztrans)
{
//	if (p_CLCL->navLockState()) return;
	p_CLCL->p_Impl->hmd()->Translate(xtrans, ytrans, ztrans);
}

void CAVENavRot(float angle, char axis)
{
//	if (p_CLCL->navLockState()) return;
	p_CLCL->p_Impl->hmd()->Rotate(angle, axis);
}

void CAVENavScale(float xscale, float yscale, float zscale)
{
//	if (p_CLCL->navLockState()) return;
	p_CLCL->p_Impl->hmd()->Scale(xscale, yscale, zscale);
}

void CAVENavWorldTranslate(float xtrans, float ytrans, float ztrans)
{
//	if (p_CLCL->navLockState()) return;
	p_CLCL->p_Impl->hmd()->WorldTranslate(xtrans, ytrans, ztrans);
}

void CAVENavWorldRot(float angle, char axis)
{
//	if (p_CLCL->navLockState()) return;
	p_CLCL->p_Impl->hmd()->WorldRotate(angle, axis);
}

void CAVENavWorldScale(float xscale, float yscale, float zscale)
{
//	if (p_CLCL->navLockState()) return;
	p_CLCL->p_Impl->hmd()->WorldScale(xscale, yscale, zscale);
}

void CAVENavTransform()
{
	p_CLCL->p_Impl->hmd()->SetNavigationMatrix();
}

void CAVENavInverseTransform()
{
	p_CLCL->p_Impl->hmd()->SetNavigationInverseMatrix();
}

void CAVENavMultMatrix(float matrix[4][4])
{
	p_CLCL->p_Impl->hmd()->MultiNavigationMatrix(matrix);
}

void CAVENavPreMultMatrix(float matrix[4][4])
{
	p_CLCL->p_Impl->hmd()->PreMultiNavigationMatrix(matrix);
}

bool CAVEgetbutton(CAVEDevice device)
{
	if (p_CLCL->p_Impl->hmd()->GetKey(device))
	{
		return true;
	}
	else
	{
		return false;
	}
}

void CAVEGetPosition(CAVEID id, float position[3])
{
	bool isTouch = false;
	switch (id)
	{
		case CAVE_HEAD:
			break;
		case CAVE_HEAD_NAV:
			break;
		case CAVE_WAND:
			isTouch = true;
			break;
		case CAVE_WAND_NAV:
			isTouch = true;
			break;
		case CAVE_LEFT_EYE:
			break;
		case CAVE_RIGHT_EYE:
			break;
		case CAVE_LEFT_EYE_NAV:
			break;
		case CAVE_RIGHT_EYE_NAV:
			break;
		default:
			break;
	}

	glm::vec3 headTranslation, headTranslationNav;
	headTranslation = p_CLCL->p_Impl->hmd()->headTranslation();
	headTranslationNav = p_CLCL->p_Impl->hmd()->headTranslationNav();
	if (id == CAVE_HEAD)
	{
		position[0] =  headTranslation.x;
		position[1] =  headTranslation.y;
		position[2] =  headTranslation.z;
		return;
	}
	else if (id == CAVE_HEAD_NAV)
	{
		position[0] =  headTranslationNav.x;
		position[1] =  headTranslationNav.y;
		position[2] =  headTranslationNav.z;
		return;
	}

	if (p_CLCL->p_Impl->hmd()->IsControllerConnected())
	{
		glm::vec3 handTranslation, handTranslationNav;
		handTranslation = p_CLCL->p_Impl->hmd()->handTranslation();
		handTranslationNav = p_CLCL->p_Impl->hmd()->handTranslationNav();
		if (id == CAVE_WAND)
		{
			position[0] = handTranslation.x;
			position[1] = handTranslation.y;
			position[2] = handTranslation.z;
			return;
		}
		else if (id == CAVE_WAND_NAV)
		{
			position[0] = handTranslationNav.x;
			position[1] = handTranslationNav.y;
			position[2] = handTranslationNav.z;
			return;
		}
	}
	else
	{
		if (id == CAVE_WAND)
		{
			position[0] = headTranslation.x;
			position[1] = headTranslation.y;
			position[2] = headTranslation.z;
			return;
		}
		else if (id == CAVE_WAND_NAV)
		{
			position[0] = headTranslationNav.x;
			position[1] = headTranslationNav.y;
			position[2] = headTranslationNav.z;
			return;
		}
	}
}

void CAVEGetVector(CAVEID id, float vector[3])
{
	bool isWand = false;
	VECTOR_TYPE type = VECTOR_FRONT;
	switch (id)
	{
		case CAVE_HEAD_FRONT_NAV:
			type = VECTOR_FRONT;
			break;
		case CAVE_HEAD_UP_NAV:
			type = VECTOR_UP;
			break;
		case CAVE_HEAD_RIGHT_NAV:
			type = VECTOR_RIGHT;
			break;
		case CAVE_HEAD_FRONT:
			type = VECTOR_FRONT;
			break;
		case CAVE_HEAD_UP:
			type = VECTOR_UP;
			break;
		case CAVE_HEAD_RIGHT:
			type = VECTOR_RIGHT;
			break;
		case CAVE_WAND_FRONT_NAV:
			isWand = true;
			type = VECTOR_FRONT;
			break;
		case CAVE_WAND_UP_NAV:
			isWand = true;
			type = VECTOR_UP;
			break;
		case CAVE_WAND_RIGHT_NAV:
			isWand = true;
			type = VECTOR_RIGHT;
			break;
		case CAVE_WAND_FRONT:
			isWand = true;
			type = VECTOR_FRONT;
			break;
		case CAVE_WAND_UP:
			isWand = true;
			type = VECTOR_UP;
			break;
		case CAVE_WAND_RIGHT:
			isWand = true;
			type = VECTOR_RIGHT;
			break;
		default:
			break;
	}

	if ((id == CAVE_HEAD_FRONT) || (id == CAVE_HEAD_UP) || (id == CAVE_HEAD_RIGHT))
	{
		glm::vec3 headVector;
		headVector = p_CLCL->p_Impl->hmd()->headVector(type);
		vector[0] = headVector.x;
		vector[1] = headVector.y;
		vector[2] = headVector.z;
		return;
	}
	else if ((id == CAVE_HEAD_FRONT_NAV) || (id == CAVE_HEAD_UP_NAV) || (id == CAVE_HEAD_RIGHT_NAV))
	{
		glm::vec3 headVector;
		headVector = p_CLCL->p_Impl->hmd()->headVectorNav(type);
		vector[0] = headVector.x;
		vector[1] = headVector.y;
		vector[2] = headVector.z;
		return;
	}
	else if ((id == CAVE_WAND_FRONT) || (id == CAVE_WAND_UP) || (id == CAVE_WAND_RIGHT))
	{
		if (p_CLCL->p_Impl->hmd()->IsControllerConnected())
		{
			glm::vec3 tmpVector;
			tmpVector = p_CLCL->p_Impl->hmd()->handVector(type);
			vector[0] = tmpVector.x;
			vector[1] = tmpVector.y;
			vector[2] = tmpVector.z;
		}
		else
		{
			glm::vec3 headVector;
			headVector = p_CLCL->p_Impl->hmd()->headVector(type);
			vector[0] = headVector.x;
			vector[1] = headVector.y;
			vector[2] = headVector.z;
		}
		return;
	}
	else if ((id == CAVE_WAND_FRONT_NAV) || (id == CAVE_WAND_UP_NAV) || (id == CAVE_WAND_RIGHT_NAV))
	{
		if (p_CLCL->p_Impl->hmd()->IsControllerConnected())
		{
			vector[0] = p_CLCL->p_Impl->hmd()->handVectorNav(type).x;
			vector[1] = p_CLCL->p_Impl->hmd()->handVectorNav(type).y;
			vector[2] = p_CLCL->p_Impl->hmd()->handVectorNav(type).z;
		}
		else
		{
			glm::vec3 headVector;
			headVector = p_CLCL->p_Impl->hmd()->headVectorNav(type);
			vector[0] = headVector.x;
			vector[1] = headVector.y;
			vector[2] = headVector.z;
		}
		return;
	}
}

void  CAVEGetOrientation(CAVEID id, float angle[3])
{
	// not implemented yet
	bool isTouch = false;
	switch (id)
	{
		case CAVE_HEAD:
			break;
		case CAVE_HEAD_NAV:
			break;
		case CAVE_WAND:
			isTouch = true;
			break;
		case CAVE_WAND_NAV:
			isTouch = true;
			break;
		case CAVE_LEFT_EYE:
			break;
		case CAVE_RIGHT_EYE:
			break;
		case CAVE_LEFT_EYE_NAV:
			break;
		case CAVE_RIGHT_EYE_NAV:
			break;
		default:
			break;
	}

	glm::vec3 headOrientation, headOrientationNav;
	headOrientation = p_CLCL->p_Impl->hmd()->headOrientation();
	headOrientationNav = p_CLCL->p_Impl->hmd()->headOrientationNav();
	if (id == CAVE_HEAD)
	{
		angle[0] = headOrientation.x;
		angle[1] = headOrientation.y;
		angle[2] = headOrientation.z;
		return;
	}
	else if (id == CAVE_HEAD_NAV)
	{
		angle[0] = headOrientationNav.x;
		angle[1] = headOrientationNav.y;
		angle[2] = headOrientationNav.z;
		return;
	}
	return; // for test
}

void CAVESetOption(CAVEID option, int value)
{
	// not implemented yet
	switch (option)
	{
		case CAVE_SHMEM_SIZE:
			break;
		default:
			break;
	}
}

float CAVEGetTime()
{
	return static_cast<float>(glfwGetTime());
}

int CAVEButtonChange(int buttonNumber)
{
	//  "0" indicates the button has not changed
	//  "1" indicates the button has been pressed
	// "-1" indicates the button has been released

	GLFWwindow* window = p_CLCL->p_Impl->hmd()->window();
	int state = -1;

	if (p_CLCL->p_Impl->hmd()->IsControllerConnected())
	{
		// if controller is connected
		state = p_CLCL->p_Impl->hmd()->GetButtonState(buttonNumber);
	}
	else
	{
		switch (buttonNumber)
		{
			case 1:
				state = p_CLCL->p_Impl->hmd()->GetMouseButton(CONTROLLER_BUTTON1);
				break;
			case 2:
				state = p_CLCL->p_Impl->hmd()->GetMouseButton(CONTROLLER_BUTTON2);
				break;
			case 3:
				state = p_CLCL->p_Impl->hmd()->GetMouseButton(CONTROLLER_BUTTON3);
				break;
			case 4:
				state = p_CLCL->p_Impl->hmd()->GetMouseButton(CONTROLLER_BUTTON4);
				break;
			default:
				break;
		}
	}

	if (p_CLCL->p_Impl->buttonState(buttonNumber) == state)
	{
		return 0;
	}
	else
	{
		p_CLCL->p_Impl->SetButtonState(buttonNumber, state);
		if (state == GLFW_PRESS) return 1;
		else if (state == GLFW_RELEASE) return -1;
	}

	return 0;
}

void CAVEUSleep(unsigned long milliseconds)
{
	Sleep(milliseconds);
}

bool IsButtonPressed(const int button)
{
	if (p_CLCL->p_Impl->hmd()->IsControllerConnected())
	{
		vr::VRControllerState_t controllerState;
		controllerState = p_CLCL->p_Impl->hmd()->controllerState();
		uint64_t buttonState = controllerState.ulButtonPressed;
		uint64_t buttonMask;

		switch (button)
		{
			case CONTROLLER_BUTTON1:
				if (p_CLCL->p_Impl->hmd()->GetDeviceType() == OCULUS_RIFT_CV1)
				{
					buttonMask = vr::ButtonMaskFromId(vr::EVRButtonId::k_EButton_A); // for Oculus
				}
				else
				{
					buttonMask = vr::ButtonMaskFromId(vr::EVRButtonId::k_EButton_Grip); // for HTC VIVE
				}
				if (buttonState & buttonMask) return true;
				break;
			case CONTROLLER_BUTTON2:
				if (controllerState.rAxis[1].x > 0.5f) return true;
				break;
			case CONTROLLER_BUTTON3:
				buttonMask = vr::ButtonMaskFromId(vr::EVRButtonId::k_EButton_ApplicationMenu);
				if (buttonState & buttonMask) return true;
				break;
			case CONTROLLER_BUTTON4:
				// not implemented
				break;
			default:
				break;
		}
	}
	else
	{
		int state = p_CLCL->p_Impl->hmd()->GetMouseButton(button);
		if (state == GLFW_PRESS)
		{
			return true;
		}
	}

	return false;
}

std::pair<float, float> GetJoyStickValue(JOYSTICK_TYPE type)
{
	return p_CLCL->p_Impl->hmd()->GetJoyStickValue();
}

void CAVEConfigure(int *argc, char **argv, char **appdefaults)
{
	p_CLCL = new CLCL();
	CAVESync = new CAVE_SYNC;
	CAVESync->Initted = false;
	CAVESync->Quit = false;
}

void CAVEInit()
{
	p_CLCL->p_Impl->StartThread();
}

void CAVEExit()
{
	if (p_CLCL != nullptr)
	{
		p_CLCL->p_Impl->StopThread();
		delete p_CLCL;
	}
}

void CAVEHalt()
{
	CAVEExit();
}

void CAVEInitApplication(CAVECALLBACK callback, int arg_num, ...)
{
	std::vector<void*> arg_list;
	va_list list;
	va_start(list, arg_num);
	for (int i = 0; i < arg_num; i++)
	{
		arg_list.push_back(va_arg(list, void*));
	}
	va_end(list);

	p_CLCL->p_Impl->SetInitFunc(callback, arg_list);
}

void CAVEStopApplication(CAVECALLBACK callback, int arg_num, ...)
{
	std::vector<void*> arg_list;
	va_list list;
	va_start(list, arg_num);
	for (int i = 0; i < arg_num; i++)
	{
		arg_list.push_back(va_arg(list, void*));
	}
	va_end(list);

	p_CLCL->p_Impl->SetStopFunc(callback, arg_list);
}

void CAVEDisplay(CAVECALLBACK callback, int arg_num, ...)
{
	std::vector<void*> arg_list;
	va_list list;
	va_start(list, arg_num);
	for (int i = 0; i < arg_num; i++)
	{
		arg_list.push_back(va_arg(list, void*));
	}
	va_end(list);

	p_CLCL->p_Impl->SetDrawFunc(callback, arg_list);
}

void CAVEFrameFunction(CAVECALLBACK callback, int arg_num, ...)
{
	std::vector<void*> arg_list;
	va_list list;
	va_start(list, arg_num);
	for (int i = 0; i < arg_num; i++)
	{
		arg_list.push_back(va_arg(list, void*));
	}
	va_end(list);

	p_CLCL->p_Impl->SetIdleFunc(callback, arg_list);
}

void* CAVEMalloc(size_t size)
{
	return malloc(size);
}

void  CAVEFree(void* ptr)
{
	free(ptr);
}

long long CAVEGetFrameNumber()
{
	return p_CLCL->p_Impl->frameIndex();
}

CAVEID CAVEProcessType()
{
	// TODO: should change to return other PROCESS_TYPE
	return CAVE_APP_PROCESS;
}

CAVELOCK CAVENewLock() { return nullptr; }
void CAVEFreeLock(CAVELOCK lock) {}
void CAVESetReadLock(CAVELOCK lock) {}
void CAVESetWriteLock(CAVELOCK lock) {}
void CAVEUnsetReadLock(CAVELOCK lock) {}
void CAVEUnsetWriteLock(CAVELOCK lock) {}

void CAVENavLock() {}
void CAVENavUnlock() {}

void CAVENavConvertCAVEToWorld(float inposition[3], float outposition[3])
{
	// not implemented yet
}

void CAVENavConvertVectorCAVEToWorld(float invector[3], float outvector[3])
{
	// not implemented yet
}

void CAVENavConvertWorldToCAVE(float inposition[3], float outposition[3])
{
	// not implemented yet
}

void CAVENavConvertVectorWorldToCAVE(float invector[3], float outvector[3])
{
	// not implemented yet
}

void CAVEGetViewport(int *origX, int *origY, int *width, int *height)
{
	// not implemented yet
	CAVEGetWindowGeometry(origX, origY, width, height);
}

CLCL::CLCL()
{
	p_Impl = new Impl();

	p_Impl->p_HMD = new OpenVR();
	for (int i = 0; i < 4; i++)
	{
		p_Impl->m_ButtonState[i] = -1;
	}
	p_Impl->m_IsThreadRunning = true;
}

CLCL::~CLCL()
{
	delete p_Impl;
}

void CLCL::Impl::StartThread()
{
	p_HMD->StartThread();
}

void CLCL::Impl::StopThread()
{
	p_HMD->StopThread();
}

void CLCL::Impl::SetInitFunc(CAVECALLBACK callback, std::vector<void *> arg_list)
{
	p_HMD->SetInitFunction(callback, arg_list);
}

void CLCL::Impl::SetStopFunc(CAVECALLBACK callback, std::vector<void *> arg_list)
{
	p_HMD->SetStopFunction(callback, arg_list);
}

void CLCL::Impl::SetDrawFunc(CAVECALLBACK callback, std::vector<void *> arg_list)
{
	p_HMD->SetDrawFunction(callback, arg_list);
}

void CLCL::Impl::SetIdleFunc(CAVECALLBACK callback, std::vector<void *> arg_list)
{
	p_HMD->SetIdleFunction(callback, arg_list);
}
