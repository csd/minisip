SUBDIRS = include
DIST_SUBDIRS = $(SUBDIRS) debian
EXTRA_DIST = libminisip.spec

lib_LTLIBRARIES = libminisip.la
libminisip_la_CPPFLAGS = $(CPPFLAGS) -I../include/libminisip

libminisip_la_SOURCES = $(minisip_src)

minisip_src =          source/File.cxx \
                     source/File2.cxx

