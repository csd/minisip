/*
  Copyright (C) 2006 Werner Dittmann 
 
  This library is free software; you can redistribute it and/or modify it
  under the terms of the GNU Lesser General Public License as published by the
  Free Software Foundation; either version 2.1 of the License, or (at your
  option) any later version.
 
  This library is distributed in the hope that it will be useful, but WITHOUT
  ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
  FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License
  for more details.
 
  You should have received a copy of the GNU Lesser General Public License
  along with this library; if not, write to the Free Software Foundation,
  Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
*/

/*
 * Authors: Werner Dittmann <Werner.Dittmann@t-online.de>
 */


#ifndef _ZIDHOSTBRIDGEMINISIP_H_
#define _ZIDHOSTBRIDGEMINISIP_H_

#include<libminisip/libminisip_config.h>

#include <stdio.h>
#include <stdint.h>
#include <string.h>

#include<libmutil/StateMachine.h>
#include<libmsip/SipSMCommand.h>

#include <libminisip/mediahandler/MediaStream.h>
#include <libminisip/rtp/SRtpPacket.h>
#include <libminisip/rtp/CryptoContext.h>

#include <libminisip/zrtp/ZrtpCallback.h>
#include <libminisip/zrtp/ZRtp.h>

/**
 * The connection between the ZRTP implementation and Minisip
 *
 * The ZRPT implementation is fairly independent from the underlying SIP and
 * RTP/SRTP implementation. This class implements specific functions and
 * interfaces that ZRTP uses to link to functions of the host. In this
 * case the host is Minisip.
 *
 * <p/>
 *
 * This bridge class implement the ZrtpCallback interface that ZRTP uses to
 * send data and to activate timer. A very Minisip specific part is the
 * handling of the timeout provider.
 *
 * <p/>
 *
 * The minisip <e>startSip</e> method call the bridge's <e>initialize</e>
 * method after the whole SIP was initialized. To avoid a new timeout provider
 * this bridge reuses the timeout provider created by SipStack. Thus the
 * initialize call looks like:
 *
 * <br>
 *
 * ZrtpHostBridge::initialize(sip->getSipStack()->getTimeoutProvider(), filename?);
 *
 * <br/>
 *
 * The initialize method stores the timeout provider and reuses it for every
 * instance. To do so the bridge inherits from <e>StateMachine<e/> but does use
 * the timeout specific parts only. The destructor frees the StateMachine to
 * maintain the timout provide reference counter.
 */

class ZrtpHostBridgeMinisip : public StateMachine<SipSMCommand,std::string>, public ZrtpCallback {

 public:

    virtual std::string getMemObjectType() { return "ZrtpHostBridgeMinisip";}
	
    /**
     * Initialize the host bridge.
     *
     * This static method must be called before any use of the host
     * bridge. If the caller does not provide a filename for the ZID
     * file the method opens the ZID file with the default name <e>
     * ~/.minisip.zid<e/>. This is a binary file.
     *
     * @param tp
     *    The timeout provider to use. In this case it shall be the 
     *    same as defined for the SIP stack.
     * @param zidFilename
     *    Optional filename for the ZID file.
     * @return
     *    TODO
     */
    static int32_t initialize(MRef<TimeoutProvider<std::string, MRef<StateMachine<SipSMCommand,std::string>*> > *> tp,
		     const char *zidFilename =NULL);

    ZrtpHostBridgeMinisip();
    ~ZrtpHostBridgeMinisip();


    void start();
    void stop();
    
    void setReceiver(MRef<MediaStreamReceiver *> r) { rStream = r; };
    void setSsrcReceiver(uint32_t ssrc)             { receiverSsrc = ssrc; };
    uint32_t getSsrcReceiver()                      { return receiverSsrc; };
    int32_t isSecureStateReceiver()                 { return receiverSecure; }

    void setSender(MRef<MediaStreamSender *> s)     { sStream = s; };
    void setSsrcSender(uint32_t ssrc)               { senderSsrc = ssrc; };
    uint32_t getSsrcSender()                        { return senderSsrc; };
    int32_t isSecureStateSender()                   { return senderSecure; }


    /**
     * Set the IP address of our remote peer.
     * 
     * We use this IP address to find the right ZRTP host
     * bridge when we receive packets on the receiver port allocated
     * by the MediaStreamReceiver.
     *
     * This is (fairly) save because one remote peer shall not have
     * several different RTP sessions for one of my receiver ports.
     * 
     * @param ra
     *    The IP address of our remote peer
     */
    void setRemoteAddress(MRef<IPAddress *> ra) { remoteAddress = ra; };

    /**
     * Get the IP address of our remote peer.
     * 
     * @return
     *    The IP address of our remote peer.
     */
    MRef<IPAddress *> getRemoteAddress() { return remoteAddress; };

    /**
     * Process a received packet with an extension header and known
     * payload type.
     *
     * This packet has an extension header and may have payload data
     * to process.  The method checks if it is a ZRTP packet, if yes
     * process it. Otherwise just return to the caller for further
     * processing of the packet.
     *
     * <p/>
     *
     * Depending on the contents of the packet and the protocol state
     * the method returns a indication to either dismiss the payload
     * data or process it as usual.
     *
     * @param packet
     *   A (S)Rtp packet to process
     * @return
     *    Returns 0 if the caller shall dismiss the payload, 1 otherwise.
     */
    int32_t processPacket(MRef<SRtpPacket *> packet);

    /**
     * Handle timeout event forwarded by TimeoutProvider.
     *
     * Just call the ZRTP engine for further processing.
     */
    void handleTimeout(const std::string &c) { zrtpEngine->processTimeout(); };

    int32_t sendDataRTP(const unsigned char* data, int32_t length);

    int32_t sendDataSRTP(const unsigned char* dataHeader, int32_t lengthHeader,
		         char *dataContent, int32_t lengthContent);

    int32_t activateTimer(int32_t time) {
	std::string s("ZRTP");
	requestTimeout(time, s);
	return 1;
    };

    int32_t cancelTimer() {
	std::string s("ZRTP");
	cancelTimeout(s);
	return 1;
    };

    void sendInfo(MessageSeverity severity, char* msg) {
	fprintf(stderr, "Severity: %d - %s\n", severity, msg);
    }

    /**
     * Switch on the security for the defined part.
     *
     * Create an CryproContext with the negotiated ZRTP data and
     * register it with the repective part (sender or receiver) thus
     * replacing the current active context (usually an empty
     * context).
     *
     * @param secrets
     *    The secret keys and salt negotiated by ZRTP
     * @param part
     *    An enum that defines sender, receiver, or both.
     */
    void srtpSecretsReady(SrtpSecret_t* secrets, EnableSecurity part);

    /**
     * Switch off the security for the defined part.
     *
     * Create an empty CryproContext and register it with the
     * repective part (sender or receiver) thus replacing the current
     * active context.
     *
     * @param part
     *    An enum that defines sender, receiver, or both.
     */
    void srtpSecretsOff(EnableSecurity part);

    /**
     * This method switches off secure state because of a session
     * error.
     *
     * The receiver detected a wrong SSRC during a session with our
     * remote peer. This could indicate a security problem - just
     * switch security off and Alert the user.
     */
    void rtpSessionError();

 private:
    ZRtp *zrtpEngine;
    SrtpSecret_t secret;

    MRef<IPAddress *> remoteAddress;
    
    MRef<MediaStreamReceiver *> rStream;
    uint32_t receiverSsrc;
    uint32_t receiverSecure;
    uint16_t receiverSeqNo;

    MRef<MediaStreamSender *> sStream;
    uint32_t senderSsrc;
    uint32_t senderSecure;
};

#endif // _ZIDHOSTBRIDGEMINISIP_H_
