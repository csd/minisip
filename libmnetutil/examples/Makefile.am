noinst_PROGRAMS = tcpclient tcpserver

tcpclient_SOURCES = tcpclient.cxx
tcpclient_CPPFLAGS = $(MINISIP_CFLAGS)
tcpclient_LDFLAGS = -lmnetutil -lresolv $(MINISIP_LIBS)

tcpserver_SOURCES = tcpserver.cxx
tcpserver_CPPFLAGS = $(MINISIP_CFLAGS)
tcpserver_LDFLAGS = -lmnetutil -lresolv $(MINISIP_LIBS)

MAINTAINERCLEANFILES = $(srcdir)/Makefile.in
