EXAMPLE_SCRIPT_FILES = run-example.sh build-examples.sh
EXAMPLE_SOURCE_FILES = \
		mutextest.cpp \
		semaphoretest.cpp \
		threadtest.cpp

TESTS = build-examples.sh \
		test-examples.sh

exampledir = $(pkgdatadir)/examples
example_SCRIPTS = $(EXAMPLE_SCRIPT_FILES)
example_DATA = $(EXAMPLE_SOURCE_FILES)

abs_top_builddir=@abs_top_builddir@/.libs
MINISIP_CFLAGS += -I$(includedir) $(CFLAGS) $(CPPFLAGS)
MINISIP_LIBS += -L$(libdir) -L$(abs_top_builddir) -lmutil 
MINISIP_LIBS += $(LDFLAGS) $(LIBS)

%.sh: %.sh.in
	sed -e 's,@EXAMPLES\@,$(EXAMPLE_SOURCE_FILES),' \
		-e 's,@MINISIP_CFLAGS\@,$(MINISIP_CFLAGS),' \
		-e 's,@MINISIP_LIBS\@,$(MINISIP_LIBS),' \
		-e 's,@buildlibdir\@,$(abs_top_builddir),' \
		-e 's,@libdir\@,$(libdir),' \
		$< > $@
	chmod 0755 $@

%.cpp: %.cxx
	sed -e 's,@PACKAGE\@,@PACKAGE@,' \
		-e 's,@PACKAGE_VERSION\@,@PACKAGE_VERSION@,' \
		$< > $@

CLEANFILES = $(EXAMPLE_SCRIPT_FILES) $(EXAMPLE_SOURCE_FILES)

MAINTAINERCLEANFILES = $(srcdir)/Makefile.in
