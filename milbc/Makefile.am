pluginsdir = $(libdir)/libminisip/plugins

AM_CPPFLAGS = $(MINISIP_CFLAGS) 

plugins_LTLIBRARIES =

plugins_LDFLAGS = -Wl,--no-undefined -no-undefined $(RELOC_LDFLAGS) -avoid-version -export-dynamic -module

#
# iLBC codec plugin
#
libcodec_ilbc_src = source/ILBCCODEC.cxx \
			source/ILBCCODEC.h \
			source/ilbc/anaFilter.cxx \
			source/ilbc/constants.cxx \
			source/ilbc/createCB.cxx \
			source/ilbc/doCPLC.cxx \
			source/ilbc/enhancer.cxx \
			source/ilbc/filter.cxx \
			source/ilbc/FrameFlassify.cxx \
			source/ilbc/gainquant.cxx \
			source/ilbc/getCBvec.cxx \
			source/ilbc/helpfun.cxx \
			source/ilbc/hpInput.cxx \
			source/ilbc/hpOutput.cxx \
			source/ilbc/iCBConstruct.cxx \
			source/ilbc/iCBSearch.cxx \
			source/ilbc/iLBC_decode.cxx \
			source/ilbc/iLBC_encode.cxx \
			source/ilbc/LPCdecode.cxx \
			source/ilbc/LPCencode.cxx \
			source/ilbc/lsf.cxx \
			source/ilbc/packing.cxx \
			source/ilbc/StateConstructW.cxx \
			source/ilbc/StateSearchW.cxx \
			source/ilbc/syntFilter.cxx \
			source/ilbc/StateConstructW.h \
			source/ilbc/StateSearchW.h \
			source/ilbc/helpfun.h \
			source/ilbc/iCBConstruct.h \
			source/ilbc/syntFilter.h \
			source/ilbc/anaFilter.h \
			source/ilbc/iLBC_decode.h \
			source/ilbc/constants.h \
			source/ilbc/getCBvec.h \
			source/ilbc/enhancer.h \
			source/ilbc/iCBSearch.h \
			source/ilbc/hpInput.h \
			source/ilbc/gainquant.h \
			source/ilbc/LPCencode.h \
			source/ilbc/createCB.h \
			source/ilbc/doCPLC.h \
			source/ilbc/iLBC_define.h \
			source/ilbc/packing.h \
			source/ilbc/LPCdecode.h \
			source/ilbc/hpOutput.h \
			source/ilbc/lsf.h \
			source/ilbc/filter.h \
			source/ilbc/iLBC_encode.h \
			source/ilbc/FrameClassify.h

plugins_LTLIBRARIES += milbc.la
milbc_la_LDFLAGS = $(plugins_LDFLAGS)
milbc_la_SOURCES = $(libcodec_ilbc_src)
milbc_la_LIBADD = $(LIBMINISIP_LIBS) $(MUTIL_LIBS)


# maintainer rules
ACLOCAL_AMFLAGS = -I m4 $(ACLOCAL_FLAGS)
MAINTAINERCLEANFILES  = $(srcdir)/configure \
			$(srcdir)/Makefile.in \
			$(srcdir)/config/depcomp \
			$(srcdir)/config/config.sub \
			$(srcdir)/config/config.guess \
			$(srcdir)/config/install-sh \
			$(srcdir)/config/ltmain.sh \
			$(srcdir)/config/missing \
			$(srcdir)/aclocal.m4 \
			$(srcdir)/config/compilation_config.h.in

dist-hook:
	cp -a version $(distdir)/.version
