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
#include<libmutil/MemObject.h>
#include<libmsip/SipSMCommand.h>

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
	
    /**
     * Initialize the host bridge.
     *
     * This static method must be called before any use of the host bridge. If the
     * caller does not provide filename for the ZID file the method opens the
     * ZID fie with the default name <e> ~/.minisip.zid<e/>. This is a binary
     * file.
     *
     * @param tp
     *    The timeout provider to use. In this case it shall be the sasme as defined
     *    for the SIP stack.
     * @param zidFilename
     *    Optional filename for the ZID file.
     * @return
     *    TODO
     */
    static int32_t initialze(MRef<TimeoutProvider<std::string, MRef<StateMachine<SipSMCommand,std::string>*> > *> tp,
		     char *zidFilename =NULL);

    ZrtpHostBridgeMinisip(uint8_t *zid);
    ~ZrtpHostBridgeMinisip();


    /**
     * Handle timeout event forwarded by TimeoutProvider.
     *
     * Just unwrap the cmd string into a char * and call the ZRTP engine for
     * further processing.
     */
    virtual void handleTimeout(const std::string &c) { zrtpEngine->processTimeout(c.c_str()); };

    virtual int32_t sendDataRTP(char *data, int32_t length);

    virtual int32_t sendDataSRTP(char *dataHeader, int32_t lengthHeader,
				 char *dataContent, int32_t lengthContent);

    virtual int32_t activateTimer(int32_t time, char *cmd) {
	std::string s(cmd);
	requestTimeout(time, s);
	return 1;
    };

    virtual int32_t cancelTimer(char *cmd) {
	std::string s(cmd);
	cancelTimeout(s);
	return 1;
    };

    ZRtp *getZrtpEngine() { return zrtpEngine; };

 private:
    ZRtp *zrtpEngine;
};

#endif // _ZIDHOSTBRIDGEMINISIP_H_
