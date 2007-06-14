AM_CPPFLAGS = -I$(top_srcdir)/include $(MINISIP_CFLAGS)
AM_LDFLAGS = -L$(top_srcdir) $(MINISIP_LIBS)
LDADD = ../libmnetutil.la

MINISIP_TESTS = \
	000_compile \
	002_naptr

if ENABLE_LDAP
MINISIP_TESTS += \
	001_ldap
endif


TESTS = $(MINISIP_TESTS)
noinst_PROGRAMS = $(MINISIP_TESTS)

000_compile_SOURCES = 000_compile.cxx

MAINTAINERCLEANFILES = $(srcdir)/Makefile.in
