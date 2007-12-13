/*
  Copyright (C) 2006 Werner Dittmann

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


/*
 * Authors: Werner Dittmann <Werner.Dittmann@t-online.de>
 */


#ifndef _ZIDHOSTBRIDGEMINISIP_H_
#define _ZIDHOSTBRIDGEMINISIP_H_

#ifdef ZRTP_SUPPORT
// #include<libminisip/libminisip_config.h>

#include <stdio.h>
#include <stdint.h>
#include <string.h>

#include <libmutil/StateMachine.h>
#include <libmutil/MessageRouter.h>
#include <libmsip/SipSMCommand.h>

#include <libminisip/media/rtp/SRtpPacket.h>
#include <libminisip/media/rtp/CryptoContext.h>

#include <libzrtpcpp/ZrtpCallback.h>
#include <libzrtpcpp/ZRtp.h>

class RealtimeMediaStreamReceiver;
class RealtimeMediaStreamSender;

/**
 * The connection between the ZRTP implementation and Minisip.
 *
 * The ZRPT implementation is fairly independent from the underlying
 * SIP and RTP/SRTP implementation. This class implements specific
 * functions and interfaces that ZRTP uses to call functions of the
 * hosting SIP client. In this case the host is Minisip.
 *
 * <p/>
 *
 * As required by ZRTP base implementation the bridge implements
 * the ZrtpCallback interface.
 *
 * <p/>
 *
 * The most minisip specific part is the implementation of the timer.
 * The minisip <e>startSip</e> method calls the bridge's
 * <e>initialize</e> method after the whole SIP was initialized. To
 * avoid a new timeout provider this bridge reuses the timeout
 * provider created by SipStack. Thus the initialize call looks like:
 *
 * <br>
 *
 * ZrtpHostBridge::initialize(sip->getSipStack()->getTimeoutProvider(),
filename?);
 *
 * <br/>
 *
 * The <code>initialize</code> method stores the timeout provider and
 * reuses it for every instance. To do so the bridge inherits from
 * Minisip's <e>StateMachine<e/> but does use the timeout specific
 * parts only. The destructor frees the StateMachine to maintain the
 * timout provider's reference counter.
 */

class ZrtpHostBridgeMinisip : public StateMachine<SipSMCommand,std::string>,
public ZrtpCallback {

 public:

    virtual std::string getMemObjectType() const { return "ZrtpHostBridgeMinisip";}

    /**
     * Initialize the host bridge.
     *
     * This static method must be called before <e>any</e> use of the
     * host bridge. If the caller does not provide a filename for the
     * ZID file the method opens the ZID file with the default name
     * <e> ~/.minisip.zid<e/>. This is a binary file.
     *
     * @param tp
     *    The timeout provider to use. In this case it shall be the
     *    same as defined for the SIP stack.
     * @param zidFilename
     *    Optional filename for the ZID file.
     * @return
     *    TODO
     */
    static int32_t initialize(MRef<TimeoutProvider<std::string,
MRef<StateMachine<SipSMCommand,std::string>*> > *> tp,
		     const char *zidFilename =NULL);

    ZrtpHostBridgeMinisip(std::string id, MRef<CommandReceiver*> callback);
    ~ZrtpHostBridgeMinisip();


    void start();
    void stop();

    void setReceiver(MRef<RealtimeMediaStreamReceiver *> r);
    void setSsrcReceiver(uint32_t ssrc)             { receiverSsrc = ssrc; };
    uint32_t getSsrcReceiver()                      { return receiverSsrc; };

    void setSender(MRef<RealtimeMediaStreamSender *> s);
    void setSsrcSender(uint32_t ssrc)               { senderSsrc = ssrc; };
    uint32_t getSsrcSender()                        { return senderSsrc; };

    bool isSecureState();

    void setCallId(std::string id)                  { callId = id; }
    /**
     * Set the IP address of our remote peer.
     *
     * The host (Minisip) shall call this mehtod to set the IP address
     * of the remote peer. We use the address to find the right ZRTP
     * host bridge when we receive packets on the receiver port
     * allocated by the RealtimeMediaStreamReceiver.
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
     * Process a received packet with an extension header.
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
     * Handle timeout event forwarded by Minisip's (SipStack)
     * TimeoutProvider.
     *
     * Just call the ZRTP engine for further processing.
     */
    void handleTimeout(const std::string & /* c */ ) {
        if (zrtpEngine != NULL) {
            zrtpEngine->processTimeout();
        }
    }

    /*
     * Refer to ZrtpCallback.h
     */
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

    void sendInfo(MessageSeverity severity, const char* msg) {
	fprintf(stderr, "Severity: %d - %s\n", severity, msg);
    }

    /**
     * This method shall handle GoClear requests.
     *
     * According to the ZRTP specification the user must be informed about
     * this message because the ZRTP implementation switches off security
     * if it could authenticate the GoClear packet.
     *
     */
    void handleGoClear() {
        fprintf(stderr, "Need to process a GoClear message!");
    }

    /**
     * Switch on the security for the defined part.
     *
     * Create an CryproContext with the negotiated ZRTP data and
     * register it with the respective part (sender or receiver) thus
     * replacing the current active context (usually an empty
     * context). This effectively enables SRTP.
     *
     * @param secrets
     *    The secret keys and salt negotiated by ZRTP
     * @param part
     *    An enum that defines sender, receiver, or both.
     */
    void srtpSecretsReady(SrtpSecret_t* secrets, EnableSecurity part);

    /**
     * This method shall switch on GUI inidicators.
     *
     * @param c
     *    The name of the used cipher algorithm and mode, or NULL
     * @param s
     *    The SAS string or NULL
     */
    virtual void srtpSecretsOn(const char* c, const char* s);


    /**
     * Switch off the security for the defined part.
     *
     * Create an empty CryproContext and register it with the
     * repective part (sender or receiver) thus replacing the current
     * active context. This effectively disables SRTP.
     *
     * @param part
     *    An enum that defines sender, receiver, or both.
     */
    void srtpSecretsOff(EnableSecurity part);

    /**
     * ZRTP calls this if the negotiation failed.
     *
     * ZRTP calls this method in case ZRTP negotiation failed. The parameters
     * show the severity as well as some explanatory text.
     * Refer to the <code>MessageSeverity</code> enum above.
     *
     * @param severity
     *     This defines the message's severity
     * @param msg
     *     The message string, terminated with a null byte.
         */
    void zrtpNegotiationFailed(MessageSeverity severity, const char* msg);

    /**
     * ZRTP calls this methof if the other side does not support ZRTP.
     *
     * If the other side does not answer the ZRTP <em>Hello</em> packets then
     * ZRTP calls this method,
     *
     */
    void zrtpNotSuppOther();

    /**
     * This method switches off secure state because of a session
     * error.
     *
     * The receiver detected a wrong SSRC during a session with our
     * remote peer. This could indicate a security problem - just
     * disable SRTP and alert the user.
     */
    void rtpSessionError();

    /**
     * Set the zfoneDeadBeef flag.
     *
     * This flag indicates the special Zfone maker SSRC 0xdeadbeef.
     *
     * @param onOff
     *     A value of one indicates that we detected a marker SSRC.
     */
    void setZfoneDeadBeef(int8_t onOff)  { zfoneDeadBeef = onOff; }

    /**
     * Get the zfoneDeadBeef flag.
     *
     * This flag indicates the special Zfone maker SSRC 0xdeadbeef.
     *
     * @return the value of zfoneDeadBeef flag. One indicates that
     *     we detected a marker SSRC
     */
    int8_t getZfoneDeadBeef()           {return zfoneDeadBeef; }

    uint16_t getZrtpSendSeqNo()         { return senderZrtpSeqNo++; }

    uint32_t getZrtpSendSsrc()          { return senderZrtpSsrc; }

    MRef<CryptoContext *> newCryptoContextForRecvSSRC(uint32_t ssrc, int roc, uint16_t seq,
                                                      int64_t keyDerivRate);

    bool isZrtpPacket(MRef<SRtpPacket *> packet);

 private:
    ZRtp *zrtpEngine;
    SrtpSecret_t secret;
    int32_t secureParts;

    MRef<IPAddress *> remoteAddress;

    MRef<RealtimeMediaStreamReceiver *> rStream;
    uint32_t receiverSsrc;
    uint32_t receiverSecure;
    uint16_t receiverSeqNo;

    MRef<RealtimeMediaStreamSender *> sStream;
    uint32_t senderSsrc;
    uint32_t senderSecure;

    bool enableZrtp;

    uint32_t recvZrtpSsrc;
    uint16_t recvZrtpSeqNo;
    MRef<CryptoContext *> recvCryptoContext;

    uint32_t senderZrtpSsrc;
    uint16_t senderZrtpSeqNo;
    MRef<CryptoContext *> senderCryptoContext;

    /*
     * The call id of our call
     */
    std::string callId;

    MRef<CommandReceiver*> messageRouterCallback;

    /**
     * This flag is true if we saw the special <em>0xdeadbeef</em> marker
     * SSRC. The Zfone implementation uses this in its ZRTP packets. Other
     * ZRTP implementation may not require such a marker SSRC.
     * (maybe even Zfone could live without it but ...)
     */
    int8_t zfoneDeadBeef;
};

#endif // ZRTP_SUPPORT

#endif // _ZIDHOSTBRIDGEMINISIP_H_
