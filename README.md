# CLCL (CAVELib Compatible Library for HMD)

***This is the CLCL for OpenVR. Oculus SDK version is [here](https://github.com/kawaharas/CLCL).***

CLCL is a C++ library for porting CAVE application software to HMDs. 
It emulates the function calls of CAVELib, which is a commercial library 
for developing application software executable on CAVEs, 
and it enables us to easily port CAVELib application software 
to HMDs with minor modification to the original source code.

## Required Hardware

&nbsp; One of the following hardware:

- Oculus Rift with Oculus Touch or mouse
- Oculus Development Kit 2 with mouse
- HTC VIVE
- HTC VIVE Pro
- Windows Mixed Reality device

## Required Software / Libraries

- Visual Studio 2017
- OpenVR 1.2.10
- GLFW 3.2.1 *
- GLEW 2.1.0 *
- GLM 0.9.9

&nbsp; \*  These libraries are needed to build with multi-threaded (/MT) option.

&nbsp; Optional (experimental: for the external camera function):

&nbsp; &nbsp; Following library is not used in the pre-built library.  
&nbsp; &nbsp; Please rebuild your own library if you want to enable the external camera function.  
&nbsp; &nbsp; To toggle enable/disable the external camera, press "c" key.

- Ovrvision Pro SDK 1.90 *

&nbsp; \* The library is needed to use the external camera (Ovrvision Pro by Wizapply).

## Building the Library

1) Open CLCL_openvr.sln.
2) Set include path and library path to OpenVR.
3) Build library.

## Using the Library

Source code modifications are needed to use CLCL.

1) Resolve conflicts between GLEW and other OpenGL header files if these are used in the target code.

Compilation of the code.

2) Change include path and library path from CAVELib's to CLCL's.
3) Change library file to link from "libcave_ogl_XX.lib" to "CLCL_openvr.lib".
4) Build with multi-threaded (/MT) option.
5) If compilation failed, modification of codes is needed. -> go back to 4)

## Controller

| |CAVE_JOYSTICK_X<br>CAVE_JOYSTICK_Y |CAVE_BUTTON1 |CAVE_BUTTON2 |CAVE_BUTTON3 |
|---|---|---|---|---|
|Oculus Touch (Right) |Thumb stick |Button A |Trigger |Button B |
|VIVE controller|Trackpad |Grip button |Trigger |Menu button |
|Motion controller (WMR) |Touchpad |Grub button |Select trigger |Menu button |
|Mouse |Wheel (CAVE_JOYSTICK_Y) |Left button |Middle button |Right button |

## About Trademarks

&nbsp; The CAVE is a registered trademark of the Board of Trustees of the University of Illinois at Chicago.  
&nbsp; CAVELib is a trademark of the University of Illinois Board of Trustees.  
&nbsp; Oculus, Rift, and Oculus Touch are trademarks or registered trademarks of Facebook Technologies, LLC.  
&nbsp; HTC and VIVE are trademarks or registered trademarks of HTC Corporation.  
&nbsp; All other trademarks are property of their respective owners.
