bin_PROGRAMS = minisip_textui

minisip_textui_CPPFLAGS = -I$(top_srcdir) $(MINISIP_CFLAGS)
minisip_textui_SOURCES = \
		MinisipTextUI.cxx \
		MinisipTextMain.cxx
minisip_textui_LDFLAGS = $(RELOC_LDFLAGS)
minisip_textui_LDADD = $(MINISIP_LIBS)

noinst_HEADERS = \
		MinisipTextUI.h

MAINTAINERCLEANFILES = \
		$(srcdir)/Makefile.in
