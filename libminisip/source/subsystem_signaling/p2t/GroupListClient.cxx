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

#include <stdio.h>
#include <errno.h>
#include<libminisip/signaling/p2t/GroupListClient.h>
#include <libmutil/dbg.h>
#include <libmutil/stringutils.h>
#include <libmutil/merror.h>
#include <libmutil/XMLParser.h>

#include<ctype.h>

#ifdef WIN32
/* Headerfiles for Windows */
#include <winsock2.h>
#include <io.h>

#else
/* Headerfiles for Unix/Linux */
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#endif


GroupListClient::GroupListClient(){
}

GroupListClient::~GroupListClient(){
}

MRef<GroupList*> GroupListClient::getGroupList(string GroupId, char *srv_addr, int port){

	string xml;
	
	//get content from GroupListServer
	string content = connectServer("GET " + GroupId + "\n", srv_addr, port);

	//remove first line with xml info
	if(starts_with(content,"<?xml")){
		uint32_t i=0;
		for(/*i*/;content[i]!='\n';i++){}
		i++;
		
		for(/*i*/;i<content.size();i++)
			xml+=content[i];
		
	}
	else
		xml=content;
	
#ifdef DEBUG_OUTPUT
	mdbg << "Received GroupList XML:"<< end;
	mdbg << xml<<end;
#endif		
	
	MRef<GroupList*> grpList = new GroupList(xml);
	return grpList;
}

MRef<GroupList*> GroupListClient::downloadGroupList(string file, char *srv_addr, int port){
	string xml;

	//get content from GroupListServer
	string cmd = "GET " + file + " HTTP/1.0\n\n";
	string content = connectServer(cmd, srv_addr, port);

	
	//remove first line with xml info
	if(starts_with(content,"<?xml")){
		uint32_t i=0;
		for(/*i*/;content[i]!='\n';i++){}
		i++;
		
		for(/*i*/;i<content.size();i++)
			xml+=content[i];
		
	}
	//otherwise no XML-content received
	else if(starts_with(content,"ERROR")){
		MRef<GroupList*>grpList=new GroupList();
		grpList->setDescription(content);
		return grpList;
	}
	else{
		MRef<GroupList*>grpList=new GroupList();
		grpList->setDescription("ERROR: Got non xml response!");
		return grpList;
	}
		
#ifdef DEBUG_OUTPUT
	mdbg << "Received GroupList XML:"<< end;
	mdbg << xml<<end;
#endif	
		
	MRef<GroupList*> grpList = new GroupList(xml);
	return grpList;
}


string GroupListClient::connectServer(string command, char *srv_addr, int port) {
	int sock;
   	struct sockaddr_in server;
    	struct hostent *host_info;
//    	unsigned long addr;
    	int count;
	char buffer[8192];
	

	
	

#ifdef _WIN32  
    	/* init TCP for Windows ("winsock") */
    	short wVersionRequested;
    	WSADATA wsaData;
    	wVersionRequested = MAKEWORD (1, 1);
    	if (WSAStartup (wVersionRequested, &wsaData) != 0) {
        	//merror("Failed to init windows Group List Server sockets");
		return "ERROR failed to init Windows Group List Server socket";
    }
#endif

    	/* create socket */
    	sock = socket( PF_INET, SOCK_STREAM, 0);
    	if (sock < 0) {
		//merror("Failed to create Group List Server socket");
		return "ERROR failed to create Group List Server socket";
	}

        /* Create socketadress of Server
     	* it is type, IP-adress and portnumber */
    	memset( &server, 0, sizeof (server));
    	
	/* convert the Servername to a IP-Adress */
    	host_info = gethostbyname( srv_addr);
    	
	if (NULL == host_info) {
        	//merror("unknown Group List server");
		return "ERROR unknown Group List Server";
        }
    	memcpy( (char *)&server.sin_addr, host_info->h_addr, host_info->h_length);

    	server.sin_family = AF_INET;
    	server.sin_port = htons( port);

    	/* connect to the server */
    	if ( connect( sock, (struct sockaddr*)&server, sizeof( server)) < 0) {
        	//merror("can't connect to GroupList Server");
		return "ERROR can't connect to GroupList Server";
        }

    	/* create and send the http GET request */
	//sprintf( buffer, "GET /getGroupMemberList HTTP/1.0\nHost: %s\n\n", host_info->h_name);
    	//sprintf( buffer, "GET /getGroupMemberList?id=%s HTTP/1.0\n", GroupId);

	memcpy(buffer, &command[0], command.size());
	send( sock, buffer, command.size(), 0);
	//send( sock, buffer, strlen( buffer), 0);
	
	
    	/* get the answer from server */
    	string data;
	do {
        	count = recv( sock, buffer, sizeof(buffer), 0);
        	data += string(buffer, count);
    	}
    	while (count > 0);
	
	// split data in header and content
	// content is separated from the header
	// with '\r' '\n' '\r' '\n' characters
	string header="";
	string content="";
	uint32_t i=0;
	
	bool n_received=false;
	bool r_received=false;

	//header
	for (/*i*/;!(n_received && r_received && data[i]=='\n');i++){
	
		if(data[i]=='\n')
			n_received=true;	
		
		else if(data[i]=='\r')
			r_received=true;	
		
		else {
			n_received=false;
			r_received=false;
		}
		header+=data[i];
	}
	
	//content
	i++;
	for (/*i*/; i<data.size(); i++){
		content += data[i];
	}
	//close(sock)
	return content;
}


bool GroupListClient::starts_with(string line, string part){
	if (part.length() > line.length())
		return false;
	for (uint32_t i=0; i< part.length(); i++)
		if ( toupper(part[i]) != toupper(line[i]) )
			return false;
	return true;
}
