AM_CPPFLAGS = $(MINISIP_CFLAGS)

bin_PROGRAMS = ministun ministund

# 
# ministun rules
ministun_SOURCES = ministun.cxx
ministun_LDFLAGS = $(MINISIP_LIBS)

if OS_WIN
ministun_res_obj = ../win32/$(PACKAGE)-res.o
ministun_LDFLAGS += -Wl,$(ministun_res_obj)
ministun_DEPENDENCIES = $(ministun_res_obj)
endif OS_WIN
# end of ministun rules
#

# 
# ministund rules
ministund_SOURCES = ministund.cxx
ministund_LDFLAGS = $(MINISIP_LIBS)

if OS_WIN
ministund_res_obj = ../win32/$(PACKAGE)d-res.o
ministund_LDFLAGS += -Wl,$(ministund_res_obj)
ministund_DEPENDENCIES = $(ministund_res_obj)
endif OS_WIN
# end of ministund rules
#

MAINTAINERCLEANFILES = $(srcdir)/Makefile.in
