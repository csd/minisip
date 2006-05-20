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

#include<config.h>

#include <libminisip/zrtp/ZrtpHostBridgeMinisip.h>
#include <libminisip/zrtp/ZIDFile.h>
#include <libminisip/zrtp/ZRtp.h>

static MRef<TimeoutProvider<std::string, MRef<StateMachine<SipSMCommand,std::string>*> > *>staticTimeoutProvider;

int32_t ZrtpHostBridgeMinisip::initialze(MRef<TimeoutProvider<std::string, MRef<StateMachine<SipSMCommand,std::string>*> > *>tp,
				  char *zidFilename) {
    staticTimeoutProvider = tp;
    ZIDFile *zf = ZIDFile::getInstance();
    zf->open(zidFilename);
    return 1;
}

ZrtpHostBridgeMinisip::ZrtpHostBridgeMinisip(uint8_t *zid): 
    StateMachine<SipSMCommand,std::string>(staticTimeoutProvider) {

    zrtpEngine = new ZRtp(zid, static_cast<ZrtpCallback *>(this));
} 

ZrtpHostBridgeMinisip::~ZrtpHostBridgeMinisip() {

    freeStateMachine();
}
