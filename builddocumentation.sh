#!/bin/bash

#Script to automatically build all the (doxygen) documentation from the 
#minisip project

#You need the doxygen and graphviz tools installed. 

#cd to the running folder ...
cd Documentation

doxygen doxygen.d/doxygen.develguide.conf
doxygen doxygen.d/doxygen.libmutil.conf
doxygen doxygen.d/doxygen.libmnetutil.conf
doxygen doxygen.d/doxygen.libmikey.conf
doxygen doxygen.d/doxygen.libmsip.conf
doxygen doxygen.d/doxygen.libminisip.conf
doxygen doxygen.d/doxygen.minisip.conf

echo "MiniSIP builddocumentation finished (maybe with errors ... i didn't check)"
echo "      The main index web page can be located under:"
echo "      $PWD/docs.html/index.html"
echo ""
echo "Enjoy!"

cd ..
