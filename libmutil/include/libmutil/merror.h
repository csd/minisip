#ifndef _MERROR_H
#define _MERROR_H

#ifdef _MSC_VER
#ifdef LIBMUTIL_EXPORTS
#define LIBMUTIL_API __declspec(dllexport)
#else
#define LIBMUTIL_API __declspec(dllimport)
#endif
#else
#define LIBMUTIL_API
#endif


/**
 * Purpose: Provide "perror"-like functionality on 
 * all operating systems.
 * Note: perror is available on windows, but reports
 * "No error" on system errors.
 * If compiled with visual studio, then we use 
 * GetLastError+FormatMessage. In all other cases
 * perror is called.
 *
 * See manual page for perror for further information.
 */
LIBMUTIL_API void merror(const char *s);

#endif
