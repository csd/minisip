# AM_MINISIP_CHECK_LDAP
#-----------------------
AC_DEFUN([AM_MINISIP_CHECK_LDAP], [
	# PKG_CHECK_MODULES([LDAP], [libldap], [liblber])

	mnetutil_save_LIBS="${LIBS}"
	LDAP_LIBS="-lldap -llber"
	LIBS="${LDAP_LIBS} ${LIBS}"
	AC_CHECK_FUNCS([ldap_init],,[AC_MSG_ERROR([OpenLDAP not found])])
	LIBS="${mnetutil_save_LIBS}"
])


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

# AM_MINISIP_CHECK_LIBMNETUTIL(VERSION [,ACTION-IF-FOUND [,ACTION-IF-NOT-FOUND]])
# -------------------------------------
AC_DEFUN([AM_MINISIP_CHECK_LIBMNETUTIL],[ 
	AC_REQUIRE([AM_MINISIP_CHECK_LIBMUTIL]) dnl
	mnetutil_found=yes
	AC_MINISIP_WITH_ARG(MNETUTIL, mnetutil, libmnetutil, $1, ifelse([$3], , [REQUIRED], [OPTIONAL]), , ,[mnetutil_found=no])
	if test ! "${mnetutil_found}" = "no"; then
		AC_MINISIP_CHECK_LIBRARY(MNETUTIL, libmnetutil, libmnetutil_config.h, mnetutil, ,[mnetutil_found=no])
	fi

	if test "${mnetutil_found}" = "yes"; then
		ifelse([$2], , :, [$2])
	else
		ifelse([$3], , [AC_MINISIP_REQUIRED_LIB(MNETUTIL, libnetmutil)], [$3])
	fi
  ])
# End of AM_MINISIP_CHECK_LIBMNETUTIL
#

# AM_MINISIP_CHECK_LIBMNETUTIL_SCTP([ACTION-IF-FOUND [,ACTION-IF-NOT-FOUND]])
# ------------------------------------
AC_DEFUN([AM_MINISIP_CHECK_LIBMNETUTIL_SCTP],[ 
	AC_REQUIRE([AM_MINISIP_CHECK_LIBMNETUTIL]) dnl
	mnetutil_sctp_found=yes

dnl Checks for SCTP support in libmnetutil
	mnetutil_save_LIBS="$LIBS"
	mnetutil_save_LDFLAGS="$LDFLAGS"
	mnetutil_save_CPPFLAGS="$CPPFLAGS"
	LDFLAGS="$LDFLAGS $LIBMNETUTIL_LDFLAGS"
	LIBS="$MINISIP_LIBS $LIBS"
	CPPFLAGS="$CPPFLAGS $MINISIP_CFLAGS"
	AM_MINISIP_CHECK_WINFUNC(["adsfasdfasdf()"],,[mnetutil_sctp_found=no],[dnl
#include<libmnetutil/SctpSocket.h>
])
	LIBS="$mnetutil_save_LIBS"
	LDFLAGS="$mnetutil_save_LDFLAGS"
	CPPFLAGS="$mnetutil_save_CPPFLAGS"

	if test "${mnetutil_sctp_found}" = "yes"; then
		AC_DEFINE([HAVE_SCTP], 1, [Define to 1 if you have libmnetutil with SCTP support])
		ifelse([$1], , :, [$1])
	else
		ifelse([$2], , :, [$2])
	fi
  ])
# End of AM_MINISIP_CHECK_LIBMNETUTIL_SCTP
#
