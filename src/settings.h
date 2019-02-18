////////////////////////////////////////////////////////////////////////////////
//
// settings.h
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

typedef unsigned int  uint;
typedef unsigned long ulong;
typedef unsigned char uchar;
typedef long long     llong;

////////////////////////////////////////////////////////////////////////////////
//
// Entries for OVRVision / OVRVision Pro
//
////////////////////////////////////////////////////////////////////////////////
//
// If you use Oculus SDK 0.5.0.1, enable USE_SDK_0_5_0_1.
//
//#define USE_SDK_0_5_0_1

////////////////////////////////////////////////////////////////////////////////
//
// Entries for OVRVision / OVRVision Pro
//
////////////////////////////////////////////////////////////////////////////////
//
// If you use OVRVision1, enable USE_OVRVISION.
// If you use OVRVision Pro, enable USE_OVRVISION and USE_OVRVISION_PRO.
// If you use OVRVision Pro with a thread for the camera process,
// enable USE_OVRVISION, USE_OVRVISION_PRO and USE_THREAD_FOR_CAMERA_PROCESS.
//

//#define USE_OVRVISION
#define USE_OVRVISION_PRO
//#define USE_THREAD_FOR_CAMERA_PROCESS

// Check the difinitions strictly
#ifndef USE_OVRVISION
#undef USE_OVRVISION_PRO
#undef USE_THREAD_FOR_CAMERA_PROCESS
#else
#ifndef USE_OVRVISION_PRO
#undef USE_THREAD_FOR_CAMERA_PROCESS
#endif //  USE_OVRVISION_PRO
#endif //  USE_OVRVISION
