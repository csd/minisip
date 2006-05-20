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
  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307
  USA
*/

/*
 * Authors: Werner Dittmann <Werner.Dittmann@t-online.de>
 */

#ifndef _ZRTP_H_
#define _ZRTP_H_

#include <libminisip/zrtp/ZrtpPacketHello.h>
#include <libminisip/zrtp/ZrtpPacketHelloAck.h>
#include <libminisip/zrtp/ZrtpPacketCommit.h>
#include <libminisip/zrtp/ZrtpPacketDHPart.h>
#include <libminisip/zrtp/ZrtpPacketConfirm.h>
#include <libminisip/zrtp/ZrtpPacketConf2Ack.h>
#include <libminisip/zrtp/ZrtpCallback.h>
#include <libmcrypto/ZrtpDH.h>

class ZrtpStateClass;

/**
 * The main ZRTP class.
 *
 * This contains the whole ZRTP implementation. It handles the ZRTP
 * HMAC, DH, and other data management. The user of this class needs
 * to know only a few methods and needs to provide only a few external
 * functions to connect to a Timer mechanism and to send data via RTP
 * and SRTP.
 *
 * <p/> 
 *
 * The main entry into the ZRTP class is the <e>
 * processExtensionHeader() </e> method.
 *
 * <p/> 
 *
 * This class does not directly handle the protocol states, timers,
 * and packet resend. The protocol state engine is responsible for
 * these actions.
 */
class ZRtp {

 private:

    /**
     * The state engine takes care of protocol processing.
     */
    ZrtpStateClass *stateEngine;

    /**
     * This is my ZID that I send to the peer.
     */
    uint8_t *zid;

    /**
     * The callback class provides me with the interface to send
     * data and to deal with timer management of the hosting system.
     */
    ZrtpCallback *callback;

    /**
     * My active Diffie-Helman context
     */
    ZrtpDH *dhContext;

    /**
     * Pre-initialized packets to start off the whole game.
     */
    ZrtpPacketHello zrtpHello;
    ZrtpPacketHelloAck zrtpHelloAck;

 public:

    /**
     * Constructor intializes all relevant data, does not start the
     * engine.
     */
    ZRtp(uint8_t *myZid, ZrtpCallback *cb);

    /**
     * Destructor cleans up.
     */
    ~ZRtp();

    /**
     * Kick off the ZRTP protocol engine.
     *
     * This method calls the <e>Initialize</e> state of the state
     * engine. After this call we are able to receive ZRTP packets
     * from our peer and to process them.
     */
    void startZrtpEngine();

    /**
     * Process RTP extension header.
     *
     * This method expects to get a pointer to the extension header of
     * a RTP packet. The method checks if this is really a ZRTP
     * packet, if this check fails the method returns 0 (false) in
     * case this is not a ZRTP packet. We return a 1 if we processed
     * the ZRTP extension header and the caller may process RTP data
     * after the extension header as usual.  The method return -1 the
     * call shall dismiss the packet and shall not forward it to
     * further RTP processing.
     *
     * @param extHeader
     *    A pointer to the first byte of the extension header. Refer to
     *    RFC3550.
     * @return
     *    Code indicating further packet handling, see description above.
     */
    int32_t processExtensionHeader(char *extHeader);

    /**
     * Process a timeout event.
     *
     * We got a timeout from the timeout provider. Forward it to the
     * protocol state engine.
     *
     * @param cmd
     *    A pointer to a string that identifies the timer.
     */
    void processTimeout(const char *cmd);

    /**
     * Send a ZRTP packet.
     *
     * The state engines calls this method to send a packet via the RTP
     * stack.
     *
     * @param packet
     *    Points to the ZRTP packet.
     * @return
     *    zero if sending failed, one if packet was send
     */
    int32_t sendPacketRTP(ZrtpPacketBase *packet);

    /*
     * Send a ZRTP packet using SRTP.
     *
     * The state engines calls this method to send a packet via the SRTP
     * stack.
     *
     * @param packet
     *    Points to the ZRTP packet.
     * @return
     *    zero if sending failed, one if packet was send
     */
    int32_t sendPacketSRTP(ZrtpPacketBase *packet);

    /*
     * Activate a Timer using the host callback.
     *
     * @param tm
     *    The time in milliseconds.
     * @return
     *    zero if activation failed, one if timer was activated
     */
    int32_t activateTimer(int32_t tm) {return (callback->activateTimer(tm)); };

    /**
     * Prepare a Hello packet.
     *
     * Just take the preinitialized Hello packet and return it. No
     * further processing required.
     *
     * @return
     *    A pointer to the initialized Hello packet.
     */
    ZrtpPacketHello *prepareHello() {return &zrtpHello; };

    /**
     * Prepare a HelloAck packet.
     *
     * Just take the preinitialized HelloAck packet and return it. No
     * further processing required.
     *
     * @return
     *    A pointer to the initialized HelloAck packet.
     */
    ZrtpPacketHelloAck *prepareHelloAck() {return &zrtpHelloAck; };

    /*
     * Prepare a Commit packet.
     *
     * We have received a Hello packet from our peer. Check the offers
     * it makes to us and select the most appropriate. Using the
     * selected values prepare a Commit packet and return it to protocol
     * state engine.
     * 
     * @param hello
     *    Points to the received Hello packet
     * @return
     *    A pointer to the prepared Commit packet
     */
    ZrtpPacketCommit *prepareCommit(ZrtpPacketHello *hello);

    /**
     * Prepare the DHPart1 packet.
     */
    ZrtpPacketDHPart *prepareDHPart1(ZrtpPacketCommit *commit);

    /**
     * Prepare the DHPart2 packet.
     */
    ZrtpPacketDHPart *prepareDHPart2(ZrtpPacketDHPart *dhPart1);

    /**
     * Prepare the Confirm1 packet.
     */
    ZrtpPacketConfirm *prepareConfirm1(ZrtpPacketDHPart *dhPart2);

    /**
     * Prepare the Confirm2 packet.
     */
    ZrtpPacketConfirm *prepareConfirm2(ZrtpPacketConfirm *confirm1);

    /**
     * Prepare the Conf2Ack packet.
     */
    ZrtpPacketConf2Ack *prepareConf2Ack(ZrtpPacketConfirm *confirm2);

    /**
     * Compare the hvi values.
     *
     * Compares the hvi hashes of both commit packets, the one we just
     * received and the one we sent in response of peer's hello. The
     * outcome of the compare determines which peer is Initiator or
     * Responder. If the hvi of the commit we sent then we are
     * Responder, otherwise we are Inititiator.
     *
     * @param commit
     *    Pointer to the peer's commit packet we just received.
     * @return
     *    -1 if our hvi is smaller
     *     1 if our hvi is bigger
     *     0 shouldn't happen because we compare crypto hashes
     */
    int32_t compareHvi(ZrtpPacketCommit *commit);

};



#endif // ZRTP

