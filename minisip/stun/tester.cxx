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


//#include"STUNMessage.h"
//#include"STUNTest.h"
#include"../netutil/IP4Address.h"
#include"../netutil/UDPSocket.h"
//#include"../util/TimeoutProvider.h"
#include"STUN.h"

int main(int argc, char **argv){
//	srand(time(0)+getpid());

	IP4Address addr("larry.gloo.net");
//	IP4Address addr("stunserver.domain.com");
	IP4Address localAddr("192.16.127.135");
	
	UDPSocket sock;

	char tmp[16];
	uint16_t port;
	int type = STUN::getNatType(addr, 3478, sock, localAddr, (uint16_t)sock.get_port(), tmp, port);
//	cerr << "Type: "<< type << endl;
	cerr << "The NAT type is "<< STUN::typeToString(type)<< endl;

	
	cerr << "External mapping is: "<< tmp << ":" << port << endl;
	
//	STUNTest tst(&addr,3478,false,false);
//	tst.run(sock);

/*	IP4Address addr("larry.gloo.net");
	
	STUNMessageBindingRequest br;
	br.addAttribute(new STUNAttributeChangeRequest(false,true));

	int len;
	unsigned char *data = br.getMessageData(len);

	UDPSocket sock(false);

	sock.send_to(addr, 3478, data, len);

	unsigned char resp[2048];
	
	int rlen=sock.do_recv(resp, 2048);
	cerr << "Received "<< rlen << " bytes" << endl;
	
	STUNMessage *msg=STUNMessage::parseMessage(resp,rlen);
	cerr <<msg->getDesc()<<endl;

	//br.sendMessage(1);
*/	
	
	return 0;
}
