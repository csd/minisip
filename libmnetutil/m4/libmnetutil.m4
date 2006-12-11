# AM_MINISIP_ENABLE_IPV6(VERSION)
# -------------------------------
AC_DEFUN([AM_MINISIP_ENABLE_IPV6],[ 
AC_ARG_ENABLE([ipv6],
    AS_HELP_STRING([--disable-ipv6],
        [disables debug output (default enabled)]), [], [enable_ipv6=yes])
  ])
# End of AM_MINISIP_ENABLE_IPV6
#

# AM_MINISIP_CHECK_IPV6(VERSION)
# ------------------------------
AC_DEFUN([AM_MINISIP_CHECK_IPV6],[ 
if test "${enable_ipv6}" = "yes"
then
	AC_CHECK_TYPES([struct sockaddr_in6],[HAVE_IPV6=yes],,[
#ifdef HAVE_NETINET_IN_H
# include <netinet/in.h>
#endif
#ifdef HAVE_WS2TCPIP_H
# include <ws2tcpip.h>
#endif
])
fi
if test "${HAVE_IPV6}" = "yes"
then
	AC_DEFINE(HAVE_IPV6, [1], [Define to 1 to enable IPv6 support ])
fi
AM_CONDITIONAL(HAVE_IPV6, test "x${HAVE_IPV6}" = "xyes")
  ])
# End of AM_MINISIP_CHECK_IPV6
#

# AM_MINISIP_CHECK_LIBMNETUTIL(VERSION)
# -------------------------------------
AC_DEFUN([AM_MINISIP_CHECK_LIBMNETUTIL],[ 
	AC_REQUIRE([AM_MINISIP_CHECK_LIBMUTIL]) dnl
	AC_MINISIP_WITH_ARG(MNETUTIL, mnetutil, libmnetutil, $1, [REQUIRED])
	AC_MINISIP_CHECK_LIBRARY(MNETUTIL, libmnetutil, libmnetutil_config.h, mnetutil)
  ])
# End of AM_MINISIP_CHECK_LIBMNETUTIL
#
