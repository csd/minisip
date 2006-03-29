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
 * 	SipUri.cxx
 * Author
 * 	Cesc Santasusana, c e s c dot s a n t a A{T g m a i l dot co m; 2005
 * Purpose
 * 
*/

#include<config.h>

#include<libmsip/SipUri.h>
#include<libmsip/SipException.h>
#include<libmutil/itoa.h>
#include<libmutil/dbg.h>
#include<libmutil/split_in_lines.h>
#include<libmutil/trim.h>

#include<vector>

#ifdef DEBUG_OUTPUT
#include<iostream>
#endif

SipUri::SipUri(string buildFrom){
	setUri( buildFrom );
}

SipUri::~SipUri(){

}

void SipUri::setUri( string buildFrom ) {
	size_t pos;
	string UriData;
	char paramDelimiter = '\0';
	
	clear();
	
#ifdef DEBUG_OUTPUT
	cerr << "SipUri::fromString = " << buildFrom << endl;
#endif

	//look for the full name ... 	
	pos = buildFrom.find( '<' );
	if( pos != string::npos ) {
		size_t pos2, pos3;
		pos2 = buildFrom.find( '>' );
		if( pos2 == string::npos ) {
#ifdef DEBUG_OUTPUT
			cerr << "SipUri::constructor - bogus Uri ... " << buildFrom << endl;
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

	//separate the params from the Uri ... 
	if( (pos = buildFrom.find( ';' )) != string::npos ) {
		UriData = buildFrom.substr( 0, pos ); 
		buildFrom.erase( 0, pos );
		paramDelimiter = ';';
	} else if( (pos = buildFrom.find( '?' )) != string::npos ) {
		UriData = buildFrom.substr( 0, pos ); 
		buildFrom.erase( 0, pos );
		paramDelimiter = '?';
	} else {
		UriData = buildFrom;
	}
	
	//parse the Uri info related to user (protocol, userName, ip, port)
	parseUserInfo( UriData );

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
// 				cerr << "SipUri:: param not understood ... ignore: " << paramName << endl;
		#endif
			}
		}
	}
	
	validUri = true;
	
}

void SipUri::setParams(string userName, string ip, string type, int32_t port){
	clear();
	
#ifdef DEBUG_OUTPUT
// 	cerr << "SipUri::setParams " << endl;
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


void SipUri::parseUserInfo( string UriData ) {
	//Lets piece the Uri (without params first ) ... in UriData string
	size_t pos;
#ifdef DEBUG_OUTPUT
// 	cerr << "SipUri::parseUserInfo - " << UriData << endl;
#endif
	//first identify the protocol ...
	if( UriData.substr(0,4) == "sip:" ) {
		setProtocolId( "sip" );
		UriData.erase( 0, 4 );
	} else 	if( UriData.substr(0,4) == "tel:" ) {
		setProtocolId( "tel" );
		UriData.erase( 0, 4 );
	} else 	if( UriData.substr(0,5) == "sips:" ) {
		setProtocolId( "sips" );
		UriData.erase( 0, 5 );
	} 
	
	//try to get the username ...
	pos = UriData.find( '@' );
	if( pos != string::npos ) { //there is a username ...
		userName = UriData.substr( 0, pos );
		UriData.erase( 0, pos + 1 );
	} else { //no user info ...
		userName = "";
	}
	
	//now, we get the host/ip ...
	pos = UriData.find( ':' );
	if( pos != string::npos ) { //there is port info ...
		setIp( UriData.substr( 0, pos ) );
		UriData.erase( 0, pos + 1);
		setPort( atoi(UriData.c_str()) );
	} else {
		setIp( UriData );
		UriData.erase( 0, pos );
		setPort( 0 );
	}
}

void SipUri::clear( ) {
	this->displayName = "";
	this->protocolId = "sip";
	this->userName = "";
	this->ip = "";
	this->port = 0;
	this->userType = "";
	this->transport = "";
	this->validUri = false;
}

string SipUri::getString() const {
	string Uri = "";
	
	if( !isValid() ) {
#ifdef DEBUG_OUTPUT
		cerr << "SipUri::getString - invalid Uri!" << endl;
#endif
		return "";
	}
	
	if( getDisplayName() != "" ) {
		Uri += "\"" + getDisplayName() + "\" ";
	}
	Uri += "<";
	if( getProtocolId() != "" )
		 Uri += getProtocolId() + ":";
	if( getUserName() != "" ) Uri += getUserName() + "@";
	Uri += getIp();
	if( getPort() != 0 ) {
		Uri += ":" + itoa( port );
	}
	if( getTransport() != "" ) Uri += ";transport=" + getTransport();
	if( getUserType() != "" ) Uri += ";user=" + getUserType();
	Uri += ">";
	
#ifdef DEBUG_OUTPUT
// 	cerr << "SipUri::getString() - " << Uri << endl << endl;
#endif
	return Uri;
}

string SipUri::getUserIpString() const {
	string Uri = "";
	
	if( !isValid() ) {
#ifdef DEBUG_OUTPUT
		cerr << "SipUri::getUserIpString - invalid Uri!" << endl;
#endif
		return "";
	}
	
	if( getUserName() != "" ) Uri += getUserName() + "@";
	Uri += getIp();
	
#ifdef DEBUG_OUTPUT
// 	cerr << "SipUri::getUserIpString() - " << Uri << endl;
#endif
	return Uri;
}

string SipUri::getRequestUriString() const {
	string Uri = "";
	
	if( !isValid() ) {
#ifdef DEBUG_OUTPUT
		cerr << "SipUri::getString - invalid Uri!" << endl;
#endif
		return "";
	}
	
	if( getProtocolId() != "" )
		 Uri += getProtocolId() + ":";
	if( getUserName() != "" ) Uri += getUserName() + "@";
	Uri += getIp();
	if( getPort() != 0 ) {
		Uri += ":" + itoa( port );
	}
	
#ifdef DEBUG_OUTPUT
// 	cerr << "SipUri::getRequestUristring() - " << Uri << endl;
#endif
	return Uri;
}

void SipUri::setDisplayName(string dispName) {
	displayName = dispName;
#ifdef DEBUG_OUTPUT
// 	cerr << "SipUri: display name = ###" << displayName << "###" << endl;
#endif
}

string SipUri::getDisplayName() const {
	return displayName;
}

void SipUri::setProtocolId(string id){
	protocolId=id;
#ifdef DEBUG_OUTPUT
// 	cerr << "SipUri: protocol id = " << protocolId << endl;
#endif
}

string SipUri::getProtocolId() const {
	return protocolId;
}

//scan the given name ... just in case someone is misusing this function,
// (it should use setUri() ).
void SipUri::setUser(string name){
		this->userName = name;
#ifdef DEBUG_OUTPUT
// 	cerr << "SipUri: user name = " << userName << endl;
#endif
}

string SipUri::getUserName() const {
	return userName;
}

void SipUri::setIp(string ip){
	this->ip=ip;
#ifdef DEBUG_OUTPUT
// 	cerr << "SipUri: ip = " << this->ip << endl;
#endif
}

string SipUri::getIp() const {
	return ip;
}

void SipUri::setPort(int32_t port){
	this->port=port;
#ifdef DEBUG_OUTPUT
// 	cerr << "SipUri: port = " << itoa( this->port ) << endl;
#endif
}

int32_t SipUri::getPort() const {
	return port;
}

void SipUri::setUserType(string type){
	this->userType=type;
#ifdef DEBUG_OUTPUT
// 	cerr << "SipUri: user type = " << this->userType << endl;
#endif
}

string SipUri::getUserType() const {
	return userType;
}

void SipUri::setTransport(string transp){
	transport = transp;
#ifdef DEBUG_OUTPUT
// 	cerr << "SipUri: transport = " << this->transport << endl;
#endif
}

string SipUri::getTransport() const {
	return transport;
}

