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

	#Possible configure commands are shown in: ./configure --help
	#  Usefull: 
	#     enable-debug (lot more output, helpfull with problems)
	#     enble-shared (compile only static libs ... speeds up compilation)

	configure_params=""
#	configure_params="$configure_params --enable-debug "
#	configure_params="$configure_params --disable-shared"
	
	if [ ${subdir} = "libmutil" ]; then 
		echo libmutil can use special params
		#here you can add the --enable-memdebug, only for libmutil
#		configure_params="$configure_params --enable-memdebug"
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

echo "+++++++++++++++++++++++++++++++++++++"
echo "Building MiniSIP application ... "
echo "+++++++++++++++++++++++++++++++++++++"

cd minisip
./bootstrap

configure_params="--enable-color-terminal"
#configure_params="$configure_params --enable-alsa"
#configure_params="$configure_params --enable-debug"
#configure_params="$configure_params --disable-shared"

echo
echo
echo "LDFLAGS = $LOC_LDFLAGS" 
echo "CXXFLAGS = $LOC_LDFLAGS"
echo
echo "=========================================================="
echo "configure_params (minisip)= $configure_params"
echo "=========================================================="

LDFLAGS=$LOC_LDFLAGS 					\
	CXXFLAGS="$LOC_CXXFLAGS -Wall"			\
	./configure $configure_params

make


