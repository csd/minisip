EXTRA_DIST = run-example.sh.in build-examples.sh.in \
		tcpclient.cxx tcpserver.cxx
EXAMPLE_SCRIPT_FILES = run-example.sh build-examples.sh
EXAMPLE_SOURCE_FILES = tcpclient.cpp tcpserver.cpp

TESTS = build-examples.sh

exampledir = $(pkgdatadir)/examples
example_SCRIPTS = $(EXAMPLE_SCRIPT_FILES)
example_DATA = $(EXAMPLE_SOURCE_FILES)
abs_top_builddir=@abs_top_builddir@
MINISIP_CFLAGS += -I$(includedir) $(CFLAGS) $(CPPFLAGS)
MINISIP_LIBS += -L$(libdir) -L$(abs_top_builddir)/.libs -lmnetutil -lresolv 
MINISIP_LIBS += $(LDFLAGS) $(LIBS)

run-example.sh: run-example.sh.in
	sed -e 's,@libdir\@,$(libdir),' $< > $@

build-examples.sh: build-examples.sh.in
	sed -e 's|@EXAMPLES\@|$(EXAMPLE_SOURCE_FILES)|' \
		-e 's|@MINISIP_CFLAGS\@|$(MINISIP_CFLAGS)|' \
		-e 's|@MINISIP_LIBS\@|$(MINISIP_LIBS)|' \
		$< > $@
	chmod 0755 $@

%.cpp: %.cxx
	sed -e 's,@PACKAGE\@,@PACKAGE@,' \
		-e 's,@PACKAGE_VERSION\@,@PACKAGE_VERSION@,' \
		$< > $@

CLEANFILES = $(EXAMPLE_SCRIPT_FILES) $(EXAMPLE_SOURCE_FILES) \
		tcpclient tcpserver

MAINTAINERCLEANFILES = $(srcdir)/Makefile.in
