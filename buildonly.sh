#! /usr/bin/env bash
#
# Simple script to build minisip.
#
# This script needs to be executed at the root of the repository trunk.  It
# will build minisip and with all the associated libraries.  The build will be
# done "in place", that is, no files will be installed on your system.
#
# This script only runs a make into every folder. It needs buildall.sh to be
#first. Note that libraries, at minimum, will be relinked, to ensure that all
#changes are caught.

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

	cd ${subdir}
		
	#force to relink ... 	
	rm -f ${subdir}.la
	
	LDFLAGS="$LDFLAGS -L$PWD/.libs"
	CXXFLAGS="$CXXFLAGS -I$PWD/include"

	make $1
	
	echo
	echo
	echo "LDFLAGS = $LDFLAGS" 
	echo "CXXFLAGS = $CXXFLAGS"
	echo
	
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

LDFLAGS="$LDFLAGS -L$PWD/.libs"
CXXFLAGS="$CXXFLAGS -I$PWD/include"
echo
echo
echo "LDFLAGS = $LDFLAGS" 
echo "CXXFLAGS = $CXXFLAGS"
echo

make $1
