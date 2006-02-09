#!/bin/sh

#This scripts compiles and install all the minisip libraries and GTK GUI
#  for a win32 system.
#Modify the parameters (basically the prefix_*) to adapt to your cross-compile environment.
#It is not really good if you are developing or debugging, as everytime you run the script it 
#  recompiles ALL the source code.

#This is the CROSS_COMPILE_FOLDER
prefix_cross=/opt/minisip_w32/cross
prefix_cross_include=$prefix_cross/include
prefix_cross_lib=$prefix_cross/lib

host_option="--host=i586-mingw32msvc"
build_option="--build=i686-pc-linux-gnu"

library_configure="--disable-static"
minisip_gtk_configure="$library_configure --enable-gtk --disable-gconf --disable-alsa --enable-dsound"

library_name=libmutil
cd $library_name
echo "$PWD"
./bootstrap
./configure $host_option $build_option prefix=$prefix_cross $library_configure CPPFLAGS=-I$prefix_cross_include LDFLAGS=-L$prefix_cross_lib
echo "CXXFLAGS=-I$prefix_cross_include LDFLAGS=-L$prefix_cross_lib ./configure $host_option $build_option prefix=$prefix_cross $library_configure"
make
make install
cd ..

library_name=libmnetutil
cd $library_name
echo "$PWD"
./bootstrap
./configure $host_option $build_option prefix=$prefix_cross $library_configure CPPFLAGS=-I$prefix_cross_include LDFLAGS=-L$prefix_cross_lib
echo "CXXFLAGS=-I$prefix_cross_include LDFLAGS=-L$prefix_cross_lib ./configure $host_option $build_option prefix=$prefix_cross $library_configure"
make
make install
cd ..

library_name=libmikey
cd $library_name
echo "$PWD"
./bootstrap
./configure $host_option $build_option prefix=$prefix_cross $library_configure CPPFLAGS=-I$prefix_cross_include LDFLAGS=-L$prefix_cross_lib
echo "CXXFLAGS=-I$prefix_cross_include LDFLAGS=-L$prefix_cross_lib ./configure $host_option $build_option prefix=$prefix_cross $library_configure"
make
make install
cd ..

library_name=libmsip
cd $library_name
echo "$PWD"
./bootstrap
./configure $host_option $build_option prefix=$prefix_cross $library_configure CPPFLAGS=-I$prefix_cross_include LDFLAGS=-L$prefix_cross_lib
echo "CXXFLAGS=-I$prefix_cross_include LDFLAGS=-L$prefix_cross_lib ./configure $host_option $build_option prefix=$prefix_cross $library_configure"
make
make install
cd ..

cd minisip
./bootstrap
echo "./configure $host_option $build_option prefix=$prefix_cross $minisip_gtk_configure CPPFLAGS=-I$prefix_cross_include -I$prefix_cross_include/directx LDFLAGS=-L$prefix_cross_lib"
./configure $host_option $build_option prefix=$prefix_cross $minisip_gtk_configure CPPFLAGS="-I$prefix_cross_include -I$prefix_cross_include/directx" LDFLAGS=-L$prefix_cross_lib
make
make install
cd ..
