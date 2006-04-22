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

#If you are debugging, you probably want to build with "g++ -g", to build
#with all debug symbols, useful for coredumps with gdb.
#Comment this line if you don't want to build with debug code
compiler_debug="-ggdb"

#Also useful, you may want to call make with some options ... supply them
#here. 
#For example, -k forces make to keep compiling even there are errors in the 
#   sources. I like this one.
make_options="-k"

#Possible configure commands are shown in: ./configure --help
#   check each folder for more details.

#This are common params, usable in all folders
base_configure_params=""
base_configure_params="$base_configure_params --enable-debug"
base_configure_params="$base_configure_params --disable-static"

#set special options for libmutil
libmutil_configure_params=""
#libmutil_configure_params="$libmutil_configure_params --enable-memdebug"

#set special options for libminisip
#   do a ./configure --help to see ALL available options ... here
#              show just a sample
libminisip_configure_params=""
libminisip_configure_params="$libminisip_configure_params --enable-color-terminal"
libminisip_configure_params="$libminisip_configure_params --enable-alsa"
      #--enable-autocall enables automatic calling for debug purposes (default disabled)
      #--enable-ipaq enables various fixes for the iPAQ (default disabled)
      #--enable-ipsec-enable enables ipsec features (default disabled)
      #--enable-aec enables push-2-talk features (default enabled)
      #--enable-video enables video features (default disabled)
      #--enable-buzzer enables IPAQ buzzer (default disabled)
      #--enable-dsound enables DirectSound sound support (default disabled)
      #--enable-portaudio      enable PortAudio V19 support (default auto).
      #--enable-gconf enables support for GConf (default enabled).
      #--enable-sdl enables SDL video output if the required library is found
      #--enable-video enables video features (default disabled)
 
#set special options for minisip
#   do a ./configure --help to see ALL available options ... here
#              show just a sample
minisip_configure_params=""
minisip_configure_params="$minisip_configure_params --enable-gtk"
	#minisip_configure_params="$minisip_configure_params --enable-textui"
	#Here you could have per gui-specific parameters ... 
	
# bootstrap needs to be given paths to m4 directories
bootstrap_params=""

for subdir in ${SUBDIRS}
do
	echo "+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++"
	echo "Building ${subdir} ... "
	echo "+++++++++++++++++++++++++++++++++++++"
		
	cd ${subdir}

	./bootstrap $bootstrap_params
	bootstrap_params="$bootstrap_params -I ../${subdir}/m4"
	
	configure_params="$base_configure_params"
	
	if [ ${subdir} = "libmutil" ]; then 
		LOC_MUTIL_LIBS=-L$PWD/.libs
		#${subdir}.$LIBS_EXTENSION
		LOC_MUTIL_CFLAGS=-I$PWD/include
		echo libmutil can use special params
		configure_params="$configure_params $libmutil_configure_params"
		MUTIL_LIBS="$LOC_MUTIL_LIBS -lmutil"	\
			MUTIL_CFLAGS=$LOC_MUTIL_CFLAGS \
			CXXFLAGS="-Wall $compiler_debug" 	\
					./configure $configure_params
	fi 

	if [ ${subdir} = "libmnetutil" ]; then 
		LOC_MNETUTIL_LIBS=-L$PWD/.libs
		LOC_MNETUTIL_CFLAGS=-I$PWD/include
		MUTIL_LIBS="$LOC_MUTIL_LIBS -lmutil"	\
			MUTIL_CFLAGS=$LOC_MUTIL_CFLAGS \
			CXXFLAGS="-Wall $compiler_debug" 	\
					./configure $configure_params
	fi 

	if [ ${subdir} = "libmikey" ]; then 
		LOC_MIKEY_LIBS=-L$PWD/.libs
		LOC_MIKEY_CFLAGS=-I$PWD/include
		MUTIL_LIBS="$LOC_MUTIL_LIBS -lmutil"	\
			MUTIL_CFLAGS=$LOC_MUTIL_CFLAGS \
			CXXFLAGS="-Wall $compiler_debug" 	\
					./configure $configure_params
	fi 

	if [ ${subdir} = "libmsip" ]; then 
		LOC_MSIP_LIBS=-L$PWD/.libs
		LOC_MSIP_CFLAGS=-I$PWD/include
		MUTIL_LIBS="$LOC_MUTIL_LIBS -lmutil"	\
			MUTIL_CFLAGS=$LOC_MUTIL_CFLAGS \
			MNETUTIL_LIBS="$LOC_MNETUTIL_LIBS -lmnetutil"	\
			MNETUTIL_CFLAGS=$LOC_MNETUTIL_CFLAGS \
			CXXFLAGS="-Wall $compiler_debug" 	\
					./configure $configure_params
	fi 

	if [ ${subdir} = "libminisip" ] ; then 
		echo libminisip can also have special config params
		LOC_LIBMINISIP_LIBS=-L$PWD/.libs
		LOC_LIBMINISIP_CFLAGS=-I$PWD/include
		configure_params_libminisip="$configure_params $libminisip_configure_params"
		MUTIL_LIBS="$LOC_MUTIL_LIBS -lmutil"	\
			MUTIL_CFLAGS=$LOC_MUTIL_CFLAGS \
			MNETUTIL_LIBS="$LOC_MNETUTIL_LIBS -lmnetutil"	\
			MNETUTIL_CFLAGS=$LOC_MNETUTIL_CFLAGS \
			MIKEY_LIBS="$LOC_MIKEY_LIBS -lmikey"	\
			MIKEY_CFLAGS=$LOC_MIKEY_CFLAGS \
			MSIP_LIBS="$LOC_MSIP_LIBS -lmsip"	\
			MSIP_CFLAGS=$LOC_MSIP_CFLAGS \
			CXXFLAGS="-Wall $compiler_debug" 	\
					./configure $configure_params_libminisip
	fi 
	
	if [ ${subdir} = "minisip" ]; then 
		echo minisip can also have special config params
		configure_params_minisip="$configure_params $minisip_configure_params"
		MUTIL_LIBS="$LOC_MUTIL_LIBS -lmutil"	\
			MUTIL_CFLAGS=$LOC_MUTIL_CFLAGS \
			MNETUTIL_LIBS="$LOC_MNETUTIL_LIBS -lmnetutil"	\
			MNETUTIL_CFLAGS=$LOC_MNETUTIL_CFLAGS \
			MIKEY_LIBS="$LOC_MIKEY_LIBS -lmikey"	\
			MIKEY_CFLAGS=$LOC_MIKEY_CFLAGS \
			MSIP_LIBS="$LOC_MSIP_LIBS -lmsip"	\
			MSIP_CFLAGS=$LOC_MSIP_CFLAGS \
			LIBMINISIP_LIBS="$LOC_LIBMINISIP_LIBS -lminisip"	\
			LIBMINISIP_CFLAGS=$LOC_LIBMINISIP_CFLAGS \
			CXXFLAGS="-Wall $compiler_debug" 	\
					./configure $configure_params_minisip
	fi 
	
	echo "=========================================================="
	echo "configure_params (${subdir})= $configure_params"
	echo "=========================================================="
	
	make $make_options
	
	cd ..
done

echo "---------------------------------------------------------------------"
echo "Warning:"
echo "   You may need to manually copy the minisip/share/minisip.glade file"
echo "   to the _pkgdatadir_ folder (in debian: /usr/local/share)."
echo " --------------------------------------------------------------------"
echo
