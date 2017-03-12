#pragma once 
/*
#pragma warning(disable:4786)
#pragma warning(disable:4503)

#undef _WIN32_WINNT
#undef WIN32_LEAN_AND_MEAN

#define WIN32_LEAN_AND_MEAN
#define _WIN32_WINNT 0x0500
#undef _UNICODE
#undef UNICODE

#define _USE_MATH_DEFINES

*/
//#ifdef _DEBUG
//	#define _STLP_DEBUG 1
//#endif

#include <math.h>
#include <string>
#include <vector>
#include <set>
#include <map>
#include <list>
#include <algorithm>
#include <cstdlib>
#include <stdio.h>
#include <stdlib.h>
#ifdef DEF_WP8
	#include "gettimeofday.h"
#else 
	#include <sys/time.h>          // for gettimeofday()
#endif
#ifdef __QT__
#include <string.h>
#endif

//#include "universalApp.h"
//#include "TextureMngr.h"

extern float MAX_MEMORY_FOR_IMAGE; //40.0
extern float MAX_MEMORY_FOR_SOUND; //25.0
extern float MEMORY_FOR_SOUND_STEP; //5.0
#define FADE_TIME 500.0
//extern eDeviceType deviceType;
extern float systemVersion;
extern float scaleImageCoeff;
//extern TextureMngr *mTextureMgr;
//#include <windows.h>
//#include <shellapi.h> 
//#include <mmsystem.h>
//#include "ModVal.h"

#ifdef _USE_WIDE_STRING

#ifndef ANDROID_OS
    #define _S(x)				L ##x
#endif

#ifndef DEF_ANDROID
#define _S(x)				x
#endif

#define LONG_BIGE_TO_NATIVE(l) (((l >> 24) & 0xFF) | ((l >> 8) & 0xFF00) | ((l << 8) & 0xFF0000) | ((l << 24) & 0xFF000000))
#define WORD_BIGE_TO_NATIVE(w) (((w >> 8) & 0xFF) | ((w << 8) & 0xFF00))
#define LONG_LITTLEE_TO_NATIVE(l) (l)
#define WORD_LITTLEE_TO_NATIVE(w) (w)

#define LENGTH(anyarray) (sizeof(anyarray) / sizeof(anyarray[0]))


#define RR_COMMENT(msg) printf("COMMENT: %s (%s:%d)\n", #msg,  __FILE__, __LINE__);
#define RR_WARNING(msg) printf("WARNING: %s (%s:%d)\n", #msg,  __FILE__, __LINE__);
#define RR_SND_WARNING(msg) printf("WARNING: %s (%s:%d)\n", #msg,  __FILE__, __LINE__);
#define RR_ERROR(msg) printf("ERROR: %s (%s:%d)\n", #msg,  __FILE__, __LINE__);
#define RR_LOG(msg) printf("LOG: %s\n", msg);

//#define OutputDebugString printf
//#define OutputDebugStringA printf

typedef unsigned char uchar;
typedef unsigned short ushort;
typedef unsigned int uint;
typedef unsigned long ulong;
typedef long long int64;

#ifdef ANDROID_OS
namespace std
{
    typedef basic_string<wchar_t> wstring;
};
#endif

typedef std::map<std::string, std::string>		DefinesMap;
typedef std::map<std::wstring, std::wstring>	WStringWStringMap;

//======================== nEW ==================================
int			sign(float x);
int			Rand(int min, int max);
float		fRand(float min = 0.0, float max = 1.0);
float		CalcDiffAngle(float Angle1, float Angle2);
void		NormalizeAngle(float& angle);
}

typedef unsigned long       DWORD;
#if OS != OS_WP8
typedef unsigned char       BYTE;
#endif
typedef unsigned short      WORD;
typedef float               FLOAT;
typedef int                 INT;
typedef unsigned int        UINT;


#if OS!=OS_WP8
template <class T> const T min ( const T a, const T b ) {
	return (a<b)?a:b;     // or: return comp(a,b)?a:b; for the comp version
}
template <class T> const T max ( const T a, const T b ) {
	return (a>b)?a:b;     // or: return +(a,b)?a:b; for the comp version
}
#endif

int isEqualWithoutRegistr(const char *str1, const char *str2);
