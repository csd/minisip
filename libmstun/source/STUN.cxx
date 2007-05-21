/*
 Copyright (C) 2004-2006 the Minisip Team
 
 This library is free software; you can redistribute it and/or
 modify it under the terms of the GNU Lesser General Public
 License as published by the Free Software Foundation; either
 version 2.1 of the License, or (at your option) any later version.
 
 This library is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 Lesser General Public License for more details.
 
 You should have received a copy of the GNU Lesser General Public
 License along with this library; if not, write to the Free Software
 Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
 */

/* Copyright (C) 2004 
 *
 * Authors: Erik Eliasson <eliasson@it.kth.se>
 *          Johan Bilien <jobi@via.ecp.fr>
*/

#include<config.h>

#include<libmstun/STUN.h>
#include<libmstun/STUNMessage.h>
#include<libmstun/STUNTest.h>

#include<libmutil/stringutils.h>

#include<libmnetutil/NetworkFunctions.h>

#include<stdio.h>

const int STUN::STUN_ERROR=0;
const int STUN::STUNTYPE_BLOCKED=1;
const int STUN::STUNTYPE_OPEN_INTERNET=2;
const int STUN::STUNTYPE_FULL_CONE=3;
const int STUN::STUNTYPE_SYMMETRIC_NAT=4;
const int STUN::STUNTYPE_PORT_RESTRICTED=5;
const int STUN::STUNTYPE_RESTRICTED=6;
const int STUN::STUNTYPE_SYMMETRIC_FIREWALL=7;

static const char *msgs[]={
	"ERROR",
	"BLOCKED",
	"OpenInternet",
	"FullCone", 
	"SymmetricNAT",
	"PortRestricted", 
	"Restricted",
	"SymmetricFirewall"};

using namespace std;

/*                                         
 * The following four tests are executed to test 
 * if/what NAT is present in the network  
 * 
 * In the first test the STUN server is contacted on it's
 * primary IP address. It will respond to the client
 * to the source address it receives it from and tell 
 * what source address the packet has.
 * 
 *  Test1 v1:                              +-----+
 *     +-----+ <-------------------------> |STUN | 
 *     | Cli |                             |(ip1)|
 *     | ent |                             +-----+
 *     +-----+                             +-----+
 *                                         |STUN |
 *                                         |(ip2)|
 *                                         +-----+
 *
 * A variant of the first test is when the STUN server is contacted 
 * on it's secondary IP. The two variants are needed to determine
 * if the NAT is a "symmetric" one.
 *  Test1 v2:                              +-----+
 *                                         |STUN |
 *     +-----+                             |(ip1)|
 *     | Cli |                             +-----+
 *     | ent |                             +-----+
 *     +-----+ <-------------------------> |STUN |
 *                                         |(ip2)|
 *                                         +-----+
 *                                          
 * In the second test the STUN server is contacted on it's
 * primary IP address, but it will use it's secondary IP
 * when it sends the response.
 * 
 *  Test2:                                 +-----+
 *                                         |STUN |
 *     +-----+ --------------------------> |(ip1)|
 *     | Cli |                             +-----+
 *     | ent |                             +-----+
 *     +-----+ <-------------------------- |STUN |
 *                                         |(ip2)|
 *                                         +-----+
 *                                         
 * In the third test the STUN server is contacted on it's 
 * prmary IP address and port. It will respond using it's
 * primary IP address, but it's secondary port.
 * 
 *  Test3:                          (port1)+-----+
 *     +-----+ --------------------------> |STUN | 
 *     | Cli | <-------------------------- |(ip1)|
 *     | Ent |                      (port2)+-----+
 *     +-----+                             +-----+
 *                                         |STUN |
 *                                         |(ip2)|
 *                                         +-----+
*/

/*
 * Note: The IP is supposed to be host byte order. This normally 
 * not done. Beware when this function somewhere else.
*/
/*
static void binIp2String(uint32_t ip, char *strBufMin16){
//	uint32_t nip = htonl(ip);
//	inet_ntop(AF_INET, &nip, strBufMin16, 16);
    sprintf( strBufMin16, "%i.%i.%i.%i", ( ip>>24 )&0xFF, 
                                         ( ip>>16 )&0xFF,
                                         ( ip>> 8 )&0xFF,
                                         ( ip     )&0xFF );
}
*/

/*
static bool isLocalIP(uint32_t ip, vector<string> &localIPs){
    char sip[20];
    binIp2String(ip,sip);
    string ssip(sip);
    
    for (vector<string>::iterator i=localIPs.begin(); i!=localIPs.end(); i++)
        if (ssip == (*i))
            return true;
    
    return false;
}
*/

void STUN::getExternalMapping(IPAddress &stunAddr, 
		uint16_t stunPort, 
		UDPSocket &socket, 
		char *bufferMappedIP, 
		uint16_t &mappedPort)
{
	STUNMessage *message = STUNTest::test(&stunAddr, stunPort, socket, false, false);

/*	STUNAttributeMappedAddress *mappedAttr=NULL;
	for (int i=0; i<message->getAttributeCount(); i++)
		if (message->getAttribute(i)==STUNAttribute::MAPPED_ADDRESS)
			mappedAddr = (STUNAttributeMappedAddress*)message->getAttribute(i);
*/

	STUNAttributeMappedAddress *mappedAddr = 
		(STUNAttributeMappedAddress *)message->getAttribute(STUNAttribute::MAPPED_ADDRESS);
	
//	uint32_t firstTestIP = message->getMappedAddress()->getBinaryIP();
	uint32_t firstTestIP = mappedAddr->getBinaryIP();
//	mappedPort = message->getMappedAddress()->getPort();
	mappedPort = mappedAddr->getPort();
	
	if (bufferMappedIP!=NULL)
		NetworkFunctions::binIp2String(firstTestIP, bufferMappedIP);
}


int STUN::getNatType(IPAddress &stunAddr, 
                uint16_t stunPort, 
                UDPSocket &socket, 
//                IPAddress &localIP, 
                vector<string> localIPs,
                uint16_t localPort)
{
	uint16_t dummy;
	return STUN::getNatType(stunAddr, stunPort, socket, localIPs, localPort, (char*)NULL, dummy);
}


int STUN::getNatType(IPAddress &stunAddr, 
                    uint16_t stunPort, 
                    UDPSocket &socket, 
//                    IPAddress &localIP, 
                    vector<string> localIPs,
                    uint16_t localPort, 
                    char* bufferMappedIP, 
                    uint16_t &mappedPort)
{

	STUNMessage *message;
	message = STUNTest::test(&stunAddr, stunPort, socket, false, false);

	if (message==NULL)
		return STUNTYPE_BLOCKED;
	
	STUNAttributeMappedAddress *mappedAddr = 
		(STUNAttributeMappedAddress *)message->getAttribute(STUNAttribute::MAPPED_ADDRESS);
//	uint32_t firstTestIP = message->getMappedAddress()->getBinaryIP();
	uint32_t firstTestIP = mappedAddr->getBinaryIP();
//	uint16_t firstTestPort = message->getMappedAddress()->getPort();
	uint16_t firstTestPort = mappedAddr->getPort();


	STUNAttributeMappedAddress *changedAddr = 
		(STUNAttributeMappedAddress *)message->getAttribute(STUNAttribute::CHANGED_ADDRESS);
//	uint32_t firstTestChangedIP=message->getChangedAddress()->getBinaryIP();
	uint32_t firstTestChangedIP=changedAddr->getBinaryIP();
//	uint32_t firstTestChangedPort = message->getChangedAddress()->getPort();
	uint32_t firstTestChangedPort = changedAddr->getPort();

	if (bufferMappedIP!=NULL)
		NetworkFunctions::binIp2String(firstTestIP, bufferMappedIP);
	mappedPort = firstTestPort;
	
///	if (firstTestIP==localIP.getBinaryIP() && firstTestPort==localPort){
	if (NetworkFunctions::isLocalIP(firstTestIP, localIPs)  &&  firstTestPort==localPort){
//		cerr << "Same IP"<< endl;
		message = STUNTest::test(&stunAddr, stunPort, socket, true, true);
		if (message==NULL){
			return STUNTYPE_SYMMETRIC_FIREWALL;
		}else{
			return STUNTYPE_OPEN_INTERNET;
		}
	
		
	}else{
//		cerr << "Behind NAT"<< endl;
		message = STUNTest::test(&stunAddr, stunPort, socket, true, true);
		if (message!=NULL){
//			cerr << "TYPE: FullCone NAT"<< endl;
			return STUNTYPE_FULL_CONE;
		}else{
//			cerr << "testing restrictions (sym/restr/portrestr)"<< endl;
			char tmp[16];
			NetworkFunctions::binIp2String(firstTestChangedIP, tmp);
//			IP4Address changedAddr(binIp2String(firstTestChangedIP));
			MRef<IPAddress*> changedAddr = IPAddress::create(tmp);

			
			message = STUNTest::test(*changedAddr, firstTestChangedPort, socket, true, true);
			if (message==NULL)
				return STUN_ERROR;

			STUNAttributeMappedAddress *mappedAddr = 
				(STUNAttributeMappedAddress *)message->getAttribute(STUNAttribute::MAPPED_ADDRESS);
			if (mappedAddr->getBinaryIP()!=firstTestIP || 
					mappedAddr->getPort()!=firstTestPort){
//				cerr << "TYPE: Symmetric"<< endl;
				return STUNTYPE_SYMMETRIC_NAT;
			}else{
//				cerr << "testing restrictions (restr/portrestr)"<< endl;
				message = STUNTest::test(&stunAddr, stunPort, socket, true, true);
				if (message == NULL){
					return STUNTYPE_PORT_RESTRICTED;
				}else{
					return STUNTYPE_RESTRICTED;	
				}
			}
		
		}	
	}
	return 0;
}


const char * STUN::typeToString(int type){
	return msgs[type];
}

