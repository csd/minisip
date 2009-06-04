#ifndef _MTYPES_H
#define _MTYPES_H

// Add cross-platform, cross library/application types in this file.

#ifdef _MSC_VER

#ifndef int8_t
typedef signed char int8_t;
#endif

#ifndef uint8_t
typedef unsigned char  uint8_t;
#endif

#ifndef byte_t
typedef unsigned char byte_t;
#endif

#ifndef int16_t
typedef __int16  int16_t;
#endif

#ifndef uint16_t
typedef unsigned short  uint16_t;
#endif

#ifndef int32_t
typedef __int32  int32_t;
#endif

#ifndef uint32_t
typedef unsigned int  uint32_t;
#endif

#ifndef int64_t
typedef __int64  int64_t;
#endif

#ifndef uint64_t
	#ifdef _MSC_VER
		typedef unsigned __int64 uint64_t;
	#else
		typedef unsigned long long  uint64_t;
	#endif
#endif

#else

#include<stdint.h>

#endif //_MSC_VER

typedef uint8_t byte_t;

#endif

