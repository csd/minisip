bin_PROGRAMS = minisip_opengl

AM_CPPFLAGS = -DMINISIP_DATADIR="\"$(pkgdatadir)\""     \
                -I$(top_srcdir)/include \
                $(MINISIP_CFLAGS)

AM_LDFLAGS = $(RELOC_LDFLAGS)


minisip_opengl_CPPFLAGS = -I$(top_srcdir) $(MINISIP_CFLAGS)
minisip_opengl_SOURCES = \
		OpenGlGui.cxx \
		Main.cxx
minisip_opengl_LDFLAGS = $(RELOC_LDFLAGS)
minisip_opengl_LDADD = $(MINISIP_LIBS) $(OPENGL_LIBS)

noinst_HEADERS = \
		OpenGlGui.h

MAINTAINERCLEANFILES = \
		$(srcdir)/Makefile.in
