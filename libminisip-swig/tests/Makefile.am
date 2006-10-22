if SWIG

noinst_SCRIPTS = test_csharp.exe pygui.py
TESTS = $(MINISIP_TESTS)
MINISIP_TESTS =
BUILT_SOURCES = 
CLEANFILES =
TESTS_ENVIRONMENT =

#
# Python
#

#MINISIP_TESTS += pygui.py
TESTS_ENVIRONMENT += PYTHONPATH=..:.

#
# C#
#
if MCS

MINISIP_TESTS += test_csharp.exe
TESTS_ENVIRONMENT += MONO_PATH=.. LD_LIBRARY_PATH=../.libs
CLEANFILES += test_csharp.exe

AM_CSFLAGS = -lib:.. -r:minisip-sharp.dll -r:System.Windows.Forms.dll

test_csharp.exe: test_csharp.cs
	$(MCS) $(CSFLAGS) $(AM_CSFLAGS) -out:$@ $^
endif MCS

endif SWIG

MAINTAINERCLEANFILES = $(srcdir)/Makefile.in
