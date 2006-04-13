#! /usr/bin/env bash
#

#Modified version from the buildonly.sh script available at the trunk root. This 
#one is to be used when cross-compiling with mingw32
	#For a proper installation, select yes
	#if you are debugging, compiling looking for errors, select "no", it will speed up the whole thing,
	#	(though the files generated won't be runnable in windows?)
do_make_install="no"

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
SUBDIRS="${SUBDIRS} libminisip"

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
#force to relink
find . -name "*.la" -exec rm -f {} \;
find . -name "minisip_gtkgui" -exec rm -f {} \;
find . -name "minisip_textui" -exec rm -f {} \;

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
tmp_lib="libminisip"
cp -f $tmp_lib/.libs/*.dll compiled_files
cp -f $tmp_lib/.libs/*.dll.a compiled_files

					#set a minum 200k ... otherwise it finds the fake script.exe
find minisip -name minisip_gtkgui.exe -size +200k -exec cp -f {} compiled_files/ \;
find minisip -name minisip_textui.exe -size +200k -exec cp -f {} compiled_files/ \;

$strip_bin compiled_files/*

echo "export LD_LIBRARY_PATH=$LD_LIBRARY_PATH"



