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


#include<config.h>

#include<libminisip/signaling/sip/PresenceMessageContent.h>

using namespace std;

MRef<SipMessageContent*> presenceSipMessageContentFactory(const string &buf, const string &){
	        return new PresenceMessageContent(buf);
}


PresenceMessageContent::PresenceMessageContent(string from, string to, string os, string osd) : 
		fromUri(from),
		toUri(to),
		onlineStatus(os), 
		onlineStatusDesc(osd)
{
	string status;
	if (os=="online"){
		status="open";
	}else{
		status="inuse";
	}
	document = "<?xml version=\"1.0\"?>\n"
		"<!DOCTYPE presence\n"
		"PUBLIC \"-//IETF//DTD RFCxxxx XPIDF 1.0//EN\" \"xpidf.dtd\">\n"
		"<presence>\n"
		"  <presentity uri=\"" +toUri+";method=SUBSCRIBE\" />\n"
		"  <atom id=\"1000\">\n"
		"    <address uri=\""+fromUri+"\">\n"
		"      <status status=\""+status+"\" />\n" 			//"open"|("closed")|"inuse"
		"      <msnsubstatus substatus=\""+onlineStatus+"\" />\n"
		"    </address>\n"
		"  </atom>\n"
		"</presence>\n";
}

PresenceMessageContent::PresenceMessageContent(const string &buildFrom){
	document = buildFrom;
}

string PresenceMessageContent::getString() const{
	return document;
}


