#ifndef _SNIFFER_H
#define _SNIFFER_H

#include<list>
#include<stdint.h>
#include<string>

class SnifferException{

};

class PacketReceiver{
	public:

		virtual void handlePacket(uint32_t from_ip, uint16_t from_port, 
					  uint32_t to_ip, uint16_t to_port, 
					  void *data, int n, int protocol ) = 0;
};

class Sniffer{
	public:
		static const int PROTOCOL_IP;
		static const int PROTOCOL_UDP;
		static const int PROTOCOL_TCP;
		
		Sniffer(std::string device);

		void addHandler(PacketReceiver *, int protocol);

		void run();
		void stop();
		void createRunThread();

	private:
		void read_packet();
		void init_promisq(const char *device);

		bool running;
		int frame_sockfd;
		std::list<class PacketReceiver *> udpReceivers;
		std::list<class PacketReceiver *> tcpReceivers;

		int n_udp;
		int n_tcp;
		int n_ip;
		int n_frames;
		
		pthread_t tid;

};

#endif
