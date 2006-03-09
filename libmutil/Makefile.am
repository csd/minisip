abs_top_srcdir = @abs_top_srcdir@
pkgconfigdir = $(libdir)/pkgconfig

SUBDIRS = include win32
DIST_SUBDIRS = $(SUBDIRS) debian
EXTRA_DIST = libmutil.spec

pkgconfig_DATA = libmutil.pc

lib_LTLIBRARIES = libmutil.la
libmutil_la_CPPFLAGS = -I$(abs_top_srcdir)/include
libmutil_la_CXXFLAGS = $(PTHREAD_CFLAGS)
libmutil_la_LDFLAGS = $(PTHREAD_CFLAGS) -no-undefined -version-info $(LT_VERSION_INFO)
libmutil_la_LIBADD = $(PTHREAD_LIBS)
libmutil_la_SOURCES = $(mutil_src)

if OS_WIN
res_obj = win32/libmutil-res.o
libmutil_la_LDFLAGS += -Wl,$(res_obj)
libmutil_la_DEPENDENCIES = $(res_obj)
endif OS_WIN

if HAVE_PTHREAD
thread_src = 	source/PosixCondVar.cxx \
	 	source/PosixMutex.cxx \
		source/PosixSemaphore.cxx
else
if OS_WIN
thread_src = 	source/Win32CondVar.cxx \
		source/Win32Mutex.cxx \
		source/Win32Semaphore.cxx
endif OS_WIN
endif !HAVE_PTHREAD

mutil_src =  	source/aes.cxx \
		source/MemObject.cxx \
		source/Library.cxx \
		source/Exception.cxx \
		source/massert.cxx \
		source/IString.cxx \
		source/rijndael-alg-fst.cxx \
		source/base64.cxx \
		source/dbg.cxx \
		source/merror.cxx \
		source/cert.cxx \
		source/hmac.cxx \
		source/Thread.cxx \
		source/TextUI.cxx \
		source/Timestamp.cxx \
		source/mtime.cxx \
		source/itoa.cxx \
		source/print_hex.cxx \
		source/split_in_lines.cxx \
		source/trim.cxx \
		source/vmd5.cxx \
		source/CommandString.cxx \
		source/XMLParser.cxx \
		source/MPlugin.cxx \
		source/CircularBuffer.cxx \
		$(thread_src)

MAINTAINERCLEANFILES = \
		$(srcdir)/configure \
		$(srcdir)/Makefile.in \
		$(srcdir)/missing \
		$(srcdir)/ltmain.sh \
		$(srcdir)/depcomp \
		$(srcdir)/config.guess \
		$(srcdir)/config.sub \
		$(srcdir)/install-sh \
		$(srcdir)/aclocal.m4

