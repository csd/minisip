if OS_WIN
res_obj = libminisip-res.o
noinst_DATA = $(res_obj)
CLEANFILES = $(res_obj)
endif OS_WIN

.rc.o:
	$(WINDRES) -I$(srcdir) -I$(top_srcdir)/share $< $@

SUFFIXES = .rc .o
