if OS_WIN
res_obj = minisip-res.o
noinst_DATA = $(res_obj)
CLEANFILES = $(res_obj)
endif OS_WIN

EXTRA_DIST = minisip.ico

.rc.o:
	$(WINDRES) -I$(srcdir) -I$(top_srcdir)/share $< $@

SUFFIXES = .rc .o

MAINTAINERCLEANFILES = $(srcdir)/Makefile.in
