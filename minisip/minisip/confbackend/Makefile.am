noinst_LIBRARIES = libconfbackend.a

libconfbackend_a_SOURCES = ConfBackend.cxx \
			 MXmlConfBackend.cxx

if GCONF_SUPPORT
AM_CPPFLAGS += @GCONF_CFLAGS@
libconfbackend_a_SOURCES += GConfBackend.cxx
endif

noinst_HEADERS  = ConfBackend.h \
		  MXmlConfBackend.h
