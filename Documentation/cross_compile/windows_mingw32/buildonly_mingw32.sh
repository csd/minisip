#! /usr/bin/env bash
#

#Modified version from the buildonly.sh script available at the trunk root. This 
#one is to be used when cross-compiling with mingw32
	#For a proper installation, select yes
	#if you are debugging, compiling looking for errors, select "no", it will speed up the whole thing,
	#	(though the files generated won't be runnable in windows?)
do_make_install="yes"

# Simple script to build minisip.
#
# This script needs to be executed at the root of the repository trunk.  It
# will build minisip and with all the associated libraries.  The build will be
# done "in place", that is, no files will be installed on your system.
#
# This script only runs a make into every folder. It needs buildall.sh to be
#first. Note that libraries, at minimum, will be relinked, to ensure that all
#changes are caught.
#
#Note that you can send a parameter to make ... for example "clean" or "distclean"
#
#The LD_LIBRARY_PATH you may want to use it to execute minisip/minisip/minisip
#       script.

SUBDIRS="${SUBDIRS} libmutil"
SUBDIRS="${SUBDIRS} libmnetutil"
SUBDIRS="${SUBDIRS} libmikey"
SUBDIRS="${SUBDIRS} libmsip"
#SUBDIRS="${SUBDIRS} libminisip"

strip_generated_files="no"
strip_bin="i586-mingw32msvc-strip"

#Also useful, you may want to call make with some options ... supply them
#here. 
#For example, -k forces make to keep compiling even there are errors in the 
#   sources. I like this one.
make_options="-k"

for subdir in ${SUBDIRS}
do
	echo "+++++++++++++++++++++++++++++++++++++"
	echo "Building ${subdir} ... "
	echo "+++++++++++++++++++++++++++++++++++++"

	cd ${subdir}
		
	#force to relink ... 	
	rm -f ${subdir}.la
	
	LD_LIBRARY_PATH="$LD_LIBRARY_PATH$PWD/.libs:"
	
	make $make_options $1
	
	if [ x$do_make_install = "xyes" ]; then 
		make install
	fi
	
	cd ..
done

echo "+++++++++++++++++++++++++++++++++++++"
echo "Building MiniSIP application ... "
echo "+++++++++++++++++++++++++++++++++++++"

cd minisip

#force to relink
rm -f ./soundcard/libsoundcard.a
rm -f ./conf/libconf.a
rm -f ./sdp/libsdp.a
rm -f ./aec/libaec.a
rm -f ./stun/libstun.a
rm -f ./rtp/librtp.a
rm -f ./minisip/ipprovider/libipprovider.a
rm -f ./minisip/gui/gtkgui/libminisip_gtkgui.a
rm -f ./minisip/gui/libminisip_gui.a
rm -f ./minisip/contactdb/libcontactdb.a
rm -f ./minisip/libminisip.a
rm -f ./mediahandler/libmediahandler.a
rm -f ./sip/libsip.a
rm -f ./codecs/g711/libcodec_g711.a
rm -f ./codecs/ilbc/libcodec_ilbc.a
rm -f ./codecs/libcodecs.a
rm -f ./spaudio/libspaudio.a
rm -f minisip/minisip

LD_LIBRARY_PATH="$LD_LIBRARY_PATH$PWD"

echo
echo "LD_PATH = $LD_LIBRARY_PATH"
echo

make $make_options $1

if [ x$do_make_install = "xyes" ]; then 
	make install
fi

cd ..

mkdir -p compiled_files
tmp_lib="libmutil"
cp -f $tmp_lib/.libs/$tmp_lib-0.dll compiled_files
cp -f $tmp_lib/.libs/$tmp_lib.dll.a compiled_files
tmp_lib="libmnetutil"
cp -f $tmp_lib/.libs/$tmp_lib-0.dll compiled_files
cp -f $tmp_lib/.libs/$tmp_lib.dll.a compiled_files
tmp_lib="libmikey"
cp -f $tmp_lib/.libs/$tmp_lib-0.dll compiled_files
cp -f $tmp_lib/.libs/$tmp_lib.dll.a compiled_files
tmp_lib="libmsip"
cp -f $tmp_lib/.libs/$tmp_lib-0.dll compiled_files
cp -f $tmp_lib/.libs/$tmp_lib.dll.a compiled_files

cp -f minisip/minisip/minisip.exe compiled_files/minisip_script.exe
cp -f minisip/minisip/.libs/minisip.exe compiled_files/minisip.exe

$strip_bin compiled_files/*

echo "export LD_LIBRARY_PATH=$LD_LIBRARY_PATH"



