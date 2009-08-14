pkgconfigdir = $(libdir)/pkgconfig

SUBDIRS = libltdl include win32 . examples m4 debian
DIST_SUBDIRS = $(SUBDIRS)
EXTRA_DIST = libmutil.spec

INCLUDES = $(LTDLINCL)

pkgconfig_DATA = libmutil.pc

AM_CPPFLAGS = -I$(top_srcdir)/include $(EXTERNAL_CFLAGS)

lib_LTLIBRARIES = libmutil.la
libmutil_la_CXXFLAGS = $(PTHREAD_CFLAGS)
libmutil_la_LDFLAGS = $(MINISIP_LIBRARY_LDFLAGS)
libmutil_la_LIBADD = $(PTHREAD_LIBS) $(LIBLTDL)
libmutil_la_DEPENDENCIES = $(LIBLTDL)
libmutil_la_SOURCES = $(mutil_src)

if OS_WIN
res_obj = win32/libmutil-res.o
libmutil_la_LDFLAGS += -Wl,$(res_obj)
libmutil_la_DEPENDENCIES += $(res_obj)
endif OS_WIN

if HAVE_PTHREAD
thread_src = 	source/CondVarPosix.cxx \
	 	source/MutexPosix.cxx \
		source/SemaphorePosix.cxx \
		source/ThreadPosix.cxx
else
if OS_WIN
thread_src = 	source/CondVarWin32.cxx \
		source/MutexWin32.cxx \
		source/SemaphoreWin32.cxx \
		source/ThreadWin32.cxx
endif OS_WIN
endif !HAVE_PTHREAD

if OS_WIN
mtime_src = 	source/mtimeWin32.cxx
else
mtime_src = 	source/mtimePosix.cxx
endif !OS_WIN

mutil_src = \
		source/CriticalSection.cxx \
		source/MemObject.cxx \
		source/LibraryPosix.cxx \
		source/MessageRouter.cxx \
		source/Exception.cxx \
		source/massert.cxx \
		source/IString.cxx \
		source/dbg.cxx \
		source/merror.cxx \
		source/TextUI.cxx \
		source/Timestamp.cxx \
		source/mtime.cxx \
		$(mtime_src) \
		source/stringutils.cxx \
		source/vmd5.cxx \
		source/CommandString.cxx \
		source/XMLParser.cxx \
		source/MPluginPosix.cxx \
		source/CircularBuffer.cxx \
		source/SipUri.cxx \
		source/CacheItem.cxx \
		source/FileSystemUtils.cxx \
		source/FileSystem.cxx \
		$(thread_src)

# maintainer rules
ACLOCAL_AMFLAGS = -I m4 $(ACLOCAL_FLAGS)
MAINTAINERCLEANFILES = \
		$(srcdir)/configure \
		$(srcdir)/Makefile.in \
		$(srcdir)/config/depcomp \
		$(srcdir)/config/config.sub \
		$(srcdir)/config/config.guess \
		$(srcdir)/config/install-sh \
		$(srcdir)/config/ltmain.sh \
		$(srcdir)/config/missing \
		$(srcdir)/missing \
		$(srcdir)/ltmain.sh \
		$(srcdir)/depcomp \
		$(srcdir)/config.guess \
		$(srcdir)/config.sub \
		$(srcdir)/install-sh \
		$(srcdir)/aclocal.m4

maintainer-clean-local:
	rm -rf $(srcdir)/libltdl

dist-hook:
	cp -a version $(distdir)/.version
