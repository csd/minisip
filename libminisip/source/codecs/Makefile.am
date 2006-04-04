SUBDIRS = g711 ilbc

noinst_LTLIBRARIES = libcodecs.la

libcodecs_la_SOURCES = ILBCCODEC.cxx \
		      G711CODEC.cxx \
		      Codec.cxx
libcodecs_la_LIBADD =

if HAS_SPEEX
libcodecs_la_SOURCES += SPEEXCODEC.cxx
libcodecs_la_LIBADD += $(SPEEX_LIBS)
endif

if HAS_GSM
libcodecs_la_SOURCES += GsmCodec.cxx
libcodecs_la_LIBADD += $(GSM_LIBS)
endif

noinst_HEADERS =  G711CODEC.h \
                  ILBCCODEC.h \
		  SPEEXCODEC.h \
		  GsmCodec.h \
		  Codec.h
