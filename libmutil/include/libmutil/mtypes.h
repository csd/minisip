#ifndef _MTYPES_H
#define _MTYPES_H

// Add cross-platform, cross library/application types in this file.

#if defined _MSC_VER

typedef unsigned short uint16_t;

typedef __int32  int32_t;
typedef unsigned int uint32_t;

typedef unsigned long long uint64_t;

#else
#include<stdint.h>
#endif 

typedef uint8_t byte_t;

#endif
