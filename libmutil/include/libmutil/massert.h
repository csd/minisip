#ifndef _MASSERT_H
#define _MASSERT_H

void massertFailed(char *expr, char *file, char *baseFile, int line);

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
#define massert(exp) if (exp) ; else massertFailed(#exp, __FILE__, __BASE_FILE__, __LINE__)
#endif

#endif
