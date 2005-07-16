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
	LDFLAGS="$LDFLAGS -L$PWD/.libs"
	CXXFLAGS="$CXXFLAGS -I$PWD/include"
	LDFLAGS="$LDFLAGS" CXXFLAGS="$CXXFLAGS" ./configure --enable-debug
	make
	cd ..
done

cd minisip
./bootstrap
LDFLAGS=$LDFLAGS CXXFLAGS=$CXXFLAGS \
	./configure --enable-debug --enable-color-terminal
make
