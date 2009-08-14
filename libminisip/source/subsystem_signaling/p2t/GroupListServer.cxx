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

#include <config.h>

#include<libminisip/signaling/p2t/GroupListServer.h>
#include <ctype.h>
#include<libmutil/dbg.h>
#include<libmnetutil/ServerSocket.h>
#include<libminisip/media/MediaHandler.h>

GroupListServer::GroupListServer(MRef<SipSoftPhoneConfiguration*>config,  int32_t localPort){
	

	
	//initialize the socket
	srv_socket = ServerSocket::create(localPort);
	//srv_socket->listen(config->inherited.externalContactIP, localPort, 20);
	
	//set local variables
	this->port = srv_socket->getPort();
	this->ip = config->inherited->externalContactIP;
	
	//enter port in SipSoftPhoneConfiguration
	phoneconfig=config;
	phoneconfig->p2tGroupListServerPort=this->port;	
}

GroupListServer::~GroupListServer(){
}




void GroupListServer::run(){
	
#ifdef DEBUG_OUTPUT
	mdbg << "GroupListServer:: Accepting connections at " << ip<<":"<<(int)port<<end;
	setThreadName("GroupListServer::run");
#endif
	
	MRef<StreamSocket*> socket;
	char buffer[16384];
//	int avail;
	int32_t nread=1;
	string received;
	
	string groupId;
	string uri;
	
	while(running){
		received="";
		groupId="";
		uri = "";
		
		//wait for incoming connection
		socket = srv_socket->accept();	
		
		//read input
		nread = socket->read(buffer, sizeof(buffer));
		received=(string)buffer;
	
		
		/**
		 * Received GET
		 */
		if(starts_with(received, "GET")) {
			
			//parse groupId
			for(uint32_t x=4;x<received.size();x++){
				if(received[x]!='\n' && received[x]!='\r')
					groupId+=received[x];
				else
					break;
			}
#ifdef DEBUG_OUTPUT	
			mdbg<<"GroupListServer:: Received: "<<end;
			mdbg<<"Command: GET"<<end;
			mdbg<<"GroupID: *" << groupId<<"*"<<end;
#endif
			//find correct GroupList and send it
			for(uint32_t k=0;k<grpLists.size();k++){
				
				if(grpLists[k]->getGroupIdentity()==groupId){
					socket->write("GET\r\n\r\n"+grpLists[k]->print());
					break;
				}
			}
			
			//no GroupList found
			//socket->write("GET\r\n\r\nNot Found\r\n"+groupId);

		}
		
		
		/**
		 * Received ADD.
		 * To add a user to a Group Member List.
		 * Not implemented.
		 */
		else if(starts_with(received, "ADD")) {
			socket->write("ADD\r\n\r\nNot implemented");
		}
		
		//close Socket
		//		delete socket;

	} 
	
}

void GroupListServer::start(){
	running=true;
	Thread t(this);
}

void GroupListServer::stop(){
	running = false;
	
	//remove port in SipSoftPhoneConfiguration
	phoneconfig->p2tGroupListServerPort=0;
	
	//empty vector
	grpLists.clear();
	
	
#ifdef DEBUG_OUTPUT
	mdbg << "GroupListServer:: "<< ip<<":"<<(int)port<<" stopped!"<<end;
#endif
}


void GroupListServer::addGroupList(MRef<GroupList*> grpList){
	grpLists.push_back(grpList);
}

MRef<GroupList*> GroupListServer::getGroupList(string groupId){
	
	for (uint32_t k=0; k<grpLists.size();k++){
		if(grpLists[k]->getGroupIdentity()==groupId)
			return grpLists[k];
	}	

	return NULL;
}

bool GroupListServer::starts_with(string line, string part){
	if (part.length() > line.length())
		return false;
	for (uint32_t i=0; i< part.length(); i++)
		if ( toupper(part[i]) != toupper(line[i]) )
			return false;
	return true;
}

