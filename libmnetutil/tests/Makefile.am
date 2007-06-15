AM_CPPFLAGS = -I$(top_srcdir)/include $(MINISIP_CFLAGS)
AM_LDFLAGS = -L$(top_srcdir) $(MINISIP_LIBS)
LDADD = ../libmnetutil.la

MINISIP_TESTS = \
	000_compile \
	002_naptr

if ENABLE_LDAP
MINISIP_TESTS += \
	001_ldap
001_ldap_SOURCES = 001_ldap.cxx
endif


TESTS = $(MINISIP_TESTS)
noinst_PROGRAMS = $(MINISIP_TESTS)

000_compile_SOURCES = 000_compile.cxx
002_naptr_SOURCES = 002_naptr.cxx

MAINTAINERCLEANFILES = $(srcdir)/Makefile.in
