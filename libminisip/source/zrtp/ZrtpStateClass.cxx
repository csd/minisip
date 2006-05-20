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

#include <iostream.h>
#include <assert.h>
#include <ctype.h>
#include <libminisip/zrtp/ZRtp.h>
#include <libminisip/zrtp/ZrtpStateClass.h>

state_t states[numberOfStates] = {
    {Initial,      &ZrtpStateClass::evInitial },
    {Detect,       &ZrtpStateClass::evDetect },
    {AckDetected,  &ZrtpStateClass::evAckDetected },
    {WaitCommit,   &ZrtpStateClass::evWaitCommit },
    {CommitSent,   &ZrtpStateClass::evCommitSent },
    {WaitDHPart2,  &ZrtpStateClass::evWaitDHPart2 },
    {WaitConfirm1, &ZrtpStateClass::evWaitConfirm1 },
    {WaitConfirm2, &ZrtpStateClass::evWaitConfirm2 },
    {WaitConfAck,  &ZrtpStateClass::evWaitConfAck },
    {SecureState,  &ZrtpStateClass::evSecureState }
};

ZrtpStateClass::ZrtpStateClass(ZRtp *p) {
    parent = p;
    engine = new ZrtpStates(states, numberOfStates, Initial);

    // Set up timers according to ZRTP spec
    T1.start = 50;
    T1.maxResend = 20;
    T1.capping = 200;

    T2.start = 150;
    T2.maxResend = 10;
    T2.capping = 600;
}

ZrtpStateClass::~ZrtpStateClass(void) {
    if (engine != NULL) {
	delete engine;
    }
}


int32_t ZrtpStateClass::processEvent(Event_t *ev) {

    if (inState(Initial)) {
	return (Done);
    }
    event = ev;
    return engine->processEvent(*this);
}


int32_t ZrtpStateClass::evInitial(void) {
    cout << "Checking for match in Initial.\n";

    ZrtpPacketHello *hello = parent->prepareHello();
    if (!parent->sendPacketRTP(static_cast<ZrtpPacketBase *>(hello))) {
	// Huston, we have a problem
    }
    nextState(Detect);

    // remember packet for easy resend in case timer triggers
    sentPacket = static_cast<ZrtpPacketBase *>(hello);
    if (startTimer(&T1) <= 0) {
	// Yet another problem
    }
    return (Done);
}

int32_t ZrtpStateClass::evDetect(void) {

    cout << "Checking for match in Detect.\n";

    char *msg, first, last;
    ZrtpPacketBase *pkt;

    if (event->type == ZrtpPacket) {
	pkt = event->data.packet;
	msg = pkt->getMessage();
	  
	first = tolower(*msg);
	last = tolower(*(msg+7));

	/* 
	 * Commit:
	 * - go the responder path
	 * - send our DHPart1
	 * - switch to state WaitDHPart2, wait for peer's DHPart2
	 * - don't start timer, we are responder
	 */
	if (first == 'c') {
	    T1.stop = 1;	// stop Hello timer processing
	    ZrtpPacketDHPart* dhPart1 = 
		parent->prepareDHPart1(static_cast<ZrtpPacketCommit *>(pkt));

	    if (!parent->sendPacketRTP(static_cast<ZrtpPacketBase *>(dhPart1))) {
		// Huston, we have a problem
	    }
	    nextState(WaitDHPart2);
	    return (Done);
	}
	/*
	 * HelloAck:
	 * - stop resending Hello, 
	 * - switch to state AckDetected, wait for peer's Hello
	 */
	if (first == 'h' && last =='k') {
	    T1.stop = 1;	// stop Hello timer processing
	    nextState(AckDetected);
	    return (Done);	  
	}
	/*
	 * Hello:
	 * - stop resending Hello
	 * - prepare and send my Commit, 
	 * - switch state to CommitSent
	 * - start timer - we are in Initiator role here
	 */
	if (first == 'h' && last ==' ') {
	    T1.stop = 1;	// stop Hello timer processing
	    ZrtpPacketCommit* commit = 
		parent->prepareCommit(static_cast<ZrtpPacketHello *>(pkt));

	    if (!parent->sendPacketRTP(static_cast<ZrtpPacketBase *>(commit))){
		// Huston, we have a problem
	    }
	    nextState(CommitSent);

	    // remember packet for easy resend in case timer triggers
	    sentPacket = static_cast<ZrtpPacketBase *>(commit);
	    if (startTimer(&T2) <= 0) {
		// Yet another problem
	    }
	    return (Done);
	}
    }
    // Timer event triggered
    else {
	// Timer triggered but we got a message during that time that
	// stopped further timer processing. Ignore the trigger
	if (T1.stop) {
	    return (Done);
	}
	if (nextTimer(&T1) > 0 && parent->sendPacketRTP(sentPacket)) {
	    return (Done);
	}
    }
    // Error packet ??
    return (Fail);
}

int32_t ZrtpStateClass::evAckDetected(void) {

    cout << "Checking for match in AckDetected.\n";

    char *msg, first, last;
    ZrtpPacketBase *pkt;

    if (event->type == ZrtpPacket) {
	pkt = event->data.packet;
	msg = pkt->getMessage();
	  
	first = tolower(*msg);
	last = tolower(*(msg+7));
	/*
	 * Hello:
	 * - stop resending Hello
	 * - Acknowledge the Hello
	 * - switch to state WaitCommit, wait for peer's Commit
	 */
	if (first == 'h') {
	    T1.stop = 1;	// stop Hello timer processing
	    ZrtpPacketHelloAck *helloAck = parent->prepareHelloAck();
	    if (!parent->sendPacketRTP(static_cast<ZrtpPacketBase *>(helloAck))) {
		// Huston, we have a problem
	    }
	    nextState(WaitCommit);
	    // remember packet for easy resend
	    sentPacket = static_cast<ZrtpPacketBase *>(helloAck);
	    return (Done);
	}
    }
    // TODO: Error packet ??
    return (Fail);
}

int32_t ZrtpStateClass::evWaitCommit(void) {

    cout << "Checking for match in WaitCommit.\n";

    char *msg, first, last;
    ZrtpPacketBase *pkt;

    if (event->type == ZrtpPacket) {
	pkt = event->data.packet;
	msg = pkt->getMessage();
	  
	first = tolower(*msg);
	last = tolower(*(msg+7));

	/*
	 * Hello:
	 * - resend HelloAck
	 * - stay in WaitCommit
	 */
	if (first == 'h') {
	    if (!parent->sendPacketRTP(sentPacket)) {
		// Huston, we have a problem
	    }
	    return (Done);
	}
	/*
	 * Commit:
	 * - prepare DH1Part packet 
	 * - send it to peer
	 * - switch state to WaitDHPart2
	 * - don't start timer, we are responder
	 */
	if (first == 'c') {
	    ZrtpPacketDHPart* dhPart1 = 
		parent->prepareDHPart1(static_cast<ZrtpPacketCommit *>(pkt));

	    if (!parent->sendPacketRTP(static_cast<ZrtpPacketBase *>(dhPart1))){
		// Huston, we have a problem
	    }
	    nextState(WaitDHPart2);
	    sentPacket = static_cast<ZrtpPacketBase *>(dhPart1);
	    return (Done);	  
	}
    }
    // TODO: Error packet ??
    return (Fail);
}

int32_t ZrtpStateClass::evCommitSent(void) {

    cout << "Checking for match in CommitSend.\n";

    char *msg, first, last;
    ZrtpPacketBase *pkt;

    if (event->type == ZrtpPacket) {
	pkt = event->data.packet;
	msg = pkt->getMessage();
	  
	first = tolower(*msg);
	last = tolower(*(msg+7));

	/*
	 * Commit:
	 * - switch off resending Commit?? probably not
	 * - compare my hvi with peer's hvi
	 * - if my hvi is greater
	 *   - I'm Initiator, stay in state
	 * - else 
	 *   - stop timer
	 *   - prepare and send DH1Packt, 
	 *   - switch to state WaitDHPart2, implies Responder path
	 */
	if (first == 'c') {
	    if (parent->compareHvi(static_cast<ZrtpPacketCommit *>(pkt))< 0) {
		T2.stop = 1;
		ZrtpPacketDHPart* dhPart1 = 
		    parent->prepareDHPart1(static_cast<ZrtpPacketCommit *>(pkt));
	
		if (!parent->sendPacketRTP(static_cast<ZrtpPacketBase *>(dhPart1))){
		    // Huston, we have a problem
		}
		nextState(WaitDHPart2);
		sentPacket = static_cast<ZrtpPacketBase *>(dhPart1);
	    }
	    // TODO: what about Timer: we depend on that fact that
	    // peer _will_ send a DHPart1 because it is Responder.
	    return (Done);
	}

	/*
	 * DHPart1:
	 * - switch off resending Commit
	 * - Prepare and send DHPart2
	 * - switch to WaitConfirm1
	 * - start timer to resend DHPart2 if necessary
	 */
	if (first == 'd') {
	    T2.stop = 1;
	    ZrtpPacketDHPart* dhPart2 = 
		parent->prepareDHPart2(static_cast<ZrtpPacketDHPart *>(pkt));

	    if (!parent->sendPacketRTP(static_cast<ZrtpPacketBase *>(dhPart2))){
		// Huston, we have a problem
	    }
	    nextState(WaitConfirm1);
	    sentPacket = static_cast<ZrtpPacketBase *>(dhPart2);
	    startTimer(&T2);
	    return (Done);
	}
    }
    else {
	// Timer triggered but we got a message during that time
	// that stopped further timer processing. Ignore the trigger
	if (T2.stop) {
	    return (Done);
	}
	if (nextTimer(&T2) > 0 && parent->sendPacketRTP(sentPacket)) {
	    return (Done);
	}
    }
    // Error packet ??
    return (Fail);
}

int32_t ZrtpStateClass::evWaitDHPart2(void) {

    cout << "Checking for match in DHPart2.\n";

    char *msg, first, last;
    ZrtpPacketBase *pkt;

    if (event->type == ZrtpPacket) {
	pkt = event->data.packet;
	msg = pkt->getMessage();
	  
	first = tolower(*msg);
	last = tolower(*(msg+7));

	/*
	 * Commit:
	 * - resend DHPart1
	 * - stay in state
	 */
	if (first == 'c') {
	    if (!parent->sendPacketRTP(sentPacket)) {
		// Huston, we have a problem
	    }
	    return (Done);
	}
	/*
	 * DHPart2:
	 * - prepare Confirm1 packet
	 * - send it via SRTP
	 * - switch to WaitConfirm2
	 * - No timer, we are responder
	 */
	if (first == 'd') {
	    ZrtpPacketConfirm* confirm = 
		parent->prepareConfirm1(static_cast<ZrtpPacketDHPart *>(pkt));
	    if (!parent->sendPacketSRTP(static_cast<ZrtpPacketBase *>(confirm))){
		// Huston, we have a problem
	    }
	    nextState(WaitConfirm2);
	    sentPacket = static_cast<ZrtpPacketBase *>(confirm);
	    return (Done);
	}
    }
    // TODO: Error packet ??
    return (Fail);
}

int32_t ZrtpStateClass::evWaitConfirm1(void) {

    cout << "Checking for match in WaitConfirm1.\n";

    char *msg, first, last;
    ZrtpPacketBase *pkt;

    if (event->type == ZrtpPacket) {
	pkt = event->data.packet;
	msg = pkt->getMessage();
	  
	first = tolower(*msg);
	last = tolower(*(msg+7));

	/*
	 * Confirm1:
	 * - Switch off resending DHPart2
	 * - prepare a Confirm2 packet 
	 * - send via SRTP, 
	 * - switch to state WaitConfAck
	 */
	if (first == 'c' && last == '1') {
	    ZrtpPacketConfirm* confirm = 
		parent->prepareConfirm2(static_cast<ZrtpPacketConfirm *>(pkt));
	    if (!parent->sendPacketSRTP(static_cast<ZrtpPacketBase *>(confirm))){
		// Huston, we have a problem
	    }
	    nextState(WaitConfAck);
	    sentPacket = static_cast<ZrtpPacketBase *>(confirm);
	    startTimer(&T2);
	    return (OkDismiss);
	}
    }
    else {
	// Nothing received from peer, resend DHPart2 until counter exceeds
	if (T2.stop) {
	    return (Done);
	}
	if (nextTimer(&T2) > 0 && parent->sendPacketRTP(sentPacket)) {
	    return (Done);
	}
    }
    // TODO: Error packet ??
    return (Fail);
}

int32_t ZrtpStateClass::evWaitConfirm2(void) {

    cout << "Checking for match in WaitConfirm2.\n";

    char *msg, first, last;
    ZrtpPacketBase *pkt;

    if (event->type == ZrtpPacket) {
	pkt = event->data.packet;
	msg = pkt->getMessage();
	  
	first = tolower(*msg);
	last = tolower(*(msg+7));

	/*
	 * DHPart2:
	 * - resend Confirm2 packet via SRTP
	 * - stay in state
	 */
	if (first == 'd') {
	    if (!parent->sendPacketSRTP(sentPacket)) {
		// Huston, we have a problem
	    }
	    return (Done);
	}
	/*
	 * Confirm2:
	 * - prepare ConfAck 
	 * - send via SRTP
	 * - switch to SecureState
	 */
	if (first == 'c' && last == '2') {
	    // send ConfAck via SRTP
	    ZrtpPacketConf2Ack* confack = 
		parent->prepareConf2Ack(static_cast<ZrtpPacketConfirm *>(pkt));
	    if (!parent->sendPacketSRTP(static_cast<ZrtpPacketBase *>(confack))){
		// Huston, we have a problem
	    }
	    nextState(SecureState);
	    return (OkDismiss);
	}
    }
    // TODO: Error packet ??
    return (Fail);
}

int32_t ZrtpStateClass::evWaitConfAck(void) {

    cout << "Checking for match in WaitConfAck.\n";

    char *msg, first, last;
    ZrtpPacketBase *pkt;

    if (event->type == ZrtpPacket) {
	pkt = event->data.packet;
	msg = pkt->getMessage();
	  
	first = tolower(*msg);
	last = tolower(*(msg+7));

	/*
	 * ConfAck:
	 * - Switch off resending Confirm2
	 * - switch to SecureState
	 */
	if (first == 'c') {
	    T2.stop = 1;
	    nextState(SecureState);
	    return (OkDismiss);
	}
    }
    else {
	// Nothing received from peer, resend Confirm2 until counter exceeds
	if (T2.stop) {
	    return (Done);
	}
	if (nextTimer(&T2) > 0 && parent->sendPacketSRTP(sentPacket)) {
	    return (Done);
	}
    }
    // TODO: Error packet ??
    return (Fail);
}

int32_t ZrtpStateClass::evSecureState(void) {

    cout << "Checking for match in SecureState.\n";

    char *msg, first, last;
    ZrtpPacketBase *pkt;

    if (event->type == ZrtpPacket) {
	pkt = event->data.packet;
	msg = pkt->getMessage();
	  
	first = tolower(*msg);
	last = tolower(*(msg+7));

	/*
	 * Confirm2:
	 * - resend ConfAck packet via SRTP
	 * - stay in state
	 */
	if (first == 'c') {
	    if (!parent->sendPacketSRTP(sentPacket)) {
		// Huston, we have a problem
	    }
	    return (Done);
	}
    }
    // Perform clean up, switch off SRTP
    // nextState(Initial) ??
    return (Done);
}

int32_t ZrtpStateClass::startTimer(zrtpTimer_t *t) {

    t->time = t->start;
    t->counter = 0;
    t->stop = 0;
    return parent->activateTimer(t->time);
}

int32_t ZrtpStateClass::nextTimer(zrtpTimer_t *t) {

    t->time += t->time;
    t->time = (t->time > t->capping)? t->capping : t->time;
    t->counter++;
    if (t->counter > t->maxResend) {
	return -1;
    }
    return parent->activateTimer(t->time);
}

