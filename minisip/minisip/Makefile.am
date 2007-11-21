SUBDIRS = . win32 gui

MAINTAINERCLEANFILES = $(srcdir)/Makefile.in

if FALSE
noinst_PROGRAMS = minisipExample

AM_CPPFLAGS = $(MINISIP_CFLAGS)

minisipExample_LDADD = $(MINISIP_LIBS)
minisipExample_SOURCES = minisipExample.cxx \
			gui/textui/MinisipTextUI.h \
			gui/textui/MinisipTextUI.cxx
endif
