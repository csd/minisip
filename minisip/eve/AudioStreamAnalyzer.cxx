
#include"AudioStreamAnalyzer.h"

#include<libmutil/itoa.h>
#include<iostream>
#include<sys/time.h>
#include<time.h>
#include"EveGui.h"
#include"../rtp/RtpPacket.h"
#include"../codecs/G711CODEC.h"
#include<pthread.h>


using namespace std;

#define INTER_ARRIVAL_TIME 30
#define TIME_DIFF_ACCEPT 30

#define LOCK pthread_mutex_lock(&mlock)
#define UNLOCK pthread_mutex_unlock(&mlock)


static bool timeMatch(Stream stream, struct timeval now){
	struct timeval previous = stream.lastTime;
//	struct timeval now;
//	gettimeofday(&now,NULL);
	
	int usec = (now.tv_sec-previous.tv_sec)*1000000 + now.tv_usec - previous.tv_usec;
	int msec = usec / 1000;
//	cerr << "diff="<< msec << " ";
	return msec > INTER_ARRIVAL_TIME-TIME_DIFF_ACCEPT && msec < INTER_ARRIVAL_TIME+ TIME_DIFF_ACCEPT;
	
}

static string ipPortToString(uint32_t ip, uint16_t port){
	unsigned char *ucp = (unsigned char *)(&ip);
	return string(itoa(ucp[0]))+"."+itoa(ucp[1])+"."+itoa(ucp[2])+"."+itoa(ucp[3])+":"+itoa(port);
	
}

bool inList(list<Stream> &l, uint32_t from_ip, uint16_t from_port, uint32_t to_ip, uint16_t to_port){
	
	for (list<Stream>::iterator i=l.begin(); i!=l.end(); i++){
		if ( (*i).from_ip ==from_ip && 
			(*i).from_port == from_port &&
			(*i).to_ip == to_ip &&
			(*i).to_port == to_port )
			return true;
	}
	return false;
}

void AudioStreamAnalyzer::handlePacket(uint32_t from_ip, uint16_t from_port,
			uint32_t to_ip, uint16_t to_port,
			void *data, int n, int protocol){
	
//	cerr << "AudioStreamAnalyzer got packet"<< endl;
	LOCK;
//	packet pack(from_ip,from_port,to_ip,to_port,data,n);
	struct timeval now;
	gettimeofday(&now,NULL);

	for (list<Stream>::iterator i=active.begin(); i!=active.end(); i++){
		if ((*i).sameStreamAs(from_ip, from_port, to_ip, to_port)){
			if (timeMatch(*i,now)){
				(*i).value+=2;
				(*i).backoff=1;
			}else{
				(*i).value-=(*i).backoff;
				(*i).backoff*=2;
			}
			(*i).lastTime.tv_usec = now.tv_usec;
			(*i).lastTime.tv_sec = now.tv_sec;
			
			if ((*i).value>=75){
				(*i).value=75;
			}
	
			
			if (gui->selectedForPlaying( ipPortToString(from_ip,from_port), ipPortToString(to_ip, to_port))){
				unsigned char *udpcontent = (unsigned char *) data;
				struct rtpheader* rtphdr = (struct rtpheader *) &udpcontent;
				if (n >=  (int)sizeof(struct rtpheader)){
					unsigned char *rtpcontent = &udpcontent[sizeof(struct rtpheader)];
					audioReceiver->handleCodedAudio(rtphdr->pt,rtphdr->seq_no,
							rtphdr->ssrc,
							rtpcontent,
							n-sizeof(struct rtpheader)
							);
				}else{
					
					cerr << "Too small "<< n << endl;
				}
			}else{
//				cerr << "Not (" << ipPortToString( (*i).from_ip, (*i).from_port)<<"->"<< ipPortToString((*i).to_ip, (*i).to_port ) << ")" << flush;
			}
			
			UNLOCK;
			return;
		}
		
	}
	
//	cerr << "N Potential: "<< potential.size()<< endl;
	for (list<Stream>::iterator i=potential.begin(); i!=potential.end(); i++){
		if ((*i).sameStreamAs(from_ip, from_port, to_ip, to_port)){
			//cerr << "Found matching stream"<< endl;
			if (timeMatch(*i, now)){
			//	cerr << "Time match"<< endl;
				(*i).value+=2;
				(*i).backoff=1;
			}else{
			//	cerr << "Time mis-match, decreasing one from "<< (*i).value << endl;
				(*i).value-=(*i).backoff;
				(*i).backoff*=2;
			}
			(*i).lastTime.tv_usec = now.tv_usec;
			(*i).lastTime.tv_sec = now.tv_sec;
			if ( (*i).value<=0 ){
				cerr << "Value is zero - removing"<< endl;
				potential.erase(i);
				UNLOCK;
				return;
			}
			if ((*i).value>=50){
//				cerr << "Value is 50 - adding to active"<< endl;
				cerr << "EEEEXXXXXXXXXXXXXXXXXXXXXXXXXXXEEEEEEE" << endl;
				if (!inList(active, from_ip, from_port, to_ip, to_port)){
					active.push_back(*i);
					cerr << "EEEE2XXXXXXXXXXXXXXXXXXXXXXXXXXXEEEEEEE" << endl;
					gui->addStream( ipPortToString( from_ip, from_port), 
							   ipPortToString( to_ip, to_port) );

					//soundcard->registerSource( (*i).from_port );
				}
				potential.erase(i);
				UNLOCK;
				return;
			}
			UNLOCK;
			return;
		}
	}

//	cerr << "Adding packet to potential queue"<< endl;
	potential.push_back(Stream(from_ip,from_port, to_ip, to_port,10));
	UNLOCK;


}

static void print_ip(uint32_t ip){
	unsigned char *ucp = (unsigned char *)(&ip);
	cerr << (int)ucp[0]<<"."<<(int)ucp[1]<<"."<<(int)ucp[2]<<"."<<(int)ucp[3];
}



static void print_stream_info_short(Stream s){
	print_ip(s.from_ip);
	cerr<<":"<<s.from_port<<" -> ";
	print_ip(s.to_ip);
	cerr << ":" << s.to_port << "; value="<< s.value;
}

static void *run(void*arg){
	AudioStreamAnalyzer *a = (AudioStreamAnalyzer *)arg;
	a->runMaintainer();
	return NULL;
}

void AudioStreamAnalyzer::startMaintainerThread(){
	pthread_t t;
	pthread_create(&t, NULL, run, (void*)this);
}

void AudioStreamAnalyzer::runMaintainer(){
	while (1){
		LOCK;
		cerr << endl <<"Before maintainer: Statistics: n_active="<< active.size()<< " n_potential="<<potential.size()<< endl;
		if (active.size()>0){
			cerr << "Active streams: "<< endl;
			for (list<Stream>::iterator i=active.begin(); i!=active.end(); i++){
				cerr <<"\t";
				print_stream_info_short(*i);
				cerr << endl;
			}
		}
		
		if (potential.size()>0){
			cerr << "Potential streams: "<< endl;
			for (list<Stream>::iterator i=potential.begin(); i!=potential.end(); i++){
				cerr <<"\t";
				print_stream_info_short(*i);
				cerr << endl;
			}
		}

		
		for (list<Stream>::iterator i = active.begin(); i!=active.end(); i++){
			(*i).value-=(*i).backoff;
			(*i).backoff*=2;
		}
		bool done;
		do{
			done=true;
			for (list<Stream>::iterator i = active.begin(); i!=active.end(); i++){
				if ((*i).value<=0){
					cerr << "Maintainer: Removing stream"<< endl;
					gui->removeStream( ipPortToString( (*i).from_ip, (*i).from_port) , 
							   ipPortToString( (*i).to_ip, (*i).to_port) );
					//soundcard->unRegisterSource( (*i).from_port );
					active.erase(i);
					done=false;
					break;
				}
			}
		}while(!done);
		
		for (list<Stream>::iterator i = potential.begin(); i!=potential.end(); i++){
			(*i).value-=(*i).backoff;
			(*i).backoff*=2;
		}
		do{
			done=true;
			for (list<Stream>::iterator i = potential.begin(); i!=potential.end(); i++){
				if ((*i).value<=0){
					cerr << "Maintainer: Removing stream"<< endl;
					potential.erase(i);
					done=false;
					break;
				}
			}
		}while(!done);
		
		cerr << endl <<"After Maintainer: Statistics: n_active="<< active.size()<< " n_potential="<<potential.size()<< endl;
		if (active.size()>0){
			cerr << "Active streams: "<< endl;
			for (list<Stream>::iterator i=active.begin(); i!=active.end(); i++){
				cerr <<"\t";
				print_stream_info_short(*i);
				cerr << endl;
			}
		}
		
		if (potential.size()>0){
			cerr << "Potential streams: "<< endl;
			for (list<Stream>::iterator i=potential.begin(); i!=potential.end(); i++){
				cerr <<"\t";
				print_stream_info_short(*i);
				cerr << endl;
			}
		}
		UNLOCK;
		sleep(2);
	}
}


void AudioReceiver::handleCodedAudio(int format, int index, int stream, unsigned char *data, int nsamples){
	short buf[1024];
	static int last_index;
	if (last_index==index)
		return;

	if (format==0){
		G711CODEC g711;
		g711.decode(data, 160, buf);

		player->pushSound(stream, buf,160, index);
		//player->pushSound(from_port /*34*/, buf, 160, index, false);
		//
	}else{
		cerr << "Unknown audio type: "<< format << endl;
	}
	last_index=index;
}


