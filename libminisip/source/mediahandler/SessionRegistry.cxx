/*
  Copyright (C) 2005, 2004 Erik Eliasson, Johan Bilien
  
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
  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/


/* Copyright (C) 2004, 2005
 *
 * Authors: Erik Eliasson <eliasson@it.kth.se>
 *          Johan Bilien <jobi@via.ecp.fr>
*/

#include<config.h>
#include<libminisip/SessionRegistry.h>
#include<libminisip/Session.h>
#include<libminisip/MediaStream.h>

using namespace std;

MRef<Session *> SessionRegistry::getSession( std::string callId ){
        list<MRef<Session *> >::iterator iSession;

        sessionsLock.lock();
        for( iSession = sessions.begin(); iSession != sessions.end(); iSession++ ){
                if( (*iSession)->getCallId() == callId ){
                        sessionsLock.unlock();
                        return *iSession;
                }
        }

        sessionsLock.unlock();
        return NULL;
}

void SessionRegistry::registerSession( MRef<Session *> session ){
        sessionsLock.lock();
        sessions.push_back( session );
        sessionsLock.unlock();
}

void SessionRegistry::unregisterSession( MRef<Session *> session ){
        sessionsLock.lock();
        sessions.remove( session );
        sessionsLock.unlock();
}
