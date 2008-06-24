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

#ifndef RTPPACKET_H
#define RTPPACKET_H

#include<libminisip/libminisip_config.h>

#include<libmutil/MemObject.h>

#include<libmnetutil/UDPSocket.h>
#include<libmnetutil/IPAddress.h>

#include<libminisip/media/rtp/RtpHeader.h>

/**
 * This class implements the RTP header.
 *
 * As a specific feature the RTP packet class handles the extension
 * header feature of RTP. This is because the extension header is not
 * standard RTP header but contains more application related data.
 *
 */

class LIBMINISIP_API RtpPacket: public MObject {
	public:
		RtpPacket();
		RtpPacket(unsigned char *content, int content_length, int seq_no,
                          unsigned timestamp, unsigned ssrc);
		RtpPacket(const RtpHeader &hdr, unsigned char *content, int content_length);
		virtual ~RtpPacket();

		static RtpPacket *readPacket(UDPSocket &udp_sock, int timeout=-1);

		void sendTo(UDPSocket &udp_sock, IPAddress &to_addr, int port);

		RtpHeader &getHeader();

                unsigned char *getContent()  {return (content_length > 0) ? content : NULL;}
                int getContentLength() const {return content_length;}
		/**
		 * Get length of extension header in bytes (not in words).
		 *
		 * @return
		 *    Length in bytes of extension header
		 */
		int getExtensionLength() const { return extensionLength; };

		/**
		 * Get pointer to extension header data.
		 *
		 * The pointer is returned as unsigned char because the extension
		 * header data structure is application dependend.
		 *
		 * @return
		 *    Returns the pointer to the extension header
		 */
		unsigned char* getExtensionHeader() { return extensionHeader; };

		/**
		 * Set extension header for this RTP packet.
		 *
		 * This method prepends the extension header to data thay may already
		 * exist in this packet. The method updates the overall data content
		 * as well as the lentgh. Als the extension header flag is set.
		 *
		 * @param data
		 *    Pointer to extensione header data.
		 * @param length
		 *    Length of extension header data in bytes.
		 */
		void setExtHeader(unsigned char* data, int length);

#ifdef DEBUG_OUTPUT
		void printDebug();
#endif
                /**
                 * Set this variable if this packet is a ZRTP packet.
                 *
                 * ZRTP requires a checksum to be appended to the packet. The
                 * value is the number of bytes of the checksum. Ths checksum
                 * algorithm is the standard IP algorithm (ones' complement of
                 * the sum of the data).
                 *
                 */
                void enableZrtpChecksum() { zrtpChecksum = 2;}

                /**
                 * Recompute an check ZRTP checksum
                 *
                 * If the <em>check</em> parameter is true then
                 * this method recomputes the ZRTP checksum and compares it
                 * with the checksum conatined in the packet. If the parameter
                 * is set to false the method only adjusts the length of the
                 * data.
                 *
                 * @param check
                 *     if <code>true</code> recompute and check, otherwise
                 *     adjust length of data only.
                 * @return <code>true</code> if check is ok or only length was
                 *     adjusted. If check fails returns <code>false</code>
                 */
                bool checkZrtpChecksum(bool check);

                uint16_t computeChecksum(uint16_t* data, int length);

		virtual char *getBytes();
		virtual int size();

	protected:
                int zrtpChecksum;
		RtpHeader header;
		int content_length;
		unsigned char *content;
		int extensionLength;
		unsigned char* extensionHeader;
};

#endif
