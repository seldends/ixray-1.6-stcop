#pragma once

#define VC_EXTRALEAN				// Exclude rarely-used stuff from Windows headers
#define WIN32_LEAN_AND_MEAN			// Exclude rarely-used stuff from Windows headers
#ifndef STRICT
#	define STRICT					// Enable strict syntax
#endif // STRICT
#define IDIRECTPLAY2_OR_GREATER		// ?
#define DIRECTINPUT_VERSION	0x0800	//
#define _CRT_SECURE_NO_DEPRECATE	// vc8.0 stuff, don't deprecate several ANSI functions

// windows.h
#undef _WIN32_WINNT

#ifndef _WIN32_WINNT
#ifdef _MSC_VER
#define _WIN32_WINNT _WIN32_WINNT_WIN7
#else // ifdef _MSC_VER
#define _WIN32_WINNT 0x0500
#endif // ifdef _MSC_VER
#endif // ifndef _WIN32_WINNT

#define NOGDICAPMASKS
#define NOMENUS
#define NOICONS
#define NOKEYSTATES
#define NODRAWTEXT
#define NOMEMMGR
#define NOSERVICE
#define NOHELP
#define NOPROFILER
//#define NOMINMAX

#pragma warning(push)
#pragma warning(disable:4005)
#include <windows.h>
#include <lmcons.h>
#include <dbghelp.h>

#ifdef IXR_ARM64
#	include <arm64_neon.h>
#	include <sse2neon/sse2neon.h>
#else
#	include <xmmintrin.h>
#	include <atlstr.h>
#	include <atlimage.h>
#endif

#ifndef __BORLANDC__
	#include <windowsx.h>
#endif
#pragma warning(pop)

#include <sys\utime.h>

#pragma warning(push)
#pragma warning(disable:4995)
#include <direct.h>
#include <io.h>
#include <commdlg.h>
#include <cderr.h>
#pragma warning(pop)


#ifdef __BORLANDC__
#   define ALIGN(a)
#   include <assert.h>
#   include <utime.h>
#   define _utimbuf utimbuf
#   define MODULE_NAME "xrCoreB.dll"

// function redefinition
#   define fabsf(a) fabs(a)
#   define sinf(a) sin(a)
#   define asinf(a) asin(a)
#   define cosf(a) cos(a)
#   define acosf(a) acos(a)
#   define tanf(a) tan(a)
#   define atanf(a) atan(a)
#   define sqrtf(a) sqrt(a)
#   define expf(a) ::exp(a)
#   define floorf floor
#   define atan2f atan2
#   define logf log

#   define _PC_24 PC_24
#   define _PC_53 PC_53
#   define _PC_64 PC_64
#   define _RC_CHOP RC_CHOP
#   define _RC_NEAR RC_NEAR
 #   define _MCW_EM MCW_EM
#else
#   define ALIGN(a) __declspec(align(a))
#   define MODULE_NAME "xrCore.dll"
#endif

using xr_special_char = wchar_t;

#define xr_strerror(errno, buffer, bufferSize) strerror_s(buffer, sizeof(buffer), errno)