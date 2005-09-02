SUBDIRS = g711 ilbc

noinst_LIBRARIES = libcodecs.a

libcodecs_a_SOURCES = ILBCCODEC.cxx \
		      G711CODEC.cxx \
		      Codec.cxx

if HAS_SPEEX
libcodecs_a_SOURCES += SPEEXCODEC.cxx
#endif

if HAS_GSM
libcodecs_a_SOURCES += GsmCodec.cxx
endif

noinst_HEADERS =  G711CODEC.h \
                  ILBCCODEC.h \
		  SPEEXCODEC.h \
		  GsmCodec.h \
		  Codec.h
