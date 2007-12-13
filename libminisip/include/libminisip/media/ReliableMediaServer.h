/*
 Copyright (C) 2004-2007 the Minisip Team
 
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

/* Copyright (C) 2004-2007
 *
 * Authors: Erik Eliasson <ere@kth.se>
 *          Johan Bilien <jobi@via.ecp.fr>
*/



#ifndef RELIABLEMEDIASERVER
#define RELIABLEMEDIASERVER

#include<libminisip/libminisip_config.h>

#include<libminisip/media/Media.h>


/**
 * Handles an offered resource.
 */
class LIBMINISIP_API ReliableMediaServer : public Media {
	public:

//		ReliableMediaServer(std::string type):sdpType(type){}
		~ReliableMediaServer();

		virtual std::string getSdpMediaType(){return sdpType;}

		virtual std::list<std::string> getSdpAttributes()=0;

		virtual uint16_t getPort(){return 33;}; //FIXME: DEBUGING!!!!!!


		/**
		 * Send the data to all the registered MediaStreamSender.
		 * If relevant, the data is first encoded using the
		 * MediaStream's selected CODEC.
		 * @param data pointer to the data to send
		 * @param length length of the data buffer
		 * @param ts timestamp to use in the RTP header
		 * @param marker whether or not the marker should be set
		 * in the RTP header
		 */
	//	virtual void sendData( byte_t * data, uint32_t length, uint32_t ts, bool marker=false );
	
		

		/**
		 * Used by the media sessions to register a MediaStreamSender.
		 * When a MediaStreamSender is registered to a Media object,
		 * it will be used to send sampled media from the corresponding
		 * medium
		 * @param sender a reference to the MediaStreamSender object to
		 * register
		 */
//		virtual void registerMediaSender( MRef<MediaStreamSender *> sender );
		
		/**
		 * Used by the media sessions to unregister a MediaStreamSender,
		 * when a media session ends.
		 * @param sender a reference to the MediaStreamSender object to
		 * unregister
		 */
//		virtual void unregisterMediaSender( MRef<MediaStreamSender *> sender );



		/**
		 * Used to register a new media source. Called upon discovery
		 * of a new SSRC identifier. Each media source may use
		 * a different decoder.
		 * @param ssrc the SSRC identifier used by the new media source
		 */
//		virtual void registerMediaSource( uint32_t ssrc, std::string callId )=0;
		
		/**
		 * Used to unregister a media source when the session ends. 
		 * @param ssrc the SSRC identifier used by the media source to
		 * unregister
		 */
//		virtual void unregisterMediaSource( uint32_t ssrc)=0;
		

	protected:
		ReliableMediaServer( std::string sdpType);

		std::string sdpType;
};


#endif
