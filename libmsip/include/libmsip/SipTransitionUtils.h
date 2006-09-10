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

/*
 * Authors: Erik Eliasson <eliasson@it.kth.se>
 *          Johan Bilien <jobi@via.ecp.fr>
*/

#ifndef SIPTRANSITIONUTILS_H
#define SIPTRANSITIONUTILS_H

#include<libmsip/libmsip_config.h>

#include<libmutil/MemObject.h>
#include<libmsip/SipSMCommand.h>
#include<string>

class SipResponse;

/**
 * Checks if a response packet has the response code indicated by a
 * pattern. The pattern may contain wild cards (*)
 * @param resp 		SIP response to check against, for example "100 OK"
 * @param pattern	Pattern, for example "100" or "1**"
 */
bool LIBMSIP_API sipResponseFilterMatch(MRef<SipResponse*> resp, const std::string &pattern);
	
#define IGN -1

bool LIBMSIP_API transitionMatch(
		const std::string& packetType,
		const SipSMCommand &command,
		int source,
		int destination,
		const std::string &respFilter="");

/** Match Sip responses */
bool LIBMSIP_API transitionMatchSipResponse(
		const std::string& cseqMethod,
		const SipSMCommand &command,
		int source,
		int destination,
		const std::string &respFilter="");

bool LIBMSIP_API transitionMatch(
		const SipSMCommand &command,
		const std::string &cmd_str,
		int source,
		int destination);

#endif

