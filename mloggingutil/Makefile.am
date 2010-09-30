bin_PROGRAMS = minisipcrashsender 
minisipcrashsender_CPPFLAGS = -I/usr/local/include 
CXXFLAGS += `pkg-config --cflags --libs gtk+-2.0` 
minisipcrashsender_LDADD = -L/usr/local/lib -lmnetutil 
minisipcrashsender_SOURCES = CrashSender.h CrashSender.cxx minisipcrashsender.cxx 

#dist_noinst_SCRIPTS = autogen.sh
