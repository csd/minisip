#! /usr/bin/env bash
#
# Simple script to run minisip
#
minisip_plugin_path_choose=$PWD/libminisip/.libs

SUBDIRS="${SUBDIRS} libmutil"
SUBDIRS="${SUBDIRS} libmnetutil"
SUBDIRS="${SUBDIRS} libmikey"
SUBDIRS="${SUBDIRS} libmsip"
SUBDIRS="${SUBDIRS} libminisip"

MY_LD_LIB_PATH="$LD_LIBRARY_PATH"

for subdir in ${SUBDIRS}
do
	cd ${subdir}
	MY_LD_LIB_PATH="$MY_LD_LIB_PATH$PWD/.libs:"
	cd ..
done

cd minisip
MY_LD_LIB_PATH="$MY_LD_LIB_PATH$PWD/minisip/.libs"

cd ..
echo "export LD_LIBRARY_PATH=$MY_LD_LIB_PATH"

MINISIP_PLUGIN_PATH="$minisip_plugin_path_choose" \
	LD_LIBRARY_PATH="$MY_LD_LIB_PATH" \
		minisip/minisip/gui/gtkgui/minisip_gtkgui
#LD_LIBRARY_PATH="$MY_LD_LIB_PATH" minisip/minisip/gui/textui/minisip_textui


