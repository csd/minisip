#if FALSE
if SWIG

AM_CPPFLAGS = -I$(top_srcdir)/include $(MINISIP_CFLAGS) -I$(srcdir)
SWIG_FLAGS = $(AM_CPPFLAGS) -DLIBMUTIL_API -DLIBMIKEY_API -DLIBMNETUTIL_API -DLIBMSIP_API -DLIBMINISIP_API
AM_LDFLAGS = -Wl,-no-undefined -no-undefined $(RELOC_LDFLAGS) -avoid-version -module

BUILT_SOURCES = 
CLEANFILES =
MAINTAINERCLEANFILES =
EXTRA_DIST = $(swig_sources)

swig_sources = minisip.i

#minisip_libs = $(MINISIP_LIBS) -lmcrypto -lcrypto -lssl -lwsock32	\
#-lwinmm -ldnsapi -liphlpapi -lwinmm -ldxguid -luuid -ldsound -lm

minisip_libs = $(MINISIP_LIBS)

#
# Ruby
#
#if TRUE
if FALSE
rubydir = $(libdir)/ruby/1.8/i486-linux/
ruby_LTLIBRARIES = minisip_ruby.la

RUBY_CFLAGS = -I/usr/lib/ruby/1.8/i486-linux/
RUBY_LIBS = -lruby1.8

minisip_ruby_la_CPPFLAGS = $(AM_CPPFLAGS) $(RUBY_CFLAGS)
minisip_ruby_la_LDFLAGS = $(AM_LDFLAGS)
minisip_ruby_la_LIBADD = $(RUBY_LIBS) $(minisip_libs)
minisip_ruby_la_SOURCES = minisip_ruby.cxx minisip_ruby.h

BUILT_SOURCES += $(minisip_ruby_la_SOURCES)
MAINTAINERCLEANFILES += $(minisip_ruby_la_SOURCES)

minisip_ruby.cxx: minisip.i
	$(SWIG) -c++ -ruby $(SWIG_FLAGS) -o minisip_ruby.cxx minisip.i
endif


#
# Python
#
# Use one of?
# ax_python_config_var
# ax_with_python
# az_python

#if TRUE
if FALSE
pythondir = $(libdir)/python2.4/site-packages
python_LTLIBRARIES = python_minisip.la
python_DATA = MinisipModule.py

PYTHON_CPPFLAGS = -I/usr/include/python2.4/
PYTHON_LIBS = -lpython2.4

python_minisip_la_CPPFLAGS = $(AM_CPPFLAGS) $(PYTHON_CPPFLAGS)
python_minisip_la_CXXFLAGS = $(AM_CXXFLAGS) -pthread
python_minisip_la_LDFLAGS = $(AM_LDFLAGS)
python_minisip_la_LIBADD = $(PYTHON_LIBS) $(minisip_libs)
python_minisip_la_SOURCES = python_minisip.cxx python_minisip.h
python_minisip_la_SWFLAGS = $(SWIG_FLAGS)
# -threads
# -interface python_minisip

BUILT_SOURCES += $(python_minisip_la_SOURCES)
MAINTAINERCLEANFILES += $(python_minisip_la_SOURCES) minisip.py

python_minisip.cxx MinisipModule.py: minisip.i
	$(SWIG) -c++ -python $(python_minisip_la_SWFLAGS) -o python_minisip.cxx minisip.i
endif

#
# C#
#
if MCS
#if FALSE

minisipdir = $(pkglibdir)
minisip_SCRIPTS = minisip-sharp.dll

BUILT_SOURCES += $(minisipsharpglue_generated_source) $(minisip_generated_sources)
#MAINTAINER
CLEANFILES += $(minisipsharpglue_generated_source)	\
$(minisip_generated_sources) minisip-sharp.dll minisip-sharp.dll.mdb

if OS_WIN
minisip_LTLIBRARIES = minisipsharpglue.la
minisipsharpglue_la_LIBADD = $(minisip_libs)
minisipsharpglue_la_LDFLAGS = $(AM_LDFLAGS) -Wl,--kill-at
minisipsharpglue_la_SOURCES = $(minisipsharpglue_generated_source)
else
minisip_LTLIBRARIES = libminisipsharpglue.la
libminisipsharpglue_la_CXXFLAGS = $(AM_CXXFLAGS) -pthread
libminisipsharpglue_la_LIBADD = $(minisip_libs)
libminisipsharpglue_la_SOURCES = $(minisipsharpglue_generated_source)
endif
minisipsharpglue_la_CSFLAGS = -I$(top_srcdir)/csharp $(SWIG_FLAGS)

minisipsharpglue_generated_source = minisipsharpglue.cxx	\
minisipsharpglue.h

minisip_sources_in = AssemblyInfo.cs.in
minisip_generated_sources = $(minisip_sources_in:.in=) MinisipModule.cs
minisip_build_sources = $(minisip_generated_sources)

LINK.cc = $(CXX) $(AM_CXXFLAGS) $(CXXFLAGS) $(AM_CPPFLAGS) $(CPPFLAGS)	\
$(AM_LDFLAGS) $(LDFLAGS) $(TARGET_ARCH)

%.cs: 

minisipsharpglue.cxx: $(swig_sources)
	$(SWIG) -c++ -csharp $(minisipsharpglue_la_CSFLAGS) -dllimport minisipsharpglue -namespace Minisip -o $@ minisip.i

minisip-sharp.dll: $(minisip_build_sources)
	mcs -debug -target:library -out:$@ *.cs

endif



endif

SUBST = sed						\
	-e 's,[@]VERSION[@],$(VERSION),g'\
	-e 's,[@]MINISIP_SWIG_MAJOR_VERSION[@],$(MINISIP_SWIG_MAJOR_VERSION),g'\
	-e 's,[@]MINISIP_SWIG_MINOR_VERSION[@],$(MINISIP_SWIG_MINOR_VERSION),g'\
	-e 's,[@]MINISIP_SWIG_MICRO_VERSION[@],$(MINISIP_SWIG_MICRO_VERSION),g'\
	-e 's,[@]CPPFLAGS[@],$(CPPFLAGS),g'

.in.cs:
	$(SUBST) $< > $@

.in.py:
	$(SUBST) $< > $@

SUFFIXES = .in


# _minisip.so: python_minisip.o
# 	g++ $(LDFLAGS) -shared -o $@ $< $(LIBS) $(PYTHON_LIBS)

# minisip.so: minisip_ruby.o
# 	g++ $(LDFLAGS) -shared minisip_ruby.o -o minisip.so $(LIBS) $(RUBY_LIBS)
# minisip_ruby.o: minisip_ruby.cxx
# 	g++ $(CPPFLAGS) $(CXXFLAGS) $(RUBY_CFLAGS) -c minisip_ruby.cxx

