/*
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Library General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

/* Copyright (C) 2004 
 *
 * Authors: Erik Eliasson <eliasson@it.kth.se>
 *          Johan Bilien <jobi@via.ecp.fr>
*/

#include<qapplication.h>

#include<malloc.h>
#include<stdio.h>
#include<stdlib.h>
#include<netinet/in.h> /*htons etc.*/
#include<unistd.h>
#include<assert.h>
#include<sys/socket.h>
#include<linux/if_ether.h>
#include<linux/if_packet.h>
#include<linux/if.h>
#include<sys/ioctl.h>
#include<errno.h>
#include<linux/socket.h>
#include<iostream>
#include<sys/types.h>
#include<list>
#include<sys/time.h>
#include<time.h>
#include<pthread.h>
#include"../rtp/RtpPacket.h"

#include"eve_qtgui.h"
#include"../soundcard/SoundIO.h"
#include"../soundcard/OssSoundDevice.h"
#include<libmutil/itoa.h>
#include"../codecs/G711CODEC.h"

/*
 1. get packet
 2. if in "potential"
   2.1 check time diff from last packet
     2.1.1 If 17-23ms increase "value"
     2.1.2 else decrease value
   2.2 if value==0 remove
   2.3 if value==20 place in "active"
 3. if not in "potential"
   3.1 place in "potential" with value of 10
   
*/
   
		// type(2byte) +2*48 bit MAC addr = 14 bytes
#define LINK_LAYER_HEADER_SIZE 14
#define IP_LAYER_HEADER_SIZE 20
#define UDPTRANSPORT_LAYER_HEADER_SIZE 8

#define INTER_ARRIVAL_TIME 30
#define TIME_DIFF_ACCEPT 30

#define LOCK pthread_mutex_lock(&mlock)
#define UNLOCK pthread_mutex_unlock(&mlock)

using namespace std;

struct packet{
	uint32_t from_ip;
	uint32_t to_ip;
	unsigned short from_port;
	unsigned short to_port;
	int32_t length;
	void *data;
	
	struct timeval time;
	
	int value;
};

list<struct packet> potential;
list<struct packet> active;
pthread_mutex_t mlock;

EveGui *gui=NULL;
SoundIO *soundcard;

long long n_udp=0;
long long n_pack=0;
long long n_audio=0;

bool inList(list<struct packet> &l, struct packet p){
	for (list<struct packet>::iterator i=l.begin(); i!=l.end(); i++){
		if ( (*i).from_ip ==p.from_ip && 
			(*i).from_port == p.from_port &&
			(*i).to_ip == p.to_ip &&
			(*i).to_port == p.to_port )
			return true;
	}
	return false;
}

bool same_stream(struct packet p1, struct packet p2){
	return  p1.from_ip==p2.from_ip && 
			p1.to_ip==p2.to_ip && 
			p1.from_port==p2.from_port && 
			p1.to_port == p2.to_port;
}

bool time_match(struct packet previous, struct packet pack){
	int usec = (pack.time.tv_sec-previous.time.tv_sec)*1000000 + pack.time.tv_usec - previous.time.tv_usec;
	int msec = usec / 1000;
//	cerr << "diff="<< msec << " ";
	return msec > INTER_ARRIVAL_TIME-TIME_DIFF_ACCEPT && msec < INTER_ARRIVAL_TIME+ TIME_DIFF_ACCEPT;
	
}

void print_ip(uint32_t ip){
	unsigned char *ucp = (unsigned char *)(&ip);
	cerr << (int)ucp[0]<<"."<<(int)ucp[1]<<"."<<(int)ucp[2]<<"."<<(int)ucp[3];
}

string ipPortToString(int32_t ip, uint16_t port){
	unsigned char *ucp = (unsigned char *)(&ip);
	return string(itoa(ucp[0]))+"."+itoa(ucp[1])+"."+itoa(ucp[2])+"."+itoa(ucp[3])+":"+itoa(port);
	
}

void print_packet_info(struct packet p){
	cerr << "From: ";
	print_ip(p.from_ip);
	cerr << ":" << p.from_port << endl;

	cerr << "To: ";
	print_ip(p.to_ip);
	cerr << ":" << p.to_port << endl;
	cerr << "Length: "<< p.length<< endl;
}

void print_packet_info_short(struct packet p){
	print_ip(p.from_ip);
	cerr<<":"<<p.from_port<<" -> ";
	print_ip(p.to_ip);
	cerr << ":" << p.to_port << "; value="<< p.value;
}

bool playEnabled(struct packet p){
	return gui->doPlay( ipPortToString(p.from_ip, p.from_port), ipPortToString(p.to_ip, p.to_port) );

}

int last_index=0;
void playAudio(int from_port, int type, int index, unsigned char *data){
	short buf[1024];
	
	if (last_index==index)
		return;
	
	if (type==0){
		G711CODEC g711;
		g711.decode(data, 160, buf);
//		cerr << "SSRC("<<ssrc<<")"<<flush;
		soundcard->pushSound(from_port /*34*/, buf, 160, index, false);
//		write(1,buf, 320);
	}else{
		cerr << "Unknown audio type: "<< type << endl;
	}
	last_index=index;
}

void handle_udp(struct packet pack){
	
	LOCK;

	for (list<struct packet>::iterator i=active.begin(); i!=active.end(); i++){
		if (same_stream(*i, pack)){
			if (time_match(*i, pack)){
				(*i).value+=2;
			}else{
				(*i).value--;
			}
			(*i).time.tv_usec = pack.time.tv_usec;
			(*i).time.tv_sec = pack.time.tv_sec;
			if ((*i).value>=75){
				(*i).value=75;
			}
	
			
			if (playEnabled(*i)){
				unsigned char *ipptr = (unsigned char *) (*i).data;
				struct rtpheader* rtphdr = (struct rtpheader *) &ipptr[IP_LAYER_HEADER_SIZE + UDPTRANSPORT_LAYER_HEADER_SIZE];
				if ((*i).length >= IP_LAYER_HEADER_SIZE + 
							UDPTRANSPORT_LAYER_HEADER_SIZE + 
							(int)sizeof(struct rtpheader) + 160 ){
					playAudio(pack.from_port, 
							rtphdr->pt, 
							rtphdr->seq_no,
							&ipptr[ IP_LAYER_HEADER_SIZE + 
								UDPTRANSPORT_LAYER_HEADER_SIZE +
								sizeof(struct rtpheader)]
						);
				}else{
					
					cerr << "Too small "<< (*i).length << endl;
				}
			}else{
//				cerr << "Not (" << ipPortToString( (*i).from_ip, (*i).from_port)<<"->"<< ipPortToString((*i).to_ip, (*i).to_port ) << ")" << flush;
			}
			
			UNLOCK;
			return;
		}
		
	}
	
	for (list<struct packet>::iterator i=potential.begin(); i!=potential.end(); i++){
		if (same_stream(*i, pack)){
//			cerr << "Found matching stream"<< endl;
			if (time_match(*i, pack)){
//				cerr << "Time match"<< endl;
				(*i).value+=2;
			}else{
//				cerr << "Time mis-match"<< endl;
				(*i).value--;
			}
			(*i).time.tv_usec = pack.time.tv_usec;
			(*i).time.tv_sec = pack.time.tv_sec;
			if ( (*i).value<=0 ){
//				cerr << "Value is zero - removing"<< endl;
				potential.erase(i);
				UNLOCK;
				return;
			}
			if ((*i).value>=50){
//				cerr << "Value is 50 - adding to active"<< endl;
				cerr << "EEEEXXXXXXXXXXXXXXXXXXXXXXXXXXXEEEEEEE" << endl;
				if (!inList(active, *i)){
					active.push_back(*i);
					gui->addStream( ipPortToString( (*i).from_ip, (*i).from_port) , 
							   ipPortToString( (*i).to_ip, (*i).to_port) );

					soundcard->registerSource( (*i).from_port );
				}
				potential.erase(i);
				UNLOCK;
				return;
			}
			UNLOCK;
			return;
		}
	}

	pack.value=10;

//	cerr << "Adding packet to potential queue"<< endl;
	potential.push_back(pack);
	UNLOCK;
}

int frame_sockfd;
int frame_ifindex;

void init_promisq(char *update_frame_device){
	struct packet_mreq mr;
	struct sockaddr_ll sll;
	struct ifreq ifr;
	if (update_frame_device!=NULL){

		if ((frame_sockfd=socket(PF_PACKET, SOCK_RAW, htons(ETH_P_IP)))==-1){
			perror("Could not create socket: ");
			exit(1);
		}
		strcpy(ifr.ifr_name, update_frame_device);

		if (ioctl(frame_sockfd, SIOGIFINDEX, &ifr) < 0) {
			fprintf(stderr,"voip_eve/init_promisq: failed to fetch ifindex: %s\n", strerror(errno));
			exit(1);;
		}

		frame_ifindex = ifr.ifr_ifindex;
		memset(&mr,0,sizeof(mr));
		mr.mr_ifindex = frame_ifindex;
		mr.mr_type = PACKET_MR_PROMISC;
		if (setsockopt(frame_sockfd, SOL_PACKET, PACKET_ADD_MEMBERSHIP, (char *)&mr, sizeof(mr)) < 0) {
			fprintf(stderr,"voip_eve/init_promisq: failed to add the promiscuous mode: %s ", strerror(errno)); 
			exit(1);
		}

		//bind...
		memset(&sll, 0, sizeof(sll));
		sll.sll_family = AF_PACKET;
		sll.sll_ifindex = frame_ifindex;
		sll.sll_protocol = htons(ETH_P_ALL);
		if (bind(frame_sockfd, (struct sockaddr*)&sll, sizeof(sll)) < 0) {
			perror("voip_eve/init_promisq: failed to bind the socket: ");
			exit(1);
		}
	}
};


void *read_packet(void *){
	unsigned char * buff; // pointer to received data
	int i; // frame length
	struct sockaddr_ll from; // source address of the message
	socklen_t fromlen = sizeof(struct sockaddr_ll); 

	buff = (unsigned char *)malloc(10000);
	unsigned char *ipbuf= (unsigned char *) &buff[100];
	while (1) {
		while (1) {
			fromlen = sizeof(from);
			i = recvfrom(frame_sockfd, &buff[100-LINK_LAYER_HEADER_SIZE], 1614, 0, (struct sockaddr *) &from, &fromlen);
			n_pack++;
//			cerr << "Received packet of type " << from.sll_protocol << "(family " << from.sll_family << ") PF_INET="<<PF_INET << endl;
			if (i == -1){
				fprintf(stderr,"voip_eve, cannot receive data: %s // \n", strerror(errno));
				// // sleep for 10 milliseconds before re-trying
				usleep(10000);
			}
			break; // exit the loop
		}
		if ((int)ipbuf[9]==17){ //if UDP
			n_udp++;
			struct packet pack;
			gettimeofday(&pack.time, NULL);
			pack.length=i-LINK_LAYER_HEADER_SIZE;
			pack.data = (void *)ipbuf;
			uint32_t *uip;
			
			uip = &(((uint32_t *)ipbuf)[3]);
			pack.from_ip = *uip;
			
			uip = &(((uint32_t *)ipbuf)[4]);
			pack.to_ip = *uip;
			
			unsigned short *usp = (unsigned short*)ipbuf;
			
			int ip_hdr_len = ipbuf[0] & 0x0F;
			
			pack.from_port = ntohs(usp[ip_hdr_len*2]);
			pack.to_port = ntohs(usp[ip_hdr_len*2+1]);

//			print_packet_info(pack);
			handle_udp(pack);
		}

	}
	
}

void *maintainer_loop(void *){
	while (1){
		
/*		if (gui!=NULL)
			gui->addStream("host1","host2");
		else
			cerr << "ERROR: GUI is NULL"<< endl;
*/			
		
		LOCK;
		for (list<struct packet>::iterator i = active.begin(); i!=active.end(); i++){
			(*i).value--;
		}
		bool done;
		do{
			done=true;
			for (list<struct packet>::iterator i = active.begin(); i!=active.end(); i++){
				if ((*i).value<=0){
//					cerr << "Maintainer: Removing stream"<< endl;
					gui->removeStream( ipPortToString( (*i).from_ip, (*i).from_port) , 
							   ipPortToString( (*i).to_ip, (*i).to_port) );
					soundcard->unRegisterSource( (*i).from_port );
					active.erase(i);
					done=false;
					break;
				}
			}
		}while(!done);
		
		for (list<struct packet>::iterator i = potential.begin(); i!=potential.end(); i++){
			(*i).value--;
		}
		do{
			done=true;
			for (list<struct packet>::iterator i = potential.begin(); i!=potential.end(); i++){
				if ((*i).value<=0){
//					cerr << "Maintainer: Removing stream"<< endl;
					potential.erase(i);
					done=false;
					break;
				}
			}
		}while(!done);
		
		cerr << endl <<"Statistics: n_active="<< active.size()<< " n_potential="<<potential.size()<< " Analyzed: "<< n_audio<<" audio; "<<n_udp<<" UDP; "<<n_pack<<" total"<<endl;
		if (active.size()>0){
			cerr << "Active streams: "<< endl;
			for (list<struct packet>::iterator i=active.begin(); i!=active.end(); i++){
				cerr <<"\t";
				print_packet_info_short(*i);
				cerr << endl;
			}
		}
		
		if (potential.size()>0){
			cerr << "Potential streams: "<< endl;
			for (list<struct packet>::iterator i=potential.begin(); i!=potential.end(); i++){
				cerr <<"\t";
				print_packet_info_short(*i);
				cerr << endl;
			}
		}
		UNLOCK;
		sleep(2);
	}
}

int  main(int argc, char **argv){

	if (argc!=3){
		fprintf(stderr, "Usage: eve <network interface> <soundcard>\n\t<network interface>\tTypically eth0 or eth1\n\t<soundcard>\tTypically /dev/dsp\n");
		exit(1);
	}

        OssSoundDevice *oss = new OssSoundDevice(argv[2]);
	soundcard = new SoundIO(oss, 1, 16000);
        cerr << "Opening soundcard "<< argv[2]<< endl;
	soundcard->openPlayback();
//        soundcard->play_testtone(5);
//        soundcard->sync();

	soundcard->start_sound_player(); //starting second thread (main is first)
	
	pthread_mutex_init(&mlock, NULL);
	
	pthread_t t;
	pthread_create(&t, NULL, maintainer_loop, NULL); //starting third thread
	init_promisq(argv[1]);
	pthread_create(&t, NULL, read_packet, NULL);  //starting fourth thread
	
	QApplication a( argc, argv );

	EveGui eve_gui;
	gui = &eve_gui;
	a.setMainWidget( &eve_gui);
	eve_gui.show();
	return a.exec();
}
