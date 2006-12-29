# AM_MINISIP_CHECK_LIBMIKEY(VERSION [,ACTION-IF-FOUND [,ACTION-IF-NOT-FOUND]])
# ----------------------------------
AC_DEFUN([AM_MINISIP_CHECK_LIBMIKEY],[ 
	AC_REQUIRE([AM_MINISIP_CHECK_LIBMCRYPTO]) dnl
	mikey_found=yes
	AC_MINISIP_WITH_ARG(MIKEY, mikey, libmikey, $1, ifelse([$3], , [REQUIRED], [OPTIONAL]), , ,[mikey_found=no])
	if test ! "${mikey_found}" = "no"; then
		AC_MINISIP_CHECK_LIBRARY(MIKEY, libmikey, libmikey_config.h, mikey)
	fi
	if test "${mikey_found}" = "yes"; then
		ifelse([$2], , :, [$2])
	else
		ifelse([$3], , [AC_MINISIP_REQUIRED_LIB(MIKEY, libmikey)], [$3])
	fi
  ])
# End of AM_MINISIP_CHECK_LIBMIKEY
#
