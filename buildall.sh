#! /usr/bin/env bash
#
# Simple script to build minisip.
#
# This script needs to be executed at the root of the repository trunk.  It
# will build minisip and with all the associated libraries.  The build will be
# done "in place", that is, no files will be installed on your system.
#

SUBDIRS="${SUBDIRS} libmutil"
SUBDIRS="${SUBDIRS} libmnetutil"
SUBDIRS="${SUBDIRS} libmikey"
SUBDIRS="${SUBDIRS} libmsip"
SUBDIRS="${SUBDIRS} libminisip"

for subdir in ${SUBDIRS}
do
	echo Building $subdir...
	cd ${subdir}
	./bootstrap
	./configure --enable-debug
	make
	MINISIP_LDFLAGS="$MINISIP_LDFLAGS -L$PWD/.libs"
	MINISIP_CXXFLAGS="$MINISIP_CXXFLAGS -I$PWD/include"
	echo $MINISIP_LDFLAGS $MINISIP_CXXFLAGS
	cd ..
done

cd minisip
./bootstrap
LDFLAGS=$MINISIP_LDFLAGS CXXFLAGS=$MINISIP_CXXFLAGS \
	./configure --enable-debug --enable-color-terminal
make
