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


/* Name
 * 	SipURI.cxx
 * Author
 * 	Cesc Santasusana, c e s c dot s a n t a A{T g m a i l dot co m; 2005
 * Purpose
 * 
*/

#include<config.h>

#include<libmsip/SipURI.h>
#include<libmsip/SipException.h>
#include<libmutil/itoa.h>
#include<libmutil/dbg.h>
#include<libmutil/split_in_lines.h>
#include<libmutil/trim.h>

#include<vector>

#ifdef DEBUG_OUTPUT
#include<iostream>
#endif

SipURI::SipURI(string buildFrom){
	setUri( buildFrom );
}

void SipURI::setUri( string buildFrom ) {
	size_t pos;
	string uriData;
	char paramDelimiter = '\0';
	
	clear();
	
#ifdef DEBUG_OUTPUT
// 	cerr << "SipURI::fromString = " << buildFrom << endl;
#endif

	//look for the full name ... 	
	pos = buildFrom.find( '<' );
	if( pos != string::npos ) {
		size_t pos2, pos3;
		pos2 = buildFrom.find( '>' );
		if( pos2 == string::npos ) {
#ifdef DEBUG_OUTPUT
			cerr << "SipURI::constructor - bogus uri ... " << buildFrom << endl;
#endif
			return;
		}
		//process the full name ... 
		string nameTmp;
		nameTmp = trim( buildFrom.substr( 0, pos ) );
		pos3 = nameTmp.find( '"' );
		while( pos3 != string::npos ) {
			nameTmp.erase( pos3, 1 );
			pos3 = nameTmp.find( '"' );
		}
		setDisplayName( nameTmp );
		buildFrom.erase( 0, pos + 1 ); //remove the full name ...
		//remove the leftovers (ZZZ)... XXX<YYY>ZZZ 
		pos2 = buildFrom.find( '>' );
		buildFrom.erase( pos2 );
	}

	//now we process the stuff that was between the < and > chars ... 

	//separate the params from the uri ... 
	if( (pos = buildFrom.find( ';' )) != string::npos ) {
		uriData = buildFrom.substr( 0, pos ); 
		buildFrom.erase( 0, pos );
		paramDelimiter = ';';
	} else if( (pos = buildFrom.find( '?' )) != string::npos ) {
		uriData = buildFrom.substr( 0, pos ); 
		buildFrom.erase( 0, pos );
		paramDelimiter = '?';
	} else {
		uriData = buildFrom;
	}
	
	//parse the uri info related to user (protocol, userName, ip, port)
	parseUserInfo( uriData );

	//now parse the parameters ... 
	if( paramDelimiter != '\0' ) {
		std::vector<string> params;
		string paramName;
		unsigned int idx; 
		params = split( buildFrom, true, paramDelimiter );
		for( idx = 0; idx < params.size(); idx++ ) {
			pos = params[idx].find( '=' );
			if( pos != string::npos ) {
				paramName = params[idx].substr( 0, pos );
				params[idx].erase( 0, pos + 1 );
			} else {
				paramName = params[idx];
				params[idx] = "";
			}
			
			if( paramName == "transport" ) {
				setTransport( params[idx] );
			} else if( paramName == "user" ) {
				setUserType( params[idx] );
			} else {
		#ifdef DEBUG_OUTPUT
// 				cerr << "SipURI:: param not understood ... ignore: " << paramName << endl;
		#endif
			}
		}
	}
	
	validUri = true;
	
}

void SipURI::setParams(string userName, string ip, string type, int32_t port){
	clear();
	
#ifdef DEBUG_OUTPUT
// 	cerr << "SipURI::setParams " << endl;
#endif	
	parseUserInfo( userName );
	if( getUserName() == "" && getIp() != "" ) {
		setUser( getIp() );
		setIp( "" );
		setPort( 0 );
	}
	
	if( getIp() == "" && ip != "" ) {
		setIp( ip );
	}
	
	if( port != 0 ) setPort( port );
	if( type != "" ) setUserType( type );
	validUri = true;
}


void SipURI::parseUserInfo( string uriData ) {
	//Lets piece the uri (without params first ) ... in uriData string
	size_t pos;
#ifdef DEBUG_OUTPUT
// 	cerr << "SipUri::parseUserInfo - " << uriData << endl;
#endif
	//first identify the protocol ...
	if( uriData.substr(0,4) == "sip:" ) {
		setProtocolId( "sip" );
		uriData.erase( 0, 4 );
	} else 	if( uriData.substr(0,4) == "tel:" ) {
		setProtocolId( "tel" );
		uriData.erase( 0, 4 );
	} else 	if( uriData.substr(0,5) == "sips:" ) {
		setProtocolId( "sips" );
		uriData.erase( 0, 5 );
	} 
	
	//try to get the username ...
	pos = uriData.find( '@' );
	if( pos != string::npos ) { //there is a username ...
		userName = uriData.substr( 0, pos );
		uriData.erase( 0, pos + 1 );
	} else { //no user info ...
		userName = "";
	}
	
	//now, we get the host/ip ...
	pos = uriData.find( ':' );
	if( pos != string::npos ) { //there is port info ...
		setIp( uriData.substr( 0, pos ) );
		uriData.erase( 0, pos + 1);
		setPort( atoi(uriData.c_str()) );
	} else {
		setIp( uriData );
		uriData.erase( 0, pos );
		setPort( 0 );
	}
}

void SipURI::clear( ) {
	this->displayName = "";
	this->protocolId = "sip";
	this->userName = "";
	this->ip = "";
	this->port = 0;
	this->userType = "";
	this->transport = "";
	this->validUri = false;
}

string SipURI::getString(){
	string uri = "";
	
	if( !isValid() ) {
#ifdef DEBUG_OUTPUT
		cerr << "SipURI::getString - invalid URI!" << endl;
#endif
		return "";
	}
	
	if( getDisplayName() != "" ) {
		uri += "\"" + getDisplayName() + "\" ";
	}
	uri += "<";
	if( getProtocolId() != "" )
		 uri += getProtocolId() + ":";
	if( getUserName() != "" ) uri += getUserName() + "@";
	uri += getIp();
	if( getPort() != 0 ) {
		uri += ":" + itoa( port );
	}
	if( getTransport() != "" ) uri += ";transport=" + getTransport();
	if( getUserType() != "" ) uri += ";user=" + getUserType();
	uri += ">";
	
#ifdef DEBUG_OUTPUT
// 	cerr << "SipUri::getString() - " << uri << endl << endl;
#endif
	return uri;
}

string SipURI::getUserIpString(){
	string uri = "";
	
	if( !isValid() ) {
#ifdef DEBUG_OUTPUT
		cerr << "SipURI::getUserIpString - invalid URI!" << endl;
#endif
		return "";
	}
	
	if( getUserName() != "" ) uri += getUserName() + "@";
	uri += getIp();
	
#ifdef DEBUG_OUTPUT
// 	cerr << "SipUri::getUserIpString() - " << uri << endl;
#endif
	return uri;
}

string SipURI::getRequestUriString() {
	string uri = "";
	
	if( !isValid() ) {
#ifdef DEBUG_OUTPUT
		cerr << "SipURI::getString - invalid URI!" << endl;
#endif
		return "";
	}
	
	if( getProtocolId() != "" )
		 uri += getProtocolId() + ":";
	if( getUserName() != "" ) uri += getUserName() + "@";
	uri += getIp();
	if( getPort() != 0 ) {
		uri += ":" + itoa( port );
	}
	
#ifdef DEBUG_OUTPUT
// 	cerr << "SipUri::getRequestUristring() - " << uri << endl;
#endif
	return uri;
}

void SipURI::setDisplayName(string dispName) {
	displayName = dispName;
#ifdef DEBUG_OUTPUT
// 	cerr << "SipURI: display name = ###" << displayName << "###" << endl;
#endif
}

string SipURI::getDisplayName(){
	return displayName;
}

void SipURI::setProtocolId(string id){
	protocolId=id;
#ifdef DEBUG_OUTPUT
// 	cerr << "SipURI: protocol id = " << protocolId << endl;
#endif
}

string SipURI::getProtocolId(){
	return protocolId;
}

//scan the given name ... just in case someone is misusing this function,
// (it should use setUri() ).
void SipURI::setUser(string name){
		this->userName = name;
#ifdef DEBUG_OUTPUT
// 	cerr << "SipURI: user name = " << userName << endl;
#endif
}

string SipURI::getUserName(){
	return userName;
}

void SipURI::setIp(string ip){
	this->ip=ip;
#ifdef DEBUG_OUTPUT
// 	cerr << "SipURI: ip = " << this->ip << endl;
#endif
}

string SipURI::getIp(){
	return ip;
}

void SipURI::setPort(int32_t port){
	this->port=port;
#ifdef DEBUG_OUTPUT
// 	cerr << "SipURI: port = " << itoa( this->port ) << endl;
#endif
}

int32_t SipURI::getPort(){
	return port;
}

void SipURI::setUserType(string type){
	this->userType=type;
#ifdef DEBUG_OUTPUT
// 	cerr << "SipURI: user type = " << this->userType << endl;
#endif
}

string SipURI::getUserType(){
	return userType;
}

void SipURI::setTransport(string transp){
	transport = transp;
#ifdef DEBUG_OUTPUT
// 	cerr << "SipURI: transport = " << this->transport << endl;
#endif
}

string SipURI::getTransport(){
	return transport;
}

