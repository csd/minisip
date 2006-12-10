dnl                                               -*- Autoconf -*-
dnl Process this file with autoconf to produce a configure script.

dnl AC_PREREQ(2.57)

m4_MINISIP_PACKAGE_VERSION(libmnetutil, mnetutil, 0, 3, 1)
m4_MINISIP_PACKAGE_CONTACT([Erik Eliasson], [eliasson@imit.kth.se])
dnl m4_MINISIP_PACKAGE_RELEASED
m4_MINISIP_LIBRARY_VERSION(0, 0, 0)

AC_INIT(MINISIP_PACKAGE_NAME, MINISIP_PACKAGE_VERSION, MINISIP_PACKAGE_CONTACT)
AC_CONFIG_AUX_DIR(config)
AC_CONFIG_MACRO_DIR(m4)
AM_MINISIP_PACKAGE_INIT
AM_CONFIG_HEADER(include/compilation_config.h)

AM_MINISIP_LIBTOOL_EXTRAS
AC_PROG_LIBTOOL
AC_PROG_AWK

AM_MINISIP_ENABLE_DEBUG
AM_MINISIP_ENABLE_TEST_SUITE
AM_MINISIP_ENABLE_IPV6

dnl networking headers
AC_CHECK_HEADERS([netinet/in.h ws2tcpip.h])
AC_CHECK_HEADERS([ifaddrs.h netdb.h sys/socket.h])
AM_MINISIP_CHECK_IPV6

AC_ARG_ENABLE(srp,
    AS_HELP_STRING([--enable-srp],
        [enables TLS-SRP support]),
    [ if test "${enable_srp}" = "yes"
      then
        AC_DEFINE(ENABLE_SRP, [], [TLS-SRP support is enabled])
	ENABLE_SRP="yes"
      fi  
      ])
AM_CONDITIONAL(ENABLE_SRP, test "${ENABLE_SRP}" = "yes" )

if test "$os_win" = "yes"; then
dnl
dnl mingw support
dnl
AC_CHECK_HEADERS([iphlpapi.h],,,[
#include<windows.h>
])

AC_CHECK_TYPES([PIP_ADAPTER_ADDRESSES, PIP_ADAPTER_DNS_SERVER_ADDRESS],,,[
#include<windows.h>
#ifdef HAVE_IPHLPAPI_H
#include<iphlp.h>
#endif
])

AC_CHECK_LIB([wsock32], [main])
AC_CHECK_LIB([iphlpapi], [main])
AC_CHECK_LIB([dnsapi], [main])
AC_CHECK_LIB([ws2_32], [main])

dnl Check if some WinXP function declarations are present in the header files
AM_MINISIP_CHECK_WINFUNCS(["getaddrinfo(0,0,0,0)" "freeaddrinfo(0)" "GetAdaptersAddresses(0,0,0,0,0)" "getnameinfo(0,0,0,0,0,0,0)"],,,[dnl
#define _WIN32_WINNT 0x0501
#include<windows.h>
#include<iphlpapi.h>
#include<ws2tcpip.h>
])
else
AC_CHECK_FUNCS([getifaddrs getnameinfo])
fi

AM_MINISIP_CHECK_LIBMUTIL([0.3.1])
AM_MINISIP_CHECK_LIBMCRYPTO([0.3.1])
AM_MINISIP_CHECK_COMPLETE

AC_CHECK_FUNCS([poll])
AC_REPLACE_FUNCS([inet_aton inet_pton inet_ntop])

dnl Checks for header files.
AC_HEADER_STDC
AC_CHECK_HEADERS([malloc.h stdlib.h string.h unistd.h sys/types.h sys/socket.h netdb.h netinet/in.h arpa/inet.h netinet/tcp.h linux/sockios.h arpa/nameser_compat.h])

dnl Checks for typedefs, structures, and compiler characteristics.
AC_C_CONST
AC_HEADER_TIME

AC_CONFIG_FILES([
		Makefile 
		libmnetutil.pc 
		examples/Makefile
		include/Makefile 
		m4/Makefile 
		tests/Makefile
		udns/Makefile
		win32/Makefile 
		win32/libmnetutil-res.rc
	])
AC_OUTPUT
