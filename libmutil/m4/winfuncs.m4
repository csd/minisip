# Checking for stdcall Windows functions.
# Copyright (C) 2006 Mikael Magnusson
#
# Based on functions.m4 in autoconf 2.69a
#
# Copyright (C) 2000, 2001, 2002, 2003 Free Software Foundation, Inc.
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2, or (at your option)
# any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 51 Franklin St, Fifth Floor, Boston MA
# 02110-1301, USA.
#
# As a special exception, the Free Software Foundation gives unlimited
# permission to copy, distribute and modify the configure scripts that
# are the output of Autoconf.  You need not follow the terms of the GNU
# General Public License when using or distributing such scripts, even
# though portions of the text of Autoconf appear in them.  The GNU
# General Public License (GPL) does govern all other use of the material
# that constitutes the Autoconf program.
#
# Certain portions of the Autoconf source text are designed to be copied
# (in certain cases, depending on the input) into the output of
# Autoconf.  We call these the "data" portions.  The rest of the Autoconf
# source text consists of comments plus executable code that decides which
# of the data portions to output in any given case.  We call these
# comments and executable code the "non-data" portions.  Autoconf never
# copies any of the non-data portions into its output.
#
# This special exception to the GPL applies to versions of Autoconf
# released by the Free Software Foundation.  When you make and
# distribute a modified version of Autoconf, you may extend this special
# exception to the GPL to apply to your modified version as well, *unless*
# your modified version has the potential to copy into its output some
# of the text that was the non-data portion of the version that you started
# with.  (In other words, unless your change moves or copies text from
# the non-data portions to the data portions.)  If your modification has
# such potential, you must delete any notice of this special exception
# to the GPL from your modified version.
#
# Written by David MacKenzie, with help from
# Franc,ois Pinard, Karl Berry, Richard Pixley, Ian Lance Taylor,
# Roland McGrath, Noah Friedman, david d zuhn, and many others.

#
# AM_MINISIP_CHECK_WINFUNC(FUNCTION, [ACTION-IF-FOUND], [ACTION-IF-NOT-FOUND],  [INCLUDES])
# -----------------------------------------------------------------
AC_DEFUN([AM_MINISIP_CHECK_WINFUNC],
[ac_func_name=`echo $1|sed -e 's/^\(.*\)(.*)$/\1/'`
AS_VAR_PUSHDEF([ac_var], [ac_cv_func_${ac_func_name}])dnl
AC_CACHE_CHECK([for ${ac_func_name}], ac_var,
[AC_LINK_IFELSE([
AC_INCLUDES_DEFAULT([$4])

int main(){
    $1;
    return 0;
}
],
                [AS_VAR_SET(ac_var, yes)],
                [AS_VAR_SET(ac_var, no)])])
AS_IF([test AS_VAR_GET(ac_var) = yes], [$2], [$3])dnl
AS_VAR_POPDEF([ac_var])dnl
])# AM_MINISIP_CHECK_WINFUNC

# AM_MINISIP_CHECK_WINFUNCS(FUNCTION..., [ACTION-IF-FOUND], [ACTION-IF-NOT-FOUND],[INCLUDES])
# ---------------------------------------------------------------------
AC_DEFUN([AM_MINISIP_CHECK_WINFUNCS],
[AC_FOREACH([AC_Func], [$1],
 [m4_pushdef([AC_Func_Name],m4_bpatsubst(m4_bpatsubst(AC_Func, ["], []),
					 [(.*)],
					 []))dnl
  AH_TEMPLATE([AS_TR_CPP(HAVE_[]AC_Func_Name)],
               [Define to 1 if you have the `]AC_Func_Name[' function.])dnl
  m4_popdef([AC_Func_Name])])dnl
for ac_func in $1
do
ac_func_name=`echo $ac_func|sed -e 's/^\(.*\)(.*)$/\1/'`
AM_MINISIP_CHECK_WINFUNC($ac_func,
              [AC_DEFINE_UNQUOTED([AS_TR_CPP([HAVE_$ac_func_name])]) $2],
              [$3], [$4])dnl
done
])
