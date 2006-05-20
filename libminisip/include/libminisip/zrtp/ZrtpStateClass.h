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

#ifndef _ZRTPSTATECLASS_H_
#define _ZRTPSTATECLASS_H_

#include <libminisip/zrtp/ZrtpStates.h>
#include <libminisip/zrtp/ZrtpPacketBase.h>

// The ZRTP states
enum zrtpStates {
    Initial,
    Detect,
    AckDetected,
    WaitCommit,
    CommitSent,
    WaitDHPart2,
    WaitConfirm1,
    WaitConfirm2,
    WaitConfAck,
    SecureState,
    numberOfStates
};


enum EventDataType {
    ZrtpPacket = 1,
    Timer
};

typedef struct Event {
    EventDataType type;
    union {
	ZrtpPacketBase *packet;
	int32_t *timer;
    } data;
} Event_t;


/**
 * The ZRTP timer structure.
 *
 * This structure holds all necessary data to compute the timer for
 * the protocol timers. The state engine allocate one structure for
 * each timer. ZRTP uses two timers, T1 and T2, to monitor protocol
 * timeouts. As a slight misuse but to make overall handling a bit
 * simpler this structure also contains the resend counter. This is
 * possible in ZRTP because it uses a simple timeout strategy.
 */
typedef struct zrtpTimer {
    int32_t time,
	start,
	increment,
	capping,
	counter,
	maxResend,
	stop;
} zrtpTimer_t;


class ZRtp;

/**
 * This class is the ZRTP protocol state engine.  This class is
 * responsible to handle the ZRTP protocol. It does not handle the
 * ZRTP HMAC, DH, and other data management. This is done in class
 * ZRtp which is the parent of this class.
 *
 * <p/>
 * The methods of this class implement the ZRTP state actions.
 */
class ZrtpStateClass {

private:
    ZRtp *parent;
    ZrtpStates *engine;
    Event_t *event;

    /**
     * The last packet that was sent.
     *
     * If we are <e>Initiator</e> then resend this packet in case of
     * timeout.
     */
    ZrtpPacketBase *sentPacket;

    zrtpTimer_t T1;
    zrtpTimer_t T2;

public:
    ZrtpStateClass(ZRtp *p);
    ~ZrtpStateClass();

    int32_t inState(const int32_t state) { return engine->inState(state); };
    void nextState(int32_t state)        { engine->nextState(state); };
    int32_t processEvent(Event_t *ev);

    /**
     * The state event handling methods.
     *
     * Refer to the protocol state diagram for further documentation.
     */
    int32_t evInitial();
    int32_t evDetect();
    int32_t evAckDetected();
    int32_t evWaitCommit();
    int32_t evCommitSent();
    int32_t evWaitDHPart2();
    int32_t evWaitConfirm1();
    int32_t evWaitConfirm2();
    int32_t evWaitConfAck();
    int32_t evSecureState();

    /**
     * Initialize and activate a timer.
     *
     * @param
     *    The ZRTP timer structure to use for the timer.
     * @return
     *    1 timer was activated
     *    0 activation failed
     */
    int32_t startTimer(zrtpTimer_t *t);

    /**
     * Compute and set the next timeout value.
     *
     * @param
     *    The ZRTP timer structure to use for the timer.
     * @return
     *    1 timer was activated
     *    0 activation failed
     *   -1 resend counter exceeded
     */
    int32_t nextTimer(zrtpTimer_t *t);

};

#endif // _ZRTPSTATECLASS_H_

