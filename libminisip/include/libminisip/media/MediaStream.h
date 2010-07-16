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

#ifndef MEDIA_STREAM_H
#define MEDIA_STREAM_H

#include<libminisip/libminisip_config.h>

#include<libmutil/MemObject.h>

#include<libminisip/media/rtp/CryptoContext.h>
#include<libminisip/media/RealtimeMedia.h>
#include<libminisip/media/ReliableMedia.h>
#include"RtpReceiver.h"
#include<libminisip/media/rtp/SRtpPacket.h>

#include<libmikey/KeyAgreement.h>
#ifdef ZRTP_SUPPORT
#include <libminisip/media/zrtp/ZrtpHostBridgeMinisip.h>
#include <time.h>
class ZrtpHostBridgeMinisip;
#endif

class UDPSocket;
class SdpHeaderM;
class IpProvider;
class RealtimeMedia;
class ReliableMedia;


class LIBMINISIP_API MediaStream : public MObject {
public:
	MediaStream(std::string callId_, MRef<Media*> m): callId(callId_),media(m){
	
	}
	/**
	 * Starts the transmission or reception of a the stream.
	 */
	virtual void start() = 0;

	/**
	 * Stops the transmission or reception of a the stream.
	 */
	virtual void stop() = 0;

#ifdef DEBUG_OUTPUT
	virtual std::string getDebugString();
#endif

	/**
	 * Returns the media type corresponding to this stream
	 * (video, audio...) as it appears in the session
	 * description (SDP).
	 * @returns the type as a string
	 */
	std::string getSdpMediaType();/* audio, video, application... */

	/**
	 * Returns additional media attributes that are to appear
	 * in the session description (SDP) as a: header.
	 * @returns the attributes as a list of string, each
	 * of them should go into a a: header
	 */
	std::list<std::string> getSdpAttributes();


	std::string getCallId(){return callId;}

	/**
	 * Used to query the port on which the media is received for
	 * a receiver, respectively where it is sent to for a sender
	 * @returns the port.
	 */
	virtual uint16_t getPort()=0;

	/**
	  Returns an MRef to the Media object used by this media stream.
	  Use with care.
	  */
	MRef<Media *> getMedia() { return media; }

protected:
	std::string callId;
	MRef<Media *> media;
};

class LIBMINISIP_API ReliableMediaStream : public MediaStream {
public:
	ReliableMediaStream(std::string callId, MRef<ReliableMedia*> m);

	std::string getTransport(){return "TCP";} //TODO: support other (secure) transports

	/**
	 * Examples
	 *                      Media formats
	 *                         vvvvvv
	 * m = audio 12345 RTP/AVP 0 1 99
	 *
	 *                     vvv
	 * m = image 12345 TCP t38
	 *
	 * @return  The list of formats supported by the reliable media
	 * 	    session.
	 */
	virtual std::string getMediaFormats()=0;

/*	virtual void start(){}
	virtual void stop(){}
*/
//	virtual uint16_t getPort();
	virtual uint16_t getPort(std::string type)=0;

protected:
	uint16_t port;
	std::string mediaFormats;



private:

};


/**
 * Abstract class that implements common functions to a
 * RealtimeMediaStreamSender and RealtimeMediaStreamReceiver
 */
class LIBMINISIP_API RealtimeMediaStream : public MediaStream {
	public:

		virtual std::string getMemObjectType() const {return "RealtimeMediaStream";}
		bool disabled;

		/**
		 * Used to check a m: header in a session description
		 * against the media stream for compatibility
		 * @param m a reference to the object representing the m:
		 * SDP header
		 * @param formatIndex is the index of the CODEC
		 * in the m: header against which we should compare the
		 * MediaStream
		 * @returns whether or not this media stream corresponds
		 * to the description in the m: header.
		 */
		virtual bool matches( MRef<SdpHeaderM *> m,
					uint32_t formatIndex );

		/**
		 * Used by the media session to pass the result of
		 * the key exchange performed during the session
		 * initiaton.
		 * @param ka a reference to the KeyAgreement object.
		 */
		void setKeyAgreement( MRef<KeyAgreement *> ka );

#ifdef ZRTP_SUPPORT
		/**
		 * Set the ZRTP implementation host brigde for this media stream.
		 *
		 * The ZRTP host bridge implements the Minisip specific functions
		 * to connect Minisip to the ZRTP protocol / key agreement. Please
		 * note that the same host bridge instance must be used for a pair
		 * of sender/receiver objects that form a RTP session.
		 *
		 * @param zsb
		 *    The pointer to the host bridge object for this RTP session.
		 *
		 * TODO: make it a list because the receiver may have several hostbridges
		 * in case of e.g. conferences.
		 */
		virtual void setZrtpHostBridge(MRef<ZrtpHostBridgeMinisip *> zsb);

		/**
		 * Get the ZRTP implementation host brigde for this media stream.
		 *
		 * @return zsb
		 *    The pointer to the host bridge object for this RTP session.
		 */
		virtual MRef<ZrtpHostBridgeMinisip *> getZrtpHostBridge();

		/**
		 * Used by ZRTP host bridge to set the crypto context for this RTP session.
		 *
		 * @param cx
		 *    The initialized CryptoContext to use for this RTP session.
		 * @param ssrc
		 *    The ssrc of this RTP stream (sender or receiver)
		 */
		void setKeyAgreementZrtp(MRef<CryptoContext *>cx);
#endif

		void flushCryptoContexts(){
			kaLock.lock();
			cryptoContexts.clear();
			kaLock.unlock();
		}

	protected:
		MRef<CryptoContext *> getCryptoContext( uint32_t ssrc, uint16_t seq_no );
		RealtimeMediaStream( std::string callId, MRef<RealtimeMedia *> );
		MRef<RealtimeMedia *> realtimeMedia;
		uint32_t csbId;

		std::string localPayloadType; //For RTP media, this is a number

		MRef<CryptoContext *> initCrypto( uint32_t ssrc, uint16_t seq_no );
		MRef<KeyAgreement *> ka;
		Mutex kaLock;
		std::list< MRef<CryptoContext *> > cryptoContexts;
#ifdef ZRTP_SUPPORT
		MRef<ZrtpHostBridgeMinisip *> zrtpBridge;
#endif
};

/**
 * The MediaStreamReceiver receives media for a given medium on a
 * given session. It is responsible for decryption and replay protection
 * in the case of SRTP.
 */
class LIBMINISIP_API RealtimeMediaStreamReceiver : public RealtimeMediaStream{
	public:
		/**
		 * Constructor, called by the MediaHandler when creating
		 * a new media session.
		 * @param media a reference to the Media object
		 * that will process
		 * incoming data on this receiver.
		 * @param rtpReceiver a reference to the RtpReceiver object
		 * to which this MediaStreamReceiver should register when
		 * the session starts
		 * @param ipProvider reference to the IpProvider object,
		 * used to obtain contact IP address and port in NAT
		 * traversal mechanism
		 */
		RealtimeMediaStreamReceiver( std::string callId,
				MRef<RealtimeMedia *> media,
				MRef<RtpReceiver *> rtpReceiver,
				MRef<RtpReceiver *> rtp6Receiver = NULL );

#ifdef DEBUG_OUTPUT
		virtual std::string getDebugString();
#endif

		virtual std::string getMemObjectType() const {return "RealtimeMediaStreamReceiver";}

		/**
		 * Starts the reception of a the stream, by subscribing to
		 * the RtpReceiver.
		 */
		virtual void start();

		/**
		 * Stops the reception of a stream, by unsubscribing to
		 * the RtpReceiver.
		 */
		virtual void stop();

		/**
		 * Used to query which port should be advertised as
		 * the contact port in the session description.
		 * @returns the port to use
		 */
		virtual uint16_t getPort();

		/**
		 * Used to query which port should be advertised as
		 * the contact port in the session description.
		 * @param addrType IP4 or IP6
		 * @returns the port to use
		 */
		uint16_t getPort( const std::string &addrType );

		/**
		 * Handles incoming RTP packets, decrypts them
		 * and send them to the corresponding media for
		 * playback.
		 * @param packet the (S)RTP packet to handle
		 */
		virtual void handleRtpPacket( const MRef<SRtpPacket *> & packet, std::string callId, MRef<IPAddress *> from );

		/**
		 * Returns a unique identifier for this Receiver. Used
		 * to register or unregister receivers to RtpReceiver objects.
		 * @returns the identifier
		 */
		uint32_t getId();

		/**
		 * Used to query the available CODECs for this media type,
		 * during this session. Used by the Session to create
		 * the session description (SDP).
		 *
		 * @returns a list of references to Codec objects, sorted
		 * according to the user's preference, first being preferred
		 */
		std::list<MRef<Codec *> > getAvailableCodecs();

		std::list<uint32_t> getSsrcList() {
			return ssrcList;
		}

#ifdef ZRTP_SUPPORT
		/**
		 * Process a received packet with an extension header
		 * and unknown payload type.
		 *
		 * This packet has an extension header and does not
		 * contain payload data to process. The method checks
		 * if it is a ZRTP packet, if yes process
		 * it. Otherwise just return to the caller.  Because
		 * the payload type is unknown the caller usually
		 * dismisses the packet.
		 *
		 * <p/>
		 *
		 * This method is called only if a ZRTP host bridge is
		 * enabled.
		 *
		 * @param packet
		 *   A (S)Rtp packet to process
		 */
		virtual void handleRtpPacketExt(MRef<SRtpPacket *> packet);
#endif

	protected:
		std::list<MRef<Codec *> > codecList;
		MRef<RtpReceiver *> rtpReceiver;
		MRef<RtpReceiver *> rtp6Receiver;
		uint32_t id;
		uint16_t externalPort;

		void gotSsrc( uint32_t ssrc, std::string callId );

		std::list<uint32_t> ssrcList;
		Mutex ssrcListLock;

		bool running;

};

/**
 * The MediaStreamSender is used to send media to a specific peer, during
 * a specific media Session. It holds the CODEC instance selected for
 * this peer, and is responsible for encryption.
 */
class LIBMINISIP_API RealtimeMediaStreamSender : public RealtimeMediaStream{
	public:
		/**
		 * Constructor, used by the MediaHandler during the
		 * creation of the media Session.
		 * @param media a reference to the Media object from
		 * which the MediaStreamSender is receiving data
		 * @param senderSock a reference to the UDPSocket object
		 * to which the data should be sent. If NULL a new one
		 * is created
		 */
		RealtimeMediaStreamSender( std::string callId,
				   MRef<RealtimeMedia *> media,
				   MRef<UDPSocket *> senderSock=NULL,
				   MRef<UDPSocket *> sender6Sock=NULL );

#ifdef DEBUG_OUTPUT
		virtual std::string getDebugString();
#endif

		virtual std::string getMemObjectType() const {return "RealtimeMediaStreamSender";}

		/**
		 * Returns the CODEC instance currently selected for
		 * this peer.
		 * @returns a reference to the CodecState object
		 */
		MRef<CodecState *> getSelectedCodec(){return selectedCodec;};
		

		void setSelectedCodec(MRef<CodecState *> t ){
			selectedCodec = t ;
		}

		/**
		 * Starts the transmission of the stream, by
		 * subscribing to the Media for data.
		 */
		virtual void start();

		/**
		 * Stops the transmission, by unsubscribing to the
		 * Media.
		 */
		virtual void stop();

		/**
		 * Used by the Session to set the port on which the
		 * the peer is expecting to receive the data. This
		 * is extracted from the received session description
		 * (SDP).
		 * @param port the port to which the RealtimeMediaStreamSender should
		 * send
		 */
		virtual void setPort( uint16_t port );

		/**
		 * Returns the port to which data is sent.
		 * @retunrs the port to which the RealtimeMediaStreamSender is sending
		 * data, or 0 if it was not set
		 */
		virtual uint16_t getPort();

		/**
		 * Used by the Media to send data, if the RealtimeMediaStreamSender
		 * has subscribed to the Media. The data will be encrypted
		 * if required, and encapsulated in an RTP packet.
		 * @param data a pointer to the data to send
		 * @param length the lenght of the data to send
		 * @param ts the timestamp to use in RTP header, or
		 * NULL if the RealtimeMediaStreamSender should decide it
		 * @param marker whether or not the marker should be set
		 * in the RTP header
		 * @param dtmf whether or not the data is a DTMF signal
		 */
		void send( byte_t * data, uint32_t length, uint32_t * ts, bool marker = false, bool dtmf = false );


		void setSelectedCodecHacked( MRef <RealtimeMedia*> m);


#ifdef ZRTP_SUPPORT
		/**
		 * Used by the ZRTP host bridge to send ZRTP data.
		 *
		 * This method sets up a ZRTP packet with a specific
		 * payload type and sends the data to our peer. These
		 * packets shall not go into normal payload processing
		 * at the receiver because of the specific payload
		 * type setting.
		 *
		 * @param data
		 *    The pointer to the extension header to send.
		 * @param length
		 *    Length of the extension header in bytes.
                 * @param payload
                 *    Pointer to the payload or NULL if no payload required
                 * @param payLen
                 *    Length of payload in bytes
		 */
		void sendZrtp(unsigned char* data, int length,
                              unsigned char* payload, int payLen);




		/**
		 * Get the current Seq number of this packet
		 *
		 * @return
		 *     The sender current sequence number.
		 */
		uint16_t getSeqNo() { return seqNo; };
#endif

		void sendRtpPacket(const MRef<RtpPacket*> & rtp);

		/**
		 * Used by the Session to specify the IP address
		 * on which the data should be sent to the peer. This
		 * information is extracted from the received session
		 * description (SDP)
		 * @param remoteAddress a pointer to the IPAddress object
		 * which represents the peer's contact IP address
		 */
		void setRemoteAddress( MRef<IPAddress *> remoteAddress );

		/**
		 * Used to mute or unmute this sender, resulting
		 * in it sending or not sending the data it receives
		 * from the Media.
		 * @param mute whether or not this MediaStreamSender should
		 * be muted
		 */
		void setMuted( bool mute ) { muted = mute;}

		/**
		 * Queries the muted state of this MediaStreamSender
		 * @returns whether or not this MediaStreamSender is muted
		 */
		bool isMuted() { return muted;}

		/**
		 * Used to know whether or not a packet should be sent
		 * even if the MediaStreamSender is muted, for the
		 * Session to be kept alive.
		 * @param max specifies after how many muted packets a
		 * keep alive packet should be sent
		 */
		bool muteKeepAlive( uint32_t max);

		/**
		 * Used to check a m: header in a session description
		 * against the media stream for compatibility. In
		 * case the MediaStreamSender is compatible, the
		 * CODEC is selected, if it wasn't already.
		 * @param m a reference to the object representing the m:
		 * SDP header
		 * @param formatIndex is the index of the CODEC
		 * in the m: header against which we should compare the
		 * MediaStream
		 * @returns whether or not this media stream corresponds
		 * to the description in the m: header.
		 */
		virtual bool matches( MRef<SdpHeaderM *> m,
					uint32_t formatIndex );

		/**
		 * Used to query the SSRC identifier used in RTP headers.
		 * It is created randomly upon creation of the
		 * MediaStreamSender.
		 * @returns the SSRC identifier used by this MediaStreamSender
		 */
		uint32_t getSsrc();

		void increaseLastTs( ) { lastTs += 160; };
		uint32_t getLastTs() { return lastTs; };

	private:
		uint32_t ssrc;
		MRef<UDPSocket *> senderSock;
//		MRef<UDPSocket *> senderSockHack;
//		uint32_t primarySsrcHack;
		MRef<UDPSocket *> sender6Sock;
		uint16_t remotePort;
		uint16_t seqNo;
		uint32_t lastTs;
		MRef<IPAddress *> remoteAddress;
		Mutex senderLock;

		std::string payloadType;
		MRef<CodecState *> selectedCodec;

		//Cesc -- does it conflict with bool disabled???
		bool muted;
		uint32_t muteCounter;

};

#endif
