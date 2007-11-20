#ifndef _MINISIPLIB_COMPILATION_CONFIG
#define _MINISIPLIB_COMPILATION_CONFIG

#if defined(_WIN32_WCE) || defined(_MSC_VER) 
/* Debug output */
//#define DEBUG_OUTPUT 

//use this to avoid the warning (coming from STLPort used in WinCE)
//exception.h(47) : warning C4275: non dll-interface class 'std::exception' used as base for dll-interface class 'Exception'
//#pragma warning (disable: 4275)

/* Define to 1 if you have the `iphlpapi' library (-liphlpapi). 
This is for the GetAdaptersInfo ... 
*/
#define HAVE_LIBIPHLPAPI 1

/* Define to 1 if you have the `wsock32' library (-lwsock32). */
//must have it ... but can't find it :)
#define HAVE_LIBWSOCK32 1

/* Define to 1 if you have the <ws2tcpip.h> header file. */
#define HAVE_WS2TCPIP_H 1

/* Define to 1 if the system has the type `struct sockaddr_in6'. */
//the struct is defined in ws2tcpip.h ... 
//AF_INETXXX are defined in winsock2.h and winsock.h
#define HAVE_STRUCT_SOCKADDR_IN6 1

/* Define to 1 if you have the <malloc.h> header file. */
#define HAVE_MALLOC_H 1

/* Define to 1 if you have the <memory.h> header file. */
#define HAVE_MEMORY_H 1

/* Define to 1 if you have the <stdlib.h> header file. */
#define HAVE_STDLIB_H 1

/* Define to 1 if you have the <string.h> header file. */
#define HAVE_STRING_H 1

/* Define to 1 if you have the <sys/stat.h> header file. */
#define HAVE_SYS_STAT_H 1

/* Define to 1 if you have the <sys/types.h> header file. */
#define HAVE_SYS_TYPES_H 1

/* Define to 1 if you have the <unistd.h> header file. */
//#define HAVE_UNISTD_H 1

/* No Kerberos in OpenSSL */
#define OPENSSL_NO_KRB5 

/* Name of package */
#define PACKAGE "libminisip"

/* Define to the address where bug reports for this package should be sent. */
#define PACKAGE_BUGREPORT "eliasson@imit.kth.se"

/* Define to the full name of this package. */
#define PACKAGE_NAME "libminisip"

/* Define to the full name and version of this package. */
#define PACKAGE_STRING "libminisip 0.3"

/* Define to the one symbol short name of this package. */
#define PACKAGE_TARNAME "libminisip"

/* Define to the version of this package. */
#define PACKAGE_VERSION "0.3"

/* Define to 1 if you have the ANSI C header files. */
#define STDC_HEADERS 1

/* Define to 1 if you can safely include both <sys/time.h> and <time.h>. */
#define TIME_WITH_SYS_TIME 1

/* STL enabled */
#define USE_STL 

/* Version number of package */
#define VERSION "0.3.1"

#endif

#endif
//#ifndef _MINISIPLIB_COMPILATION_CONFIG
