eve:	EveGui.cxx EveGui.h eve.cxx Sniffer.cxx eve.h AudioStreamAnalyzer.h AudioStreamAnalyzer.cxx SipAnalyzer.h SipAnalyzer.cxx
	g++ -Wall -g -o eve -I../include -lmutil -lmnetutil -lresolv -lssl -lpthread `pkg-config --cflags --libs gtk+-2.0` EveGui.cxx eve.cxx Sniffer.cxx AudioStreamAnalyzer.cxx SipAnalyzer.cxx ../codecs/*.o ../codecs/*/*.o ../soundcard/*.o ../spaudio/*.o -lsamplerate
tstsniffer: tstsniffer.cxx Sniffer.h Sniffer.cxx
	g++ -Wall -g -o tstsniffer -I../include -lmutil -lmnetutil -lresolv -lssl -lpthread `pkg-config --cflags --libs gtk+-2.0` tstsniffer.cxx Sniffer.cxx ../codecs/*.o ../codecs/*/*.o ../soundcard/*.o ../spaudio/*.o -lsamplerate
clean: 
	rm -f *.o eve
install:
	cp eve /usr/bin

