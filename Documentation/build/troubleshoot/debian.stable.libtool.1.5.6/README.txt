SOLVING THE PROBLEMS WITH LIBTOOL 1.5.6

In Debian Stable (sarge, May 2006), the version of libtool files installed
contains a bug (the .libs/.libs problem). 

In this folder, the solution is provided: a patched version of the needed
files (and a patch.diff file for you to check the differences).

* What to do?
Copy libtool to     /usr/bin 
Copy libtoolize to  /usr/bin
Copy ltmain.sh to /usr/share/libtool
Copy (again) ltmain.sh to /usr/share/libtool/libltdl

* What we have done? 
The version shipped in the debian stable sarge is 1.5.6.
In this patched version, the version has been "bumped" to 1.5.6.1, 
   this way our configuration tools can see that the version has been
   patched, thus removing some work arounds (see in libmutil/m4/libmutil.m4)

Cesc
May 2006
