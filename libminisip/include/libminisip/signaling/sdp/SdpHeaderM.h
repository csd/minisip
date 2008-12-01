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

/* Name
 * 	SdpHeaderM.h
 * Author
 * 	Erik Eliasson, eliasson@it.kth.se
 * Purpose
 * 
*/


#ifndef SDPHEADERM_H
#define SDPHEADERM_H

#include<libminisip/libminisip_config.h>

#include<libminisip/signaling/sdp/SdpHeader.h>
#include<libminisip/signaling/sdp/SdpHeaderC.h>
#include<libminisip/signaling/sdp/SdpHeaderI.h>
#include<vector>

class SdpHeaderA;

class LIBMINISIP_API SdpHeaderM : public SdpHeader{
	public:
		SdpHeaderM(std::string buildFrom);
		SdpHeaderM(std::string media, int32_t port, int32_t n_ports, std::string transport);
		SdpHeaderM(const SdpHeaderM &src);
		virtual ~SdpHeaderM();
		
		SdpHeaderM &operator=(const SdpHeaderM &src);

		virtual std::string getMemObjectType() const {return "SdpHeaderM";}

		void addFormat(std::string format);
		int32_t getNrFormats();
		std::string getFormat(int32_t i);

		std::string getMedia();
		void setMedia(std::string m);

		int32_t getPort();
		void setPort(int32_t p);

		int32_t getNrPorts();
		void setNrPorts(int32_t n);

		std::string getTransport();
		void setTransport(std::string t);

		virtual std::string getString();

		void addAttribute(MRef<SdpHeaderA*>);
		std::string getAttribute(std::string key, uint32_t index);
		std::list<MRef<SdpHeaderA*> > getAttributes();

		std::string getRtpMap(std::string format);
		
		std::string getFmtpParam(std::string format);

		void setConnection( MRef<SdpHeaderC*> c );
		MRef<SdpHeaderC*> getConnection();
		
		void setInformation(MRef<SdpHeaderI*> i);
		MRef<SdpHeaderI*> getInformation();
		
	private:
		std::string media;
		int32_t port;
		int32_t nPorts;
		std::string transport;
		std::vector<std::string> formats;
		std::list<MRef<SdpHeaderA*> >attributes;
		MRef<SdpHeaderC*> connection;
		MRef<SdpHeaderI*> information;
};
#endif
