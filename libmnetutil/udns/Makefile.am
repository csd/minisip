#! /usr/bin/make -rf
# $Id: Makefile,v 1.38 2005/09/12 12:09:10 mjt Exp $
# libudns Makefile
#
# Copyright (C) 2005  Michael Tokarev <mjt@corpit.ru>
# Copyright (C) 2006  Mikael Magnusson <mikma@users.sourceforge.net>
# This file is part of UDNS library, an async DNS stub resolver.
#
# This library is free software; you can redistribute it and/or
# modify it under the terms of the GNU Lesser General Public
# License as published by the Free Software Foundation; either
# version 2.1 of the License, or (at your option) any later version.
#
# This library is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public
# License along with this library, in file named COPYING.LGPL; if not,
# write to the Free Software Foundation, Inc., 59 Temple Place,
# Suite 330, Boston, MA  02111-1307  USA

noinst_LTLIBRARIES = \
		libudns.la

AM_CPPFLAGS = $(MINISIP_CFLAGS) -D_WIN32_WINNT=0x501 -DHANDLE_ERROR=NULL -DEOVERFLOW=-1 -I$(srcdir)/source -DNO_INET6 
AM_LDFLAGS = $(MINISIP_LIBRARY_LDFLAGS)

VERS   = 0.0.8

SRCS = source/udns_dn.c				\
	source/udns_dntosp.c			\
	source/udns_parse.c			\
	source/udns_resolver.c			\
	source/udns_misc.c			\
	source/udns_rr_a.c			\
	source/udns_rr_ptr.c			\
	source/udns_rr_mx.c			\
	source/udns_rr_txt.c			\
	source/udns_bl.c			\
	source/udns_rr_srv.c			\
	source/udns_rr_naptr.c			\
	source/udns.h				\
	udns_codes.c
USRCS  = dnsget.c rblcheck.c ex-rdns.c
DIST   = COPYING.LGPL udns.h udns.3 dnsget.1 rblcheck.1 $(USRCS) \
	Makefile TODO

libudns_la_SOURCES = $(SRCS)

libudns_la_LIBADD = $(MINISIP_LIBS)

#EXTRADIST = $(addprefix source/,$(DIST))
CLEANFILES = udns_codes.c


# udns_codes.c is generated from udns.h
udns_codes.c:	source/udns.h Makefile
	@echo Generating $@
	@set -e; exec >$@.tmp; \
	set T type C class R rcode; \
	echo "/* Automatically generated. */"; \
	echo "#include \"udns.h\""; \
	while [ "$$1" ]; do \
	 echo; \
	 echo "const struct dns_nameval dns_$${2}tab[] = {"; \
	 $(AWK) "/^  DNS_$${1}_[A-Z0-9_]+[ 	]*=/ \
	  { printf \" {%s,\\\"%s\\\"},\\n\", \$$1, substr(\$$1,7) }" \
	  $(srcdir)/source/udns.h ; \
	 echo " {0,0}};"; \
	 echo "const char *dns_$${2}name(enum dns_$${2} code) {"; \
	 echo " static char nm[20];"; \
	 echo " switch(code) {"; \
	 $(AWK) "BEGIN{i=0} \
	   /^  DNS_$${1}_[A-Z0-9_]+[ 	]*=/ \
	   {printf \" case %s: return dns_$${2}tab[%d].name;\\n\",\$$1,i++}\
	   " $(srcdir)/source/udns.h ; \
	 echo " }"; \
	 echo " return _dns_format_code(nm,\"$$2\",code);"; \
	 echo "}"; \
	 shift 2; \
	done
	@mv $@.tmp $@
