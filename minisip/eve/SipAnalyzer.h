#ifndef _SIPANALYZER_H
#define _SIPANALYZER_H

#include"Sniffer.h"

class EveGui;

class SipAnalyzer : public PacketReceiver{
	public:	

		SipAnalyzer(EveGui*);

		virtual void handlePacket(uint32_t from_ip, uint16_t from_port,
				uint32_t to_ip, uint16_t to_port,
				void *data, int n, int protocol );
	private:

		EveGui *gui;
				


};


#endif
