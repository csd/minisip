/*
 Copyright (C) 2004-2006 the Minisip Team
 
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

/* Copyright (C) 2004 
 *
 * Authors: Erik Eliasson <eliasson@it.kth.se>
 *          Johan Bilien <jobi@via.ecp.fr>
*/

/* Name
 * 	GroupList.h
 * Author
 * 	Florian Maurer, florian.maurer@floHweb.ch
 * Purpose
 *		
 * 
*/

#include <config.h>

#include<libminisip/signaling/p2t/GroupList.h>
#include<libmutil/stringutils.h>
#include<libmutil/XMLParser.h>

GroupList::GroupList():MObject(/*"GroupList"*/){
	setGroupIdentity("");
	setGroupOwner("");
	setDescription("");
	setSessionType(0);
	setMembership(0);
	setMaxFloorTime(0);
	setMaxParticipants(0);
}

GroupList::GroupList(string xml){
	XMLParser *parser= new XMLstringParser(xml);
		
	setGroupIdentity(parser->getValue("grouplist/id",""));
	setGroupOwner(parser->getValue("grouplist/owner", ""));
	setDescription(parser->getValue("grouplist/description", ""));
	setSessionType(parser->getIntValue("grouplist/type", 0));
	setMembership(parser->getIntValue("grouplist/membership",0));
	setMaxFloorTime(parser->getIntValue("grouplist/floortime",0));
	setMaxParticipants(parser->getIntValue("grouplist/maxparticipants",0));
	
	//memberlist
	for(int k=0; k<parser->getIntValue("grouplist/members/size",0);k++)
		addMember(parser->getValue("grouplist/members/member["+itoa(k)+"]/uri"));

	//userlist
	for(int l=0; l<parser->getIntValue("grouplist/participants/size",0);l++){
		addUser(parser->getValue("grouplist/participants/user["+itoa(l)+"]/uri"),
			parser->getIntValue("grouplist/participants/user["+itoa(l)+"]/prio",0));
	}
	
	
	
	delete parser;

}

GroupList::~GroupList(){

}

void GroupList::addMember(string uri){
	members.push_back(uri);
}

bool GroupList::isMember(string uri){
	for(uint32_t x=0; x<members.size();x++){
		if (members[x]==uri)
			return true;
		}
	return false;
}


bool GroupList::isParticipant(string uri){
	for(uint32_t x=0; x<users.size();x++){
		if (users[x]->getUri()==uri){
			return true;
		}
	}
	return false;
}

bool GroupList::isParticipant(int ssrc){
	for(uint32_t x=0; x<users.size();x++){
		if (users[x]->getSSRC()==ssrc){
			return true;
		}
	}
	return false;
}

void GroupList::addUser(string uri, IPAddress* to_addr, int RTPport, int RTCPport, Codec *codec,
			int priority,
			int seqNo,
			int status){
	
	//create new User Element
	GroupListUserElement* user = new GroupListUserElement(uri, to_addr, RTPport, RTCPport, codec, priority, seqNo, status);
	
	//add to the vector users
	users.push_back(user);
}

MRef<GroupListUserElement*> GroupList::getUser(string uri){
	for(uint32_t x=0; x<users.size();x++) {
		if(users[x]->getUri()==uri)
			return users[x];
	}
	return NULL;
}

MRef<GroupListUserElement*> GroupList::getUser(int ssrc){
	for(uint32_t x=0; x<users.size();x++) {
		if(users[x]->getSSRC()==ssrc)
			return users[x];
	}
	return NULL;
}

void GroupList::addUser(string uri, int priority, int status){

	//create new User Element
	MRef<GroupListUserElement*> user = new GroupListUserElement(uri, priority, status);
	
	//add to the vector users
	users.push_back(user);
}

void GroupList::removeUser(string uri){
	
	vector<MRef<GroupListUserElement*> >::iterator iter = users.begin();	
	for(uint32_t x=0; x<users.size();x++) {

		if(users[x]->getUri()==uri)
			users.erase(iter);
		
		iter++;
	}
}

/**
 * a typical GroupList looks like:
 *<grouplist id="1234" owner="user@iptel.org" description="desc" type="0" membership="0" floortime="60" maxparticipants="99">
 *	<members size="2">
 *		<member uri="user@iptel.org"/>
 *		<member uri="user1@iptel.org"/>
 *	</members>
 *	<participants size="2">
 *		<user uri="user@iptel.org" prio="0"/>
 *		<user uri="user1iptel.org" prio="0"/>
 *	</participants>
 *</grouplist>
 */

string GroupList::print(){
	string xml="";
	
	xml += "<grouplist id=\"" + getGroupIdentity() + "\" ";
	xml += "owner=\""+ getGroupOwner() + "\" ";
	xml += "description=\""+ getDescription() + "\" ";
	xml += "type=\""+ itoa(getSessionType()) + "\" ";
	xml += "membership=\""+ itoa(getMembership()) + "\" ";
	xml += "floortime=\""+ itoa(getMaxFloorTime()) + "\" ";
	xml += "maxparticipants=\""+ itoa(getMaxParticipants()) + "\">\n";
	
	if(members.empty()==false){
		xml += "<members size=\"" + itoa(members.size()) + "\">\n";
		
		for(uint32_t k=0; k<members.size();k++) 
			xml+="<member uri=\""+members[k]+"\"/>\n";
		
		xml += "</members>\n";
	}
	
	if(users.empty()==false){
		xml += "<participants size=\"" + itoa(users.size()) + "\">\n";
		
		for(uint32_t k=0; k<users.size();k++) 
			xml+="<user uri=\""+users[k]->getUri()+"\" prio=\"" + itoa(users[k]->getPriority())+"\"/>\n";
		
		xml += "</participants>\n";
	}
	
	xml+="</grouplist>";
	return xml;
}

string GroupList::print_debug(){

	string xml="";
	
	xml += "<grouplist id=\"" + getGroupIdentity() + "\" ";
	xml += "owner=\""+ getGroupOwner() + "\" ";
	xml += "description=\""+ getDescription() + "\" ";
	xml += "type=\""+ itoa(getSessionType()) + "\" ";
	xml += "membership=\""+ itoa(getMembership()) + "\" ";
	xml += "floortime=\""+ itoa(getMaxFloorTime()) + "\" ";
	xml += "maxparticipants=\""+ itoa(getMaxParticipants()) + "\">\n";
	
	if(members.empty()==false){
		xml += "<members size=\"" + itoa(members.size()) + "\">\n";
		
		for(uint32_t k=0; k<members.size();k++) 
			xml+="<member uri=\""+members[k]+"\"/>\n";
		
		xml += "</members>\n";
	}
	
	if(users.empty()==false){
		xml += "<participants size=\"" + itoa(users.size()) + "\">\n";
		
		for(uint32_t k=0; k<users.size();k++) {
			xml+="<user uri=\""+users[k]->getUri()+"\" prio=\"" + itoa(users[k]->getPriority())+"\" ";
			xml+="status=" + itoa(users[k]->getStatus()) + " ";
			xml+="callID="+users[k]->getCallId() + " />\n";
			}
			
		xml += "</participants>\n";
	}
	
	xml+="</grouplist>";
	return xml;

}

