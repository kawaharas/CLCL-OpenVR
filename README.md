# CLCL (CAVELib Compatible Library for HMD)

***This is the CLCL for OpenVR. Oculus SDK version is [here](https://github.com/kawaharas/CLCL).***

CLCL is a C++ library for porting CAVE application software to HMDs. 
It emulates the function calls of CAVELib, which is a commercial library 
for developing application software executable on CAVEs, 
and it enables us to easily port CAVELib application software 
to HMDs with minor modification to the original source code.

## Required Hardware

One of the following hardware:

- Oculus Rift with Oculus Touch or mouse
- Oculus Development Kit 2 with mouse
- HTC VIVE
- HTC VIVE Pro
- Windows Mixed Reality device

## Required Software / Libraries

**Minimum requirement:**

- Visual Studio
- OpenVR 1.2.10
- GLFW 3.2.1 *
- GLEW 2.1.0 *
- GLM 0.9.9

\*  These libraries are needed to build with multi-threaded (/MT) option.

**Optional (experimental: for the external camera function):**

Following library is not used in the pre-built library.  
Please rebuild your own library if you want to enable the external camera function.  
To toggle enable/disable the external camera, press "c" key.

- Ovrvision Pro SDK 1.90 *

\* The library is needed to use the external camera (Ovrvision Pro by Wizapply).

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
5) If compilation failed, modification of code or project settings are needed. -> go back to 4)

## Controller Inputs

| |CAVE_JOYSTICK_X<br>CAVE_JOYSTICK_Y |CAVE_BUTTON1 |CAVE_BUTTON2 |CAVE_BUTTON3 |
|---|---|---|---|---|
|Oculus Touch (Right) |Thumb stick |Button A |Trigger |Button B |
|VIVE controller|Trackpad |Grip button |Trigger |Menu button |
|Motion controller (WMR) |Touchpad |Grub button |Select trigger |Menu button |
|Mouse |Wheel (CAVE_JOYSTICK_Y) |Left button |Middle button |Right button |

## Citation

Please cite the following paper if you find this library useful in your work.

Shintaro Kawahara and Akira Kageyama: Development of CAVELib Compatible Library for HMD-type VR Devices, Journal of Advanced Simulation in Science and Engineering, Vol.6, No.1, pp.234-248 (2019) doi:10.15748/jasse.6.234

## About Trademarks

The CAVE is a registered trademark of the Board of Trustees of the University of Illinois at Chicago.  
CAVELib is a trademark of the University of Illinois Board of Trustees.  
Oculus, Rift, and Oculus Touch are trademarks or registered trademarks of Facebook Technologies, LLC.  
HTC and VIVE are trademarks or registered trademarks of HTC Corporation.  
All other trademarks are property of their respective owners.
