#ifndef _AudioStreamAnalyzer_H
#define _AudioStreamAnalyzer_H

#include<stdint.h>
#include"Sniffer.h"
#include<list>

using namespace std;


class Stream{
	public:	
		Stream(uint32_t from_ip, unsigned short from_port, uint32_t to_ip, unsigned short to_port, int v) :
			from_ip(from_ip),from_port(from_port),
			to_ip(to_ip),to_port(to_port)/*,data(data),length(len)*/,value(v),backoff(1)
		{

		}


		bool sameStreamAs(uint32_t from_ip, uint16_t from_port, uint32_t to_ip, uint16_t to_port){
			return  this->from_port==from_port  &&  this->to_port == to_port &&
				this->from_ip==from_ip  &&  this->to_ip==to_ip;
		}

		struct timeval lastTime;

		uint32_t from_ip;
		unsigned short from_port;
		uint32_t to_ip;
		unsigned short to_port;
		//void *data;
		//int32_t length;

		int value;
		int backoff;
	
	private:


};

class SoundPlayer{
	
	public:	
		void pushSound(int stream,short *buf,int n, int index){

		}

};

class AudioReceiver{
	public:
		AudioReceiver(SoundPlayer *p):player(p){
		
		}

		void handleCodedAudio(int format, int index, int stream, unsigned char *data, int nsamples);

		private:
		SoundPlayer *player;
};



class EveGui;

class AudioStreamAnalyzer : public PacketReceiver{
	public:

		AudioStreamAnalyzer(EveGui *g, AudioReceiver *ar):gui(g),audioReceiver(ar){
			pthread_mutex_init(&mlock, NULL);
		}
		
		virtual ~AudioStreamAnalyzer(){}

		virtual void handlePacket(uint32_t from_ip, uint16_t from_port,
				uint32_t to_ip, uint16_t to_port,
				void *data, int n, int protocol );

		void startMaintainerThread();
		void runMaintainer();

	private:
		EveGui *gui;
		AudioReceiver *audioReceiver;
		list<Stream> potential;
		list<Stream> active;
		pthread_mutex_t mlock;
};


#endif
