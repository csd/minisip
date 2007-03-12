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

#ifndef RTCPAPP_H
#define RTCPAPP_H

#include<libminisip/libminisip_config.h>

#include<libminisip/signaling/p2t/RtcpAPPHeader.h>
#include<libmnetutil/UDPSocket.h>
#include<libmnetutil/IPAddress.h>
#include<libminisip/signaling/p2t/P2T.h>

#if BYTE_ORDER == LITTLE_ENDIAN

#else
#error RTP only works with little endian -- fix.
#endif


/**
 * Representation of the RTCP APP packet with the 
 * correct bit size for every header value. 
 * <p><b>RTCP APP packet:</b><br>
 * The structure of the RTCP APP packet is shown below. The definitions of the fields
 * are found in RFC 3550, section 6.6.
 * <p>
 * <img src=material/RtcpAPP.gif>
 * <p>
 * This struct represents the header fields (all fields above the 
 * application-dependent data) of the RTCP packet and
 * is defined as follows:<p>
 * <code>
 * struct rtcpAPPheader{<br>
 * &nbsp;&nbsp;unsigned st:5; //the subtype field<br>
 * &nbsp;&nbsp;unsigned p:1; //the padding bit<br>
 * &nbsp;&nbsp;unsigned v:2; //the version field<br>
 * &nbsp;&nbsp;unsigned pt:8; //the packet type field<br>
 * &nbsp;&nbsp;unsigned length:16; //the length field<br>
 * &nbsp;&nbsp;unsigned ssrc:32; //the SSRC field<br>
 * &nbsp;&nbsp;unsigned c1:8; //the first character of the name field<br>
 * &nbsp;&nbsp;unsigned c2:8; //the second character of the name field<br>
 * &nbsp;&nbsp;unsigned c3:8; //the third character of the name field<br>
 * &nbsp;&nbsp;unsigned c4:8; //the fourth character of the name field<br>
 * };<br>
 * </code>
 * 
 */
struct rtcpAPPheader{               // WARNING: ONLY FOR x86 arch...
	unsigned st:5;
	unsigned p:1;
	unsigned v:2;
	unsigned pt:8;
	unsigned length:16;

	unsigned ssrc:32;
	//unsigned name:32;
	//name
	unsigned c1:8;
	unsigned c2:8;
	unsigned c3:8;
	unsigned c4:8;
};

/**
 * a RTCP APP packet.
 * <p><b>RTCP APP packet:</b><br>
 * The structure of the RTCP APP packet is shown below. The definitions of the fields
 * are found in RFC 3550, section 6.6.
 * <p>
 * <img src=material/RtcpAPP.gif>
 * <p>
 * This class represents a RTCP APP packet.
 * @author Florian Maurer, florian.maurer@floHweb.ch
 */

class LIBMINISIP_API RtcpAPP{
	public:
		/**
		 * Constructor.
		 **/
		RtcpAPP();

		/**
		 * Constructor.
		 * @param header         the header fields
		 * @param appdata        the application dependent data
		 * @param appdata_length the length of the application dependent data
		 *			 (should be multiple of 4)
		 **/
		RtcpAPP(RtcpAPPHeader header, unsigned char *appdata, int appdata_length);	
		
		/**
		 * Constructor.
		 * @param name value for the name field
		 * @param ssrc value for the ssrc field
		 **/
		RtcpAPP( std::string name, unsigned ssrc );
		
		/**
		 * Constructor.
		 * @param subtype value for the subtype field
		 * @param name    value for the name field
		 * @param ssrc    value for the ssrc field
		 **/
		RtcpAPP(unsigned subtype, std::string name, unsigned ssrc );
		
		/**
		 * Constructor.
		 * @param appdata	   application dependent data
		 * @param appdata_length   length of appdata. (should be multiple of 4)
		 * @param subtype          value for the subtype field
		 * @param name             value for the name field
		 * @param ssrc             value for the ssrc field
		 **/
		RtcpAPP(unsigned char *appdata, int appdata_length, unsigned subtype, std::string name, unsigned ssrc );
		
		/**
		 * Destructor
		 **/
		virtual ~RtcpAPP();
		
		/**
		 * return the header fields
		 * @return <code>RtcpAPPHeader</code> object containing the header fields of the RTCP
		 *         APP packet.
		 **/
		RtcpAPPHeader &getHeader();
		
		/**
		 * sends the RTCP APP packet via the specified socket to the
		 * destination.
		 * @param udp_sock the socket that is used to send the packet
		 * @param to_addr  the destination address
		 * @param port     the destination port
		 **/
		void sendTo(UDPSocket &udp_sock, IPAddress &to_addr, int port);		
		
		/**
		 * get the representation of the packet according the RFC with the correct
		 * bit size for every value.
		 * @return char
		 **/
		virtual char *getBytes();		
		
		/**
		 * get the size of the packet
		 * @return the number of bytes
		 **/
		virtual int size();		
		
		/**
		 * reads the input from a socket and return a RTCP APP packet.
		 * @return <code>RtcpAPP</code> object.
		 **/	
		static RtcpAPP *readPacket(UDPSocket &udp_sock, int timeout=-1);
		
		/**
		 * get the application dependetn data of the RTCP APP packet.
		 * @return unsigned char
		 */
		unsigned char *getAppdata();
		
		/**
		 * get the length of the application dependent data
		 * @return the length in bytes
		 */
		int getAppdataLength();
	
	protected:
		///the header fields
		RtcpAPPHeader header;
		///the length of the application-dependent data
		int appdata_length;
		///the application dependent data
		unsigned char *appdata;
};
 
#endif
