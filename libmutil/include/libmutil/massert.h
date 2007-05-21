#ifndef _MASSERT_H
#define _MASSERT_H

#include <libmutil/libmutil_config.h>



LIBMUTIL_API void massertFailed(const char *expr, const char *file, const char *baseFile, int line);

#ifdef NDEBUG
#define massert(exp)
#else
/**
 * massert works the same way as assert except that 
 * if libmutil has support for stack traces, then
 * the current state of the stack is output.
 * Note that the stack trace will contain calls to
 * "massertFailed" and "getStackTraceString".
 */

#ifdef _MSC_VER
#include<assert.h>
#define massert(exp) assert(exp)
#else
#ifdef __BASE_FILE__
#define massert(exp) if (exp) ; else massertFailed(#exp, __FILE__, __BASE_FILE__, __LINE__)
#else
#define massert(exp) if (exp) ; else massertFailed(#exp, __FILE__, __FILE__, __LINE__)
#endif
#endif
#endif

#endif
