/*
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Library General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

/* Copyright (C) 2004 
 *
 * Authors: Erik Eliasson <eliasson@it.kth.se>
 *          Johan Bilien <jobi@via.ecp.fr>
*/

#include<libmutil/MemObject.h>
#include<libmsip/SipSMCommand.h>

class SipResponse;

/**
 * Checks if a response packet has the response code indicated by a
 * pattern. The pattern may contain wild cards (*)
 * @param resp 		SIP response to check against, for example "100 OK"
 * @param pattern	Pattern, for example "100" or "1**"
 */
bool sipResponseFilterMatch(MRef<SipResponse*> resp, const string &pattern);
	

#define IGN -1

bool transitionMatch(const SipSMCommand &command,
	int packetType=IGN,
	int source=IGN,
	int destination=IGN,
	const string &respFilter="");

bool transitionMatch(const SipSMCommand &command,
	const string &command,
	int source=IGN,
	int destination=IGN);

