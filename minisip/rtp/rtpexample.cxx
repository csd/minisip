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

/* rtpexample.cxx
**
** Purpose:
**	Demonstrates how the RTP implementation can be used to transmit
**	information. This application implements both a sending and
**	receiving part and which one will be started depends on the
**	arguments to the application
**
** @author Erik Eliasson, eliasson@it.kth.se
*/


/*
 * Here you specify to which host and port the SRTP traffic will be sent.
 */
#define TO_HOST "192.168.100.1"
#define TO_PORT 20000

#include"RtpPacket.h"
#include"SRtpPacket.h"
#include"CryptoContext.h"
#include<unistd.h>
#include"../netutil/IP4Address.h"
#include<string.h>
#include"../util/ConfigFile.h"
#include"../util/print_hex.h"
#include <sys/time.h>

/* SENDER
 *
 * Purpose: Sends one of four names in an SRTP packet once per second
 * to localhost:20000
 *
 * Alg.
 *  1. Create socket that will be used for srtp traffic.
 *  2. Specify receiver
 *  3. Do once per second
 *    3.1 Define content of packet to transmit.
 *    3.2 Create SRTP packet.
 *    3.3 Send SRTP packet.
 */

#define N_PACKETS 500000
void sender(/*ConfigFile config*/){
        struct timeval *Tps, *Tpf;
        struct timezone *Tzp;
        //void *Tzp;
	
	unsigned char master_key[17] = "MMMMMMMMMMMMMMMM";
	unsigned char master_salt[15] = "SSSSSSSSSSSSSS";

        Tps = (struct timeval*) malloc(sizeof(struct timeval));

        Tpf = (struct timeval*) malloc(sizeof(struct timeval));

        Tzp = 0;





//	cout << "-[RTP sender started]-"<< endl;

//000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000
//00000000000000000000000000000000
	//string messages[5]={"Erik00000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000\0", "J-O\0","Pawel\0","Israel\0","NOTUSED\0"};
	string message = "Message10000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000end";
//	cout << "-[Creating socket]-"<< endl;
								// 1.
	UDPSocket *udpsock = new UDPSocket(false, 10000); //false means do not use IPv6, 10000 is the port
	IP4Address ipaddr(TO_HOST);				// 2.
	ipaddr.set_port(TO_PORT);


             //CryptoContext *scontext_s = new CryptoContext(config.get_string("master_key"), config.get_string("salt_key"), config.get_string ("sec_services"), config.get_string ("cipher_type"), config.get_string ("auth_type"));

        	//cout << "-[Crypto Context created!]-"<< endl;
	
	CryptoContext *scontext = new CryptoContext( 0, 0, aes_cm, hmac_sha1,
		master_key, 16, master_salt, 14 );
	scontext->derive_srtp_keys( 0 );
	
	gettimeofday (Tps, Tzp);
	
	for (int i=0; i<N_PACKETS; i++){
	//	fprintf( stderr, "Sent message: %s\n", message.c_str() );
                        //char sbuf[2048];
                        //for (unsigned int e=0;e<messages[0].length();e++)
                          //      sbuf[e]=messages[0][e], sbuf[e+1]=0;

		unsigned char rtp_content[160];
		//rtp_content = malloc( message.size() + 1 );
		//memcpy( rtp_content, message.c_str(), message.size() + 1 );
		
		
		//RtpPacket *rtppacket = new RtpPacket((void *)rtpcontent.c_str(), rtpcontent.length()+1, i, i*1000); //data, data length, sequence nr, timestamp
                SRtpPacket *rtppacket = new SRtpPacket(rtp_content, message.length()+1, i, i*1000,0); //data, data length, sequence nr, timestamp
#if 0
		// some replay protection control
		if( i == 12 )
			rtppacket->get_header().set_seq_no( 11 );

		// send the packet 15 after the 16
		if( i == 15 )
			continue;

		if( i == 17 )
			rtppacket->get_header().set_seq_no( 15 );

		// Send the packet 10 again
		if( i == 20 )
			rtppacket->get_header().set_seq_no( 10 );
#endif
		rtppacket->protect( scontext );
		
//		cerr << "tag sent:" << print_hex(rtppacket->get_tag(), 4)<<endl;
		


  	      rtppacket->send_to(*udpsock, ipaddr);
              
//	      cout << "i: " << i << endl;
                //delete srtppkt;
              delete rtppacket;
	}
	gettimeofday (Tpf, Tzp);

	long long unsigned usec =  (Tpf->tv_sec*1000000+Tpf->tv_usec) - (Tps->tv_sec*1000000 + Tps->tv_usec );
	printf( "µSeconds: %llu\n", usec );
	printf( "Nb Packets/s: %f\n", (((float)N_PACKETS)/((float)usec))*1000000 );
	printf( "datarate (in bit/s): %f\n", ((float)N_PACKETS/usec)*1000000*160*8 );
}


/* RECEIVER
 *
 * Purpose: Receives RTP packets,  and prints the content of them to standard
 * output.
 * Alg.
 * 1. Create socket
 * 2. Forever do
 *   2.1 Read rtp packet from socet
 *   2.2 Print content of rtp packet.
 */
#if 0
void receiver(/*ConfigFile config*/){
struct timeval *Tps, *Tpf;
struct timezone *Tzp;
//void *Tzp;
	unsigned char master_key[17] = "MMMMMMMMMMMMMMMM";
	unsigned char master_salt[15] = "SSSSSSSSSSSSSS";

Tps = (struct timeval*) malloc(sizeof(struct timeval));

Tpf = (struct timeval*) malloc(sizeof(struct timeval));

Tzp = 0;

         int i=1;

	cout << "-[RTP receiver started]-"<< endl;

	UDPSocket udpsock(false, 20000); //false means do not use IPv6, 20000 is the port
		//cout << "-[Opening Crypto Context]-"<< endl;

             //CryptoContext *scontext_r = new CryptoContext(config.get_string("master_key"), config.get_string("salt_key"), config.get_string ("sec_services"), config.get_string ("cipher_type"), config.get_string ("auth_type"));

             //             cout << "-[Crypto Context created!]-"<< endl;
	CryptoContext *scontext = new CryptoContext( 0, 0, aes_cm, hmac_sha1,
		master_key, 16, master_salt, 14 );
	scontext->derive_srtp_keys( 0 );
             while (1){
		     gettimeofday (Tps, Tzp);
		     SRtpPacket *packet = packet->read_packet(udpsock, scontext);
		     fprintf( stderr,"Packet length %i\n", packet->get_content_length() );
		     fprintf( stderr,"Packet number %i\n", packet->get_header().get_seq_no() );
		     cerr << "tag received:" << print_hex(packet->get_tag(), 4)<<endl;
		     bool auth =  packet->unprotect(scontext);
		     fprintf(stderr, "Authentication failed ? %i\n",auth);
		     gettimeofday (Tpf, Tzp);
		     
                          printf("Total Time creating RTP pkt(usec): %ld\n",

                                        (Tpf->tv_sec-Tps->tv_sec)*1000000

                                       + Tpf->tv_usec-Tps->tv_usec);

                          //RtpPacket *rtppkt = packet->get_rtp_packet(scontext_r, packet);
		if( !auth )
			cout << "Received packet: " << (char*)packet->get_content()<<endl;
		//delete rtppkt;
                          delete packet;
                          //cout << "i: " << i << endl;
                          //i++;
	}
}
#endif
void usage(){
	cerr << "Usage srtpexample {s|r}"<< endl;
	exit(1);
}

/*
 * Alg.
 *  o if argument is 's' then "sender()"
 *  o if argument is 'r' then "receiver()"
 *  o (else) usagemessage
 */
int main(int argc, char **argv){
        //string configfile = "../minisip/minisip.conf";
	if (argc!=2 || strlen(argv[1])!=1 )
		usage();
             //ConfigFile config(configfile);
	switch (argv[1][0]){
		case 's':
		case 'S':
             		sender(/*config*/);
			break;
		case 'r':
		case 'R':
                        //        	receiver(/*config*/);
			break;
		default:
			usage();
	}
	return 0;
}
