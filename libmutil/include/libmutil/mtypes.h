#ifndef _MTYPES_H
#define _MTYPES_H

// Add cross-platform, cross library/application types in this file.

#if defined _MSC_VER
typedef unsigned int uint32_t;
typedef int int32_t;
typedef unsigned long long uint64_t

#else
#include<stdint.h>
#endif 

typedef uint8_t byte_t;

#endif
