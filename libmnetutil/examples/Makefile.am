EXAMPLES = tcpclient.cpp tcpserver.cpp

TESTS = build-examples.sh

exampledir = $(pkgdatadir)/examples
example_SCRIPTS = run-example.sh build-examples.sh
example_DATA = $(EXAMPLES)
abs_top_builddir=@abs_top_builddir@
MINISIP_CFLAGS += -I$(includedir) $(CFLAGS) $(CPPFLAGS)
MINISIP_LIBS += -L$(libdir) -L$(abs_top_builddir)/.libs -lmnetutil -lresolv 
MINISIP_LIBS += $(LDFLAGS) $(LIBS)

run-example.sh: run-example.sh.in
	sed -e 's,@libdir\@,$(libdir),' $< > $@

build-examples.sh: build-examples.sh.in
	sed -e 's,@EXAMPLES\@,$(EXAMPLES),' \
		-e 's,@MINISIP_CFLAGS\@,$(MINISIP_CFLAGS),' \
		-e 's,@MINISIP_LIBS\@,$(MINISIP_LIBS),' \
		$< > $@

%.cpp: %.cxx
	sed -e 's,@PACKAGE\@,@PACKAGE@,' \
		-e 's,@PACKAGE_VERSION\@,@PACKAGE_VERSION@,' \
		$< > $@


MAINTAINERCLEANFILES = $(srcdir)/Makefile.in
