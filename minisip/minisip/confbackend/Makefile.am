noinst_LTLIBRARIES = libconfbackend.la

libconfbackend_la_SOURCES = ConfBackend.cxx \
			 MXmlConfBackend.cxx

if GCONF_SUPPORT
AM_CPPFLAGS += @GCONF_CFLAGS@
libconfbackend_la_SOURCES += GConfBackend.cxx
libconfbackend_la_LIBADD = $(GCONF_LIBS)
endif

noinst_HEADERS  = ConfBackend.h \
		  MXmlConfBackend.h \
		  GConfBackend.h
