#! /usr/bin/env bash
#
# Simple script to build minisip.
#
# This script needs to be executed at the root of the repository trunk.  It
# will build minisip and with all the associated libraries.  The build will be
# done "in place", that is, no files will be installed on your system.
#
# Instructions: 
#  Use buildall.sh script the first time you get the sources of minisip. It will
# create all files needed and compile all the sources. 
# Then, if you modify a file in the sources and do not want to recompile all 
# MiniSIP again, use the "buildonly.sh" script

# Note that you con modify the configuration of minisip by editing the 
# configure_params variable in this script. Some useful options are commented
# out by default, but feel free to use them.

SUBDIRS="${SUBDIRS} libmutil"
SUBDIRS="${SUBDIRS} libmnetutil"
SUBDIRS="${SUBDIRS} libmikey"
SUBDIRS="${SUBDIRS} libmsip"
SUBDIRS="${SUBDIRS} libminisip"
SUBDIRS="${SUBDIRS} minisip"

#Possible configure commands are shown in: ./configure --help
#   check each folder for more details.

#This are common params, usable in all folders
base_configure_params=""
#base_configure_params="$base_configure_params --enable-debug"
#base_configure_params="$base_configure_params --disable-shared"

#set special options for libmutil
libmutil_configure_params=""
#libmutil_configure_params="$libmutil_configure_params --enable-memdebug"

#set special options for libminisip and minisip
#   do a ./configure --help to see ALL available options ... here
#              show just a sample
minisip_configure_params=""
minisip_configure_params="$minisip_configure_params --enable-color-terminal"
#minisip_configure_params="$minisip_configure_params --enable-alsa"
      #--enable-autocall enables automatic calling for debug purposes (default disabled)
      #--enable-ipaq enables various fixes for the iPAQ (default disabled)
      #--enable-ipsec-enable enables ipsec features (default disabled)
      #--enable-aec enables push-2-talk features (default enabled)
      #--enable-video enables video features (default disabled)
      #--enable-textui enables the text based user interface (default disabled)

      
for subdir in ${SUBDIRS}
do
	echo "+++++++++++++++++++++++++++++++++++++"
	echo "Building ${subdir} ... "
	echo "+++++++++++++++++++++++++++++++++++++"
	echo
	echo "LDFLAGS = $LOC_LDFLAGS" 
	echo "CXXFLAGS = $LOC_CXXFLAGS"
	echo
	echo "+++++++++++++++++++++++++++++++++++++"
		
	cd ${subdir}
	./bootstrap


	configure_params="$base_configure_params"
	
	if [ ${subdir} = "libmutil" ]; then 
		echo libmutil can use special params
		configure_params="$configure_params $libmutil_configure_params"
	fi 
	
	if [ ${subdir} = "libminisip" ] ; then 
		echo libminisip can also have special config params
		configure_params="$configure_params $minisip_configure_params"
	fi 
	
	if [ ${subdir} = "minisip" ]; then 
		echo minisip can also have special config params
		configure_params="$configure_params $minisip_configure_params"
	fi 
	
	
	echo "=========================================================="
	echo "configure_params (${subdir})= $configure_params"
	echo "=========================================================="
	
	LOC_LDFLAGS="$LOC_LDFLAGS -L$PWD -L$PWD/.libs"
	LOC_CXXFLAGS="$LOC_CXXFLAGS -I$PWD/include"
	
	LDFLAGS=$LOC_LDFLAGS 					\
		CXXFLAGS="$LOC_CXXFLAGS -Wall"			\
			./configure $configure_params
	
	make
	
	cd ..
done

echo "---------------------------------------------------------------------"
echo "Warning:"
echo "   You may need to manually copy the minisip/share/minisip.glade file"
echo "   to the _pkgdatadir_ folder (in debian: /usr/local/share)."
echo " --------------------------------------------------------------------"
echo
