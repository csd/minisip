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
 * 	GroupListUserElement.cxx
 * Author
 * 	Florian Maurer, florian.maurer@floHweb.ch
 * Purpose
 *		
 * 
*/

#include <config.h>

#include<libminisip/signaling/p2t/GroupListUserElement.h>

GroupListUserElement::GroupListUserElement(string uri, IPAddress* addr, int RTPport, int RTCPport, Codec *codec,
			int priority,
			int seqNo,
			int status,
			int ssrc,
			string callId)/*:MObject()*/{
	this->uri=uri;
	this->to_addr=addr;
	this->RTPport=RTPport;
	this->RTCPport=RTCPport;
	this->codec = codec;
	this->priority=priority;
	this->seqNo=seqNo;
	this->status=status;
	this->ssrc=ssrc;
	this->callId=callId;
	
	this->localStarted=false;
	
}
	
GroupListUserElement::GroupListUserElement(string uri, int priority, int status)/*:MObject("GroupListUserElement")*/ {
	this->uri=uri;
	this->priority=priority;
	this->status=status;
}
	
GroupListUserElement::~GroupListUserElement(){

}
