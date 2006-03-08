Author: Cesc Santasusana   (cesc DOT santa AATT gma il DOT com)
Date: March 2006
===============================================================================

This Readme file contains as much info as I can remember over the 
whole port process ... as well as how to configure the MS EVC++ 4.0
to be able to compile yourself the code.
Environment target, build tools, ... : 
	- MS Embedded Visual C++ 4.0 + SP4
	- Active Sync 4.1
	- Microsoft Pocket PC 2003 SDK.msi
	- Windows Mobile 2003 Second Edition Developer Resources
	(Tools above ... get them from: http://msdn.microsoft.com/mobility/windowsmobile/downloads/default.aspx#wm03 )
	- The Run-time Type Information library for the Pocket PC 2003 SDK
			http://support.microsoft.com/kb/830482/en-us
	- Platform: ARMv4 (not v4i or v4t ... didn't try). Other platforms should also work ...
	- OS: compiled in a winxp sp2 ... for a Pocket PC 2003 Second Edition

===============================================================================
IMPORTANT !!
* Forget about using the emulator (for x86) included in MS EMB VC++ 4.0.
* Get the one in win2005, Microsoft Community preview, which is an ARM emulator, thus
	you can basically run the same binaries on the emulator and on the device. 
So, forget the stuff about compiling for the emulator below
(the problem is openssl, which won't compile for Pocket PC with an X86 platfrom).
Microsoft Community preview web: 
	http://msdn.microsoft.com/mobility/windowsmobile/downloads/emulatorpreview/default.aspx

===============================================================================
COMPILE WCECOMPAT (1.2)
Needed to complete the wince C core library ... otherwise, openssl does not compile out of the box
- enter the wcecompat folder
- set the WCE variables (in the MS EVC 4.0\bin, execute the .bat file, either ARMv4 or emulator)
- run nmake
wcecompat does not place the libs in different subfolders depending on the build platform ...
i have added a build/ folder, where you can place the include and lib folder for each platform you build ... 
- create also a build/wcecompat/*.h  folder, so that we can specifically request wcecompat files (#include <wcecompat/xxx.h>

===============================================================================
COMPILE STLPort (5.0.1)
Use this port, as the included stl implementation is very poor ...
- cd build/lib
- set the MS EVC 4.0 environment
- execute > configure -c evc4
- run nmake -f nmake-evc4.mak
- wait ... compiling ... 
- do the same for the "emulator" platform (first ARM, or not)
- the resulting compilation is:
	-under stlport: these files are common (includes and so on)
	-under lib/ and bin/, there are platform specific builds for arm4 and x86-emu, as dll, lib, and so on.
	
===============================================================================
COMPILE OPENSSL (0.9.8a)
- For ARM, follow the instruction on openssl's "install.wce", it works out of the box
	(remember to set the WCECOMPAT to the wcecompat/build/arm4 folder ... )
	You can build the static libs (ms\ce.mak) and the shared ones (ms\cedll.mak)

- For the emulator: set TARGETCPU=X86 ... add some extra defs ... 
	* i had to modify crypto/dso/dso_win32.c ... line 1221 ... it would not compile, and it is just a simple change ... 
		(have not tested it back to ARMv4 ... may not work, but i am on a cruzade to compile for the emulator).
		
===============================================================================
MICROSOFT EMBEDDED VC++ 4.0
- Remember to add to each file the Ccrtrtti.lib library and the /GR /GX
- If needed by the lib, remember to add to the link libraries the libeay32.lib and ssleay32.lib
- Add (in the first position of all libraries to link) wcecompatex.lib (for WCECompat)
- If needed, add winsock.lib, ws2.lib, Iphlpapi.lib (this lib is to obtain lists of network interfaces ... )
- Configure Tools->Options->Directories:
	* for pocket pc 2003 second edition ... set the include folders:
			- STLPort501/stlport
			- wcecompat12/build/wcecompat_include/wcecompat
			- wcecompat12/build/wcecompat_include/
			- openssl098a/inc32
	* for pocket pc 2003 second edition ... set the library folders:
			- STLPort501/lib/evc4-arm
			- wcecompat12/build/arm4/lib
			- openssl098a/out32dll_ARMV4
NOTE: we need to include wcecompat include folder twice, so we can do #include<io.h> and also force inclusion by
				#include<wcecompat/io.h>

===============================================================================
PROBLEM: STLPort and OPENSSL
- it pops up when compiling with #include<map/vector/hash/map> and #include<openssl/err.h> ... both end up including identifier "hash" ... 
	Solution: Include openssl/err.h before any <list/map/hash/vector> ... it causes the following error:
		compilation under EVC 4.0 to fail, collision between STLPort and Openssl
		.....\minisip.evc4\openssl098a\inc32\openssl\err.h(297) : error C2955: 'hash' : use of class template requires template argument list
			....\minisip.evc4\stlport501\stlport\stl\_hash_fun.h(40) : see declaration of 'hash'

===============================================================================
PROBLEM: STLPort, WCECompat and errno
- stlport and errno ... stlport won't include native errno.h, because it does not exhist for normal win ce sdk ... 
	warning message: "eMbedded Visual C++ 3 and .NET don't have a errno.h header; STLport won't include native errno.h here"
	but, wcecompat implements it, so we can include it ...
	Now, as STLPort MUST be the first include directory, it prevails ... then, we have to force the compiler to get the
		wcecompat/errno.h ... we do that in the <config.h> header. It is a hack, and it makes errno.h to be included everywhere, 
		but hei, it just declares a few defines and an extern int ... so, no biggie. :)
	
===============================================================================
LIBMUTIL
- it compiles clean, just some spurious warnings due to STLPort ... see compilation_config.h to enable those warnings.
- the dbg.cxx and dbg.h files, when under _WIN32_WCE, do not use the latest dbg class ... i reverted them to an older version
	(release 1923), otherwise it would not compile
	This change also effects libmsip/SipSMCommand.h.cxx ... it declares dbg as a friend ... not a big change needed ... 

===============================================================================
LIBMNETUTIL
- clean compile ... or almost
- When using io.h, the #define in wcecompat cause all "close" strings to be 
	changed to "_close" ... the same for "open", "read", "write" ... which 
	makes some of our classes to look for the wrong member ... (for example, Socket::close)
	Solution: whenever including io.h, #undef the above strings.
	
===============================================================================
LIBMIKEY
- compiles clean ... 
	
===============================================================================
LIBMSIP
- The changes in libmutil::dbg ... 	This change also effects libmsip/SipSMCommand.h.cxx ... it declares dbg as a friend ... not a big change needed ... 
- Add wcecompat/errno.h ... from <config.h>
- Add openssl/err.h ... from <libmsip_config.h>

===============================================================================
MINISIP
- Added to include/config.h (made to look like the ones in libmxxx ) the openssl/err.h ... easy way to solve the problem
- The evc compiler is very strange. It forces us to #include some header files
	in the .cxx files to complete the definition os some "forward declarations" 
	(we add these headers only for win32_wce ... the other build-environments do 
	not need it).
	The headers included are not tailored, rather all placed in one common file and
	then this file included in any source file that needs it ... 
	See minisip/include/minisip_wce_extra_includes.h (for example, in MessageRouter.cxx)

===============================================================================

TO BE CONTINUED ... 




