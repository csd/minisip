
#include"Sniffer.h"

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
#include <pthread.h>
#include <signal.h>


#define MAX_FRAME_LENGTH 65536 
#define RTP_HEADER_SIZE 12
#define LINK_LAYER_HEADER_SIZE 14
#define IP_LAYER_HEADER_SIZE 20
#define UDPTRANSPORT_LAYER_HEADER_SIZE 8


using namespace std;


const int Sniffer::PROTOCOL_IP=1;
const int Sniffer::PROTOCOL_UDP=2;
const int Sniffer::PROTOCOL_TCP=4;

Sniffer::Sniffer(string device):running(false),n_udp(0), n_tcp(0),n_ip(0),n_frames(0){
	init_promisq(device.c_str());
}

void Sniffer::addHandler(PacketReceiver *rcvr, int protocol){
	
	switch(protocol){
		case PROTOCOL_UDP:
			udpReceivers.push_back(rcvr);
			break;
		case PROTOCOL_TCP:
			tcpReceivers.push_back(rcvr);
			break;
		default:
			assert(false);
	}
	
}

void int_handler(int signo){
//	cerr << "Sighandler got signal"<< endl;
}

void Sniffer::run(){
	signal(SIGUSR2, int_handler);
	siginterrupt(SIGUSR2, 1);
//	cerr << "Sniffer running "<< endl;
	running=true;
	
	while (running){
		read_packet();
	}
//	cerr << "Sniffer thread stopped"<< endl;
	
}

void Sniffer::stop(){
	running=false;
	pthread_kill(tid, SIGUSR2);
	if (pthread_join(tid, NULL)){
		cerr << "Error when waiting for thread: "<<strerror(errno)<< endl;
	}
}

static void * threadStarter(void *arg){
	Sniffer *s = (Sniffer *)arg;
	s->run();
	return NULL;
}

void Sniffer::createRunThread(){
	pthread_create(&tid, NULL, threadStarter, this);
}

void Sniffer::init_promisq(const char *update_frame_device){
	struct packet_mreq mr;
	struct sockaddr_ll sll;
	struct ifreq ifr;
	int frame_ifindex;

	if (update_frame_device!=NULL){

		if ((frame_sockfd=socket(PF_PACKET, SOCK_RAW, htons(ETH_P_IP)))==-1){
			perror("Could not create socket: ");
			throw new SnifferException;
		}
		strcpy(ifr.ifr_name, update_frame_device);

		if (ioctl(frame_sockfd, SIOGIFINDEX, &ifr) < 0) {
			fprintf(stderr,"voip_eve/init_promisq: failed to fetch ifindex: %s\n", strerror(errno));
			exit(1);
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


void Sniffer::read_packet(){
	unsigned char * buff; 			// pointer to received data
	int i; 					// frame length
	struct sockaddr_ll from; 		// source address of the message
	socklen_t fromlen = sizeof(struct sockaddr_ll); 

	buff = (unsigned char *)malloc(MAX_FRAME_LENGTH);
	
	unsigned char *ipbuf= (unsigned char *) &buff[100];
	unsigned char *end_of_packet;
	
	fromlen = sizeof(from);
	i = recvfrom(frame_sockfd, &buff[100-LINK_LAYER_HEADER_SIZE], 1614, 0, (struct sockaddr *) &from, &fromlen);
//	cerr << "Received packet of type " << from.sll_protocol << "(family " << from.sll_family << ") PF_INET="<<PF_INET << endl;
//	cerr << "sll_protocol in hex: "<< hex << from.sll_protocol << dec << endl;
	end_of_packet = &buff[100-LINK_LAYER_HEADER_SIZE+i];
	if (i == -1){
		if (errno==EINTR){
		}else{
			fprintf(stderr,"voip_eve, cannot receive data: %s // \n", strerror(errno));
		}
		delete buff;
		return;
	}
	n_frames++;

	int protocol = ntohs(from.sll_protocol);

	if (protocol!=0x0800){	//if not IP, return
		delete buff;
		return;
	}
	n_ip++;

	struct timeval pack_time;
	gettimeofday(&pack_time, NULL);
	int pack_length=i-LINK_LAYER_HEADER_SIZE;
	//void *pack_data = (void *)ipbuf;
	uint32_t *uip;

	uip = &(((uint32_t *)ipbuf)[3]);
	uint32_t pack_from_ip = *uip;

	uip = &(((uint32_t *)ipbuf)[4]);
	uint32_t pack_to_ip = *uip;

	unsigned short *usp = (unsigned short*)ipbuf;

	int ip_hdr_len = ipbuf[0] & 0x0F;
//	cerr << "ip_hdr_len (expect 5):"<<ip_hdr_len <<endl;
//	cerr << "i="<<i <<" ipbuf[9]: "<<(int)ipbuf[9]<< endl;

	uint16_t pack_from_port = ntohs(usp[ip_hdr_len*2]);
	uint16_t pack_to_port = ntohs(usp[ip_hdr_len*2+1]);


	if ((int)ipbuf[9]==17){ //if UDP
		n_udp++;
		//handle_udp(pack);
		//assert( ((pack_length-ip_hdr_len*4) ==(end_of_packet - &ipbuf[ ip_hdr_len*4 ] ) ));
		list<PacketReceiver*>::iterator itt;
		unsigned char *udpcontentptr = &ipbuf[ ip_hdr_len*4+8 ];
		for (itt=udpReceivers.begin(); itt!=udpReceivers.end();itt++){
			(*itt)->handlePacket(pack_to_ip, pack_to_port, 
					     pack_from_ip, pack_from_port,
					     udpcontentptr,pack_length-(ip_hdr_len*4+8),
					     PROTOCOL_UDP
					    );
		}
	}


	if ((int)ipbuf[9]==6){ // if TCP
		n_tcp++;

		unsigned char *tcpbuf = &ipbuf[ ip_hdr_len*4 ];
		unsigned int tcplen = tcpbuf[12]>>4;
		tcpbuf+=tcplen*4;
		
		list<PacketReceiver*>::iterator itt;
		for (itt=tcpReceivers.begin(); itt!=tcpReceivers.end();itt++){
			(*itt)->handlePacket(pack_to_ip, pack_to_port, 
					     pack_from_ip, pack_from_port,
					     tcpbuf, end_of_packet-tcpbuf,
					     PROTOCOL_TCP
					    );
		}
	}

	delete buff;
}



