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

#include<config.h>


#include<libmsip/SipTransactionUtils.h>
#include<libmsip/SipResponse.h>
#include<libmutil/split_in_lines.h>

/**
 * Checks if a response packet has the response code indicated by a
 * pattern. The pattern may contain wild cards (*)
 * @param resp 		SIP response to check against, for example "100 OK"
 * @param pattern	Pattern, for example "100" or "1**"
 */
bool sipResponseFilterMatch(MRef<SipResponse*> resp, const string &pattern){
	int32_t status = resp->getStatusCode();

	if ( (pattern[0]=='*' || (status/100==(pattern[0]-'0'))) &&
			(pattern[1]=='*' || ((status/10)%10 == pattern[1]-'0')) &&
			(pattern[2]=='*' || (status%10 == pattern[2]-'0') )){
		return true;
	}else{
		return false;
	}
}




bool transitionMatch(const SipSMCommand &command,
		int packetType,
		int source,
		int destination,
		const string &respFilter)
{

	if (source!=IGN &&      command.getSource()!=SipSMCommand::ANY &&      command.getSource() != source){
		return false;
	}
	if (destination!=IGN && command.getDestination()!=SipSMCommand::ANY && command.getDestination() != destination){
		return false;
	}
	
	if (command.getType()!=SipSMCommand::COMMAND_PACKET){
		return false;
	}

	if (packetType!=IGN && command.getCommandPacket()->getType()!=packetType){
		return false;
	}

	if (respFilter.size()>0){
		vector<string> filters = split_in_lines(respFilter);
		for (vector<string>::iterator i=filters.begin(); i!=filters.end(); i++){
			if (sipResponseFilterMatch( MRef<SipResponse*> ( (SipResponse *)*command.getCommandPacket() ), *i )){
				return true;
			}
		}
		return false;
	}

	return true;
}


bool transitionMatch(const SipSMCommand &command,
		const string &cmd_str,
		int source,
		int destination)
{

	if (command.getType()!=SipSMCommand::COMMAND_STRING){
		return false;
	}
	if (destination!=IGN && destination!=SipSMCommand::ANY && command.getDestination() != destination){
		return false;
	}
	if (source!=IGN && source!=SipSMCommand::ANY && command.getSource() != source){
		return false;
	}
	if (command.getCommandString().getOp()!=cmd_str){
		return false;
	}
	return true;
}

