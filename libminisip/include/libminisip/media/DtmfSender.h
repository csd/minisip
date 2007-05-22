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

/* Copyright (C) 2004, 2005
 *
 * Authors: Erik Eliasson <eliasson@it.kth.se>
 *          Johan Bilien <jobi@via.ecp.fr>
*/

#ifndef DTMFSENDER_H
#define DTMFSENDER_H

#include<libminisip/libminisip_config.h>

#include<libmutil/MemObject.h>

class Session;

class LIBMINISIP_API DtmfEvent{
	public:
		DtmfEvent( uint8_t symbol_, 
				uint8_t volume_, 
				uint16_t duration_, 
				bool endOfEvent_, 
				bool startOfEvent_, 
				uint32_t * ts_, 
				bool lastBlock_ = false ):
					symbol(symbol_),
					volume(volume_),
					duration(duration_),
					endOfEvent(endOfEvent_),
					startOfEvent(startOfEvent_),
					ts(ts_),
					lastBlock(lastBlock_){
	};

	private:
		uint8_t symbol;
		uint8_t volume;
		uint16_t duration;
		bool endOfEvent;
		bool startOfEvent;
		uint32_t * ts;
		bool lastBlock;
	
		friend class DtmfSender;
};

class LIBMINISIP_API DtmfSender : public MObject {
	public:
		DtmfSender( MRef<Session *> session );
		void timeout( DtmfEvent * event );
		virtual std::string getMemObjectType() const { return "DtmfSender"; };

	private:
		MRef<Session *> session;
		void sendPayload( uint8_t payload[], bool mark, uint32_t * ts );
};

#endif
