abs_top_srcdir = @abs_top_srcdir@

SUBDIRS = include
DIST_SUBDIRS = $(SUBDIRS) debian
EXTRA_DIST = libmutil.spec

lib_LTLIBRARIES = libmutil.la
libmutil_la_CPPFLAGS = -I$(abs_top_srcdir)/include
libmutil_la_LDFLAGS = -no-undefined

libmutil_la_SOURCES = $(mutil_src)

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
		source/Mutex.cxx \
		source/CondVar.cxx \
		source/Semaphore.cxx \
		source/CommandString.cxx \
		source/XMLParser.cxx \
		source/MPlugin.cxx \
		source/CircularBuffer.cxx

