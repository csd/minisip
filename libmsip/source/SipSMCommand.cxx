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

const int SipSMCommand::COMMAND_PACKET=1;
const int SipSMCommand::COMMAND_STRING=2;
const int SipSMCommand::remote=1;
const int SipSMCommand::TU=2;
const int SipSMCommand::transaction=3;
const int SipSMCommand::ANY=4;
const int SipSMCommand::DIALOGCONTAINER=5;

Dbg & operator<<(Dbg &o, const SipSMCommand &c){
	char *s[5]={"remote","TU","transaction","IGN","DIALOGCONTAINER"};
	if (c.type==SipSMCommand::COMMAND_PACKET){
                
		o <<"COMMAND_PACKET:"
                    << (**c.getCommandPacket()).getDescription() 
                    <<",source="<< s[c.source-1]
                    <<",dest="<<s[c.destination-1];
        }else{
		o <<"COMMAND_STRING:"<<c.getCommandString().getString()
                    <<",source="<< s[c.source-1]
                    <<",dest="<<s[c.destination-1];
			
        }
	return o;
}

int SipSMCommand::getType() const {
	return type;
}

int SipSMCommand::getDestination() const {
	return destination;
}

void SipSMCommand::setDestination(int i){
	destination=i;
}

int SipSMCommand::getSource() const{
	return source;
}

void SipSMCommand::setSource(int i){
	source=i;
}

SipSMCommand::SipSMCommand(MRef<SipMessage*> p, int source, int destination): type(COMMAND_PACKET), cmdstr("",""), cmdpkt(p), source(source), destination(destination), dispatchCount(0){

}

SipSMCommand::SipSMCommand(const CommandString &s, int source, int destination): type(COMMAND_STRING), cmdstr(s),cmdpkt(NULL),source(source), destination(destination), dispatchCount(0){

}

MRef<SipMessage*> SipSMCommand::getCommandPacket()const {
	return cmdpkt;
}

CommandString SipSMCommand::getCommandString()const{
	return cmdstr;
}

