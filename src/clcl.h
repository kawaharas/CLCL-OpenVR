////////////////////////////////////////////////////////////////////////////////
//
// clcl.h
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

#define USE_CLCL

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX // to use "std::max()"
#include <windows.h>
#endif // _WIN32
#include <GL/glew.h>

#define _USE_MATH_DEFINES
#include <iostream>
#include <cmath>

extern float CAVENear, CAVEFar;
//float CAVENearTLS[4], CAVEFarTLS[4];
//#define CAVENear   (CAVENearTLS[CAVETID])
//#define CAVENear   ((float)CAVENearTLS[CAVETID])
//#define CAVEFar    (CAVEFarTLS[CAVETID])
//#define CAVEFar    ((float)CAVEFarTLS[CAVETID])

const int CAVE_MAX_WALLS = 1;

extern const int CONTROLLER_BUTTON1;
extern const int CONTROLLER_BUTTON2;
extern const int CONTROLLER_BUTTON3;
extern const int CONTROLLER_BUTTON4;

typedef enum {
	JOYSTICK_LEFT = 0,
	JOYSTICK_RIGHT,
} JOYSTICK_TYPE;

typedef enum {
	CAVE_HEAD,
	CAVE_WAND,
	CAVE_LEFT_EYE,
	CAVE_RIGHT_EYE,
	CAVE_HEAD_NAV,
	CAVE_WAND_NAV,
	CAVE_LEFT_EYE_NAV,
	CAVE_RIGHT_EYE_NAV,
	CAVE_HEAD_FRONT,
	CAVE_WAND_FRONT,
	CAVE_HEAD_BACK,
	CAVE_WAND_BACK,
	CAVE_HEAD_LEFT,
	CAVE_WAND_LEFT,
	CAVE_HEAD_RIGHT,
	CAVE_WAND_RIGHT,
	CAVE_HEAD_UP,
	CAVE_WAND_UP,
	CAVE_HEAD_DOWN,
	CAVE_WAND_DOWN,
	CAVE_HEAD_FRONT_NAV,
	CAVE_WAND_FRONT_NAV,
	CAVE_HEAD_BACK_NAV,
	CAVE_WAND_BACK_NAV,
	CAVE_HEAD_LEFT_NAV,
	CAVE_WAND_LEFT_NAV,
	CAVE_HEAD_RIGHT_NAV,
	CAVE_WAND_RIGHT_NAV,
	CAVE_HEAD_UP_NAV,
	CAVE_WAND_UP_NAV,
	CAVE_HEAD_DOWN_NAV,
	CAVE_WAND_DOWN_NAV,
	CAVE_APP_PROCESS,
	CAVE_DISPLAY_PROCESS,
	CAVE_TRACKER_PROCESS,
	CAVE_NETWORK_PROCESS,
	CAVE_DISTRIB_PROCESS,

	CAVE_DIST_NETWORKSLAVE,
	CAVE_GL_ACCUMSIZE,
	CAVE_GL_SAMPLES,
	CAVE_GL_STENCILSIZE,
	CAVE_NET_BUFFERSIZE,
	CAVE_NET_NUMBUFFERS,
	CAVE_NET_UPDATELOCALDATA,
	CAVE_PROJ_INCLUDENAVIGATION,
	CAVE_PROJ_USEWINDOW,
	CAVE_PROJ_USEMODELVIEW,
	CAVE_SCRAMNET_ARENASIZE,
	CAVE_SHMEM_SIZE,
	CAVE_SHMEM_ADDRESS,
	CAVE_SIM_DRAWOUTLINE,
	CAVE_SIM_DRAWTIMING,
	CAVE_SIM_DRAWUSER,
	CAVE_SIM_DRAWWAND,
	CAVE_SIM_VIEWMODE,
	CAVE_TRACKER_SIGNALRESET

} CAVEID;

typedef enum {
	CAVE_NULLDEVICE			=   0,
	CAVE_AKEY				=  65, // GLFW_KEY_A
	CAVE_BKEY				=  66, // GLFW_KEY_B,
	CAVE_CKEY				=  67, // GLFW_KEY_C,
	CAVE_DKEY				=  68, // GLFW_KEY_D,
	CAVE_EKEY				=  69, // GLFW_KEY_E,
	CAVE_FKEY				=  70, // GLFW_KEY_F,
	CAVE_GKEY				=  71, // GLFW_KEY_G,
	CAVE_HKEY				=  72, // GLFW_KEY_H,
	CAVE_IKEY				=  73, // GLFW_KEY_I,
	CAVE_JKEY				=  74, // GLFW_KEY_J,
	CAVE_KKEY				=  75, // GLFW_KEY_K,
	CAVE_LKEY				=  76, // GLFW_KEY_L,
	CAVE_MKEY				=  77, // GLFW_KEY_M,
	CAVE_NKEY				=  78, // GLFW_KEY_N,
	CAVE_OKEY				=  79, // GLFW_KEY_O,
	CAVE_PKEY				=  80, // GLFW_KEY_P,
	CAVE_QKEY				=  81, // GLFW_KEY_Q,
	CAVE_RKEY				=  82, // GLFW_KEY_R,
	CAVE_SKEY				=  83, // GLFW_KEY_S,
	CAVE_TKEY				=  84, // GLFW_KEY_T,
	CAVE_UKEY				=  85, // GLFW_KEY_U,
	CAVE_VKEY				=  86, // GLFW_KEY_V,
	CAVE_WKEY				=  87, // GLFW_KEY_W,
	CAVE_XKEY				=  88, // GLFW_KEY_X,
	CAVE_YKEY				=  89, // GLFW_KEY_Y,
	CAVE_ZKEY				=  90, // GLFW_KEY_Z,
	CAVE_ZEROKEY			=  48, // GLFW_KEY_0,
	CAVE_ONEKEY				=  49, // GLFW_KEY_1,
	CAVE_TWOKEY				=  50, // GLFW_KEY_2,
	CAVE_THREEKEY			=  51, // GLFW_KEY_3,
	CAVE_FOURKEY			=  52, // GLFW_KEY_4,
	CAVE_FIVEKEY			=  53, // GLFW_KEY_5,
	CAVE_SIXKEY				=  54, // GLFW_KEY_6,
	CAVE_SEVENKEY			=  55, // GLFW_KEY_7,
	CAVE_EIGHTKEY			=  56, // GLFW_KEY_8,
	CAVE_NINEKEY			=  57, // GLFW_KEY_9,
	CAVE_LEFTCTRLKEY		= 341, // GLFW_KEY_LEFT_CONTROL,
	CAVE_RIGHTCTRLKEY		= 345, // GLFW_KEY_RIGHT_CONTROL,
//	CAVE_CTRLKEY			= GLFW_KEY_LEFT_CONTROL | GLFW_KEY_RIGHT_CONTROL,
	CAVE_CTRLKEY			= 341, // GLFW_KEY_LEFT_CONTROL,
	CAVE_ESCKEY				= 256, // GLFW_KEY_ESCAPE,
	CAVE_F1KEY				= 290, // GLFW_KEY_F1,
	CAVE_F2KEY				= 291, // GLFW_KEY_F2,
	CAVE_F3KEY				= 292, // GLFW_KEY_F3,
	CAVE_F4KEY				= 293, // GLFW_KEY_F4,
	CAVE_F5KEY				= 294, // GLFW_KEY_F5,
	CAVE_F6KEY				= 295, // GLFW_KEY_F6,
	CAVE_F7KEY				= 296, // GLFW_KEY_F7,
	CAVE_F8KEY				= 297, // GLFW_KEY_F8,
	CAVE_F9KEY				= 298, // GLFW_KEY_F9,
	CAVE_F10KEY				= 299, // GLFW_KEY_F10,
	CAVE_F11KEY				= 300, // GLFW_KEY_F11,
	CAVE_F12KEY				= 301, // GLFW_KEY_F12,
	CAVE_CAPSLOCKKEY		= 280, // GLFW_KEY_CAPS_LOCK,
	CAVE_SCROLLLOCKKEY		= 281, // GLFW_KEY_SCROLL_LOCK,
	CAVE_NUMLOCKKEY			= 282, // GLFW_KEY_NUM_LOCK,
	CAVE_PRINTSCREENKEY		= 283, // GLFW_KEY_PRINT_SCREEN,
	CAVE_PAGEUPKEY			= 266, // GLFW_KEY_PAGE_UP,
	CAVE_PAGEDOWNKEY		= 267, // GLFW_KEY_PAGE_DOWN,
	CAVE_HOMEKEY			= 268, // GLFW_KEY_HOME,
	CAVE_ENDKEY				= 269, // GLFW_KEY_END,
	CAVE_LEFTSHIFTKEY		= 344, // GLFW_KEY_RIGHT_SHIFT,
	CAVE_LEFTALTKEY			= 342, // GLFW_KEY_LEFT_ALT,
	CAVE_RIGHTSHIFTKEY		= 340, // GLFW_KEY_LEFT_SHIFT,
	CAVE_RIGHTALTKEY		= 346, // GLFW_KEY_RIGHT_ALT,
	CAVE_PAUSEKEY			= 284, // GLFW_KEY_PAUSE,
	CAVE_INSERTKEY			= 260, // GLFW_KEY_INSERT,
	CAVE_TABKEY				= 258, // GLFW_KEY_TAB,
	CAVE_RETKEY				= 257, // GLFW_KEY_ENTER,
	CAVE_SPACEKEY			=  32, // GLFW_KEY_SPACE,
	CAVE_BACKSPACEKEY		= 259, // GLFW_KEY_BACKSPACE,
	CAVE_DELKEY				= 261, // GLFW_KEY_DELETE,
	CAVE_SEMICOLONKEY		=  59, // GLFW_KEY_SEMICOLON,
	CAVE_PERIODKEY			=  46, // GLFW_KEY_PERIOD,
	CAVE_COMMAKEY			=  44, // GLFW_KEY_COMMA,
	CAVE_EQUALKEY			=  61, // GLFW_KEY_EQUAL,
	CAVE_MINUSKEY			=  45, // GLFW_KEY_MINUS,
	CAVE_QUOTEKEY			=  39, // GLFW_KEY_APOSTROPHE,
	CAVE_ACCENTGRAVEKEY		=  96, // GLFW_KEY_GRAVE_ACCENT,
	CAVE_BACKSLASHKEY		=  92, // GLFW_KEY_BACKSLASH,
	CAVE_LEFTBRACKETKEY		=  91, // GLFW_KEY_LEFT_BRACKET,
	CAVE_RIGHTBRACKETKEY	=  93, // GLFW_KEY_RIGHT_BRACKET,
	CAVE_LEFTARROWKEY		= 263, // GLFW_KEY_LEFT,
	CAVE_RIGHTARROWKEY		= 262, // GLFW_KEY_RIGHT,
	CAVE_DOWNARROWKEY		= 264, // GLFW_KEY_DOWN,
	CAVE_UPARROWKEY			= 265  // GLFW_KEY_UP
} CAVEDevice;

typedef struct {
	bool Initted;
	bool Quit;
} CAVE_SYNC;

extern CAVE_SYNC *CAVESync;

void  CAVEGetOrientation(CAVEID id, float angle[3]);
bool  CAVEMasterWall();
bool  CAVEMasterDisplay();
bool  CAVEDistribMaster();
int   CAVEDistribNumNodes();
void  CAVEDisplayBarrier();
void  CAVEGetPosition(CAVEID id, float position[3]);
void  CAVEGetVector(CAVEID id, float vector[3]);
float CAVEGetTime();
int   CAVEButtonChange(int buttonNumber);
void  CAVENavTranslate(float xtrans, float ytrans, float ztrans);
void  CAVENavRot(float angle, char axis);
void  CAVENavScale(float xscale, float yscale, float zscale);
void  CAVENavWorldTranslate(float xtrans, float ytrans, float ztrans);
void  CAVENavWorldRot(float angle, char axis);
void  CAVENavWorldScale(float xscale, float yscale, float zscale);
void  CAVENavTransform();
void  CAVENavInverseTransform();
void  CAVENavLoadIdentity();
bool  CAVEgetbutton(CAVEDevice device);
void  CAVEUSleep(unsigned long milliseconds);
void  CAVENavMultMatrix(float matrix[4][4]);
void  CAVENavPreMultMatrix(float matrix[4][4]);

int   CAVEUniqueIndex();
int   CAVENumPipes();
void  CAVEGetWindowGeometry(int *origX, int *origY, int *width, int *height);
void  CAVENavGetMatrix(float matrix[4][4]);
void  CAVENavLoadMatrix(float matrix[4][4]);

bool  IsButtonPressed(const int buttonID);
std::pair<float, float> GetJoyStickValue(JOYSTICK_TYPE type);

#define CAVEBUTTON1 (IsButtonPressed(CONTROLLER_BUTTON1))
#define CAVEBUTTON2 (IsButtonPressed(CONTROLLER_BUTTON2))
#define CAVEBUTTON3 (IsButtonPressed(CONTROLLER_BUTTON3))
#define CAVEBUTTON4 (IsButtonPressed(CONTROLLER_BUTTON4))
#define CAVE_JOYSTICK_X (GetJoyStickValue(JOYSTICK_RIGHT).first)
#define CAVE_JOYSTICK_Y (GetJoyStickValue(JOYSTICK_RIGHT).second)

void  CAVEConfigure(int *argc, char **argv, char **appdefaults);
void  CAVEInit();
void  CAVEExit();
void  CAVEHalt();

typedef void (* CAVECALLBACK)();

void  CAVEInitApplication(CAVECALLBACK callback, int num_arg, ...);
void  CAVEDisplay(CAVECALLBACK callback, int num_arg, ...);
void  CAVEFrameFunction(CAVECALLBACK callback, int num_arg, ...);
void  CAVEStopApplication(CAVECALLBACK callback, int arg_num, ...);

void* CAVEMalloc(size_t size);
void  CAVEFree(void* ptr);

long long CAVEGetFrameNumber();

CAVEID CAVEProcessType();

typedef void *CAVELOCK;
CAVELOCK CAVENewLock();
void CAVEFreeLock(CAVELOCK lock);
void CAVESetReadLock(CAVELOCK lock);
void CAVESetWriteLock(CAVELOCK lock);
void CAVEUnsetReadLock(CAVELOCK lock);
void CAVEUnsetWriteLock(CAVELOCK lock);
void CAVENavLock();
void CAVENavUnlock();
void CAVENavConvertCAVEToWorld(float inposition[3], float outposition[3]);
void CAVENavConvertVectorCAVEToWorld(float invector[3], float outvector[3]);
void CAVENavConvertWorldToCAVE(float inposition[3], float outposition[3]);
void CAVENavConvertVectorWorldToCAVE(float invector[3], float outvector[3]);
void CAVEGetViewport(int *origX, int *origY, int *width, int *height);
void CAVESetOption(CAVEID option, int value);

void sginap(unsigned long milliseconds);

class CLCL
{
public:
	CLCL();
	~CLCL();

	class Impl;
	Impl *p_Impl;
};

extern CLCL *p_CLCL;
