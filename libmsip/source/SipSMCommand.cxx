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


#include<config.h>


#include<libmsip/SipSMCommand.h>
#include<libmutil/dbg.h>
#include<libmsip/SipMessage.h>

using namespace std;

const int SipSMCommand::COMMAND_PACKET=1;
const int SipSMCommand::COMMAND_STRING=2;
//const int SipSMCommand::remote=1;
const int SipSMCommand::dialog_layer=2;
const int SipSMCommand::transaction_layer=3;
//const int SipSMCommand::callback=4;
const int SipSMCommand::dispatcher=5;
const int SipSMCommand::transport_layer=6;


Dbg & operator<<(Dbg &o, const SipSMCommand &c){
	const char *s[6]={"(illegal)","dialog_layer","transaction_layer","(illegal)","dispatcher","transport_layer"};
	if (c.type==SipSMCommand::COMMAND_PACKET){
                
		o <<"COMMAND_PACKET:"
                    << (**c.getCommandPacket()).getDescription() 
                    <<" source="<< s[c.source-1]
                    <<" dest="<<s[c.destination-1];
        }else{
		o <<"COMMAND_STRING:"<<c.getCommandString().getString()
                    <<",source="<< s[c.source-1]
                    <<" dest="<<s[c.destination-1];
		
        }
	return o;
}

#ifndef _WIN32_WCE
ostream & operator<<(ostream &o, const SipSMCommand &c){
	const char *s[6]={"(illegal)","dialog_layer","transaction_layer","(illegal)","dispatcher","transport_layer"};
	if (c.type==SipSMCommand::COMMAND_PACKET){
                
		o <<"COMMAND_PACKET:"
                    << (**c.getCommandPacket()).getDescription() 
                    <<" source="<< s[c.source-1]
                    <<" dest="<<s[c.destination-1];
        }else{
		o <<"COMMAND_STRING:"<<c.getCommandString().getString()
                    <<",source="<< s[c.source-1]
                    <<" dest="<<s[c.destination-1];
		
        }
	return o;
}
#endif

int SipSMCommand::getType() const {
	return type;
}

int SipSMCommand::getDestination() const {
	return destination;
}

void SipSMCommand::setDestination(int i){
	destination=i;
}

string SipSMCommand::getDestinationId() const{
	if (type==SipSMCommand::COMMAND_PACKET)
		return cmdpkt->getCallId();
	else
		return cmdstr.getDestinationId();
}

int SipSMCommand::getSource() const{
	return source;
}

void SipSMCommand::setSource(int i){
	source=i;
}

SipSMCommand::SipSMCommand(MRef<SipMessage*> p, 
		int s, 
		int d): 
			type(COMMAND_PACKET), 
			cmdstr("",""), 
			cmdpkt(p), 
			source(s), 
			destination(d)
{

}

SipSMCommand::SipSMCommand(const CommandString &cs, 
		int s, 
		int d): 
			type(COMMAND_STRING), 
			cmdstr(cs),
			cmdpkt(NULL),
			source(s), 
			destination(d)
{

}

MRef<SipMessage*> SipSMCommand::getCommandPacket()const {
	return cmdpkt;
}

CommandString SipSMCommand::getCommandString()const{
	return cmdstr;
}

