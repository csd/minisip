#!/bin/bash

#Script to automatically build all the (doxygen) documentation from the 
#minisip project

#You need the doxygen and graphviz tools installed. 

#cd to the running folder ...
cd Documentation

doxygen doxygen.conf/doxygen.develguide.conf
doxygen doxygen.conf/doxygen.libmutil.conf
doxygen doxygen.conf/doxygen.libmnetutil.conf
doxygen doxygen.conf/doxygen.libmikey.conf
doxygen doxygen.conf/doxygen.libmsip.conf
doxygen doxygen.conf/doxygen.libminisip.conf
doxygen doxygen.conf/doxygen.minisip.conf

echo "MiniSIP builddocumentation finished (maybe with errors ... i didn't check)"
echo "      The main index web page can be located under:"
echo "      $PWD/docs.html/index.html"
echo ""
echo "Enjoy!"

cd ..
