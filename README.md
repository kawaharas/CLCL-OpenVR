# CLCL (CAVELib Compatible Library for HMD)

&nbsp; ***This is the OpenVR version of CLCL.***

CLCL is a C++ library for porting CAVE application software to HMDs. 
It emulates the function calls of CAVELib, which is a commercial library 
for developing application software executable on CAVEs, 
and it enables us to easily port CAVELib application software 
to HMDs with minor modification to the original source code.

## Required Software / Libraries

- Visual Studio 2017
- OpenVR 1.2.10
- GLFW 3.2.1
- GLEW 2.1.0
- GLM 0.9.9

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
4) Build.
5) If compilation failed, modification of codes is needed. -> go back to 4)
