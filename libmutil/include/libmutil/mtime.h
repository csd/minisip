#ifndef MINISIP_TIME_H
#define MINISIP_TIME_H

#ifdef _MSC_VER
#define uint64_t unsigned long long
#else
#include<stdint.h>
#endif


uint64_t mtime();

#endif
