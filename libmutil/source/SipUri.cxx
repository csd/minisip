/*
  Copyright (C) 2005, 2004 Erik Eliasson, Johan Bilien
  Copyright (C) 2005-2006 Mikael Magnusson
  
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
 *          Mikael Magnusson <mikma@users.sourceforge.net>
*/


/* Name
 * 	SipUri.cxx
 * Author
 * 	Cesc Santasusana, c e s c dot s a n t a A{T g m a i l dot co m; 2005
 * Purpose
 * 
*/

#include<config.h>

#include<libmutil/SipUri.h>
//#include<libmsip/SipException.h>
#include<libmutil/dbg.h>
#include<libmutil/stringutils.h>

#include <stdlib.h>

#include<vector>
#include<locale>

#ifdef DEBUG_OUTPUT
#include<iostream>
#endif

using namespace std;

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
	mdbg("signaling/sip") << "SipUri::fromString = " << buildFrom << endl;
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

			setParameter( paramName, params[ idx ] );
		}
	}
	
	validUri = true;
	
}

void SipUri::setParams(string userName_, string ip_, string type, int32_t port_){
	clear();
	
#ifdef DEBUG_OUTPUT
// 	cerr << "SipUri::setParams " << endl;
#endif	
	parseUserInfo( userName_ );
	if( getUserName() == "" && getIp() != "" ) {
		setUser( getIp() );
		setIp( "" );
		setPort( 0 );
	}
	
	if( getIp() == "" && ip_ != "" ) {
		setIp( ip_ );
	}
	
	if( port_ != 0 ) setPort( port_ );
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
	
	if( UriData[0] == '[' ){
		pos = UriData.find(']');

		if( pos != string::npos ){
			setIp( UriData.substr( 1, pos - 1 ) );

			if( UriData[ pos + 1 ] == ':' ) { //there is port info ...
				UriData.erase( 0, pos + 2);
				setPort( atoi(UriData.c_str()) );
			} else {
				setPort( 0 );
			}
			return;
		}
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
	this->validUri = false;
	this->parameters.clear();
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
	Uri += getRequestUriString();
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

	if( getIp().find(':') != string::npos ){
		// IPv6
		Uri += '[' + getIp() + ']';
	}
	else{
		Uri += getIp();
	}
	
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
	Uri += getUserIpString();
	if( getPort() != 0 ) {
		Uri += ":" + itoa( port );
	}
	
	map<string, string>::const_iterator iter;
	map<string, string>::const_iterator last = parameters.end();

	for( iter = parameters.begin(); iter != last; iter++ ){
		string key = iter->first;
		string val = iter->second;

		if( val.empty() )
			Uri += ';' + key ;
		else
			Uri += ';' + key + '=' + val;
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

const string & SipUri::getDisplayName() const {
	return displayName;
}

void SipUri::setProtocolId(string id){
	protocolId=id;
#ifdef DEBUG_OUTPUT
// 	cerr << "SipUri: protocol id = " << protocolId << endl;
#endif
}

const string & SipUri::getProtocolId() const {
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

const string & SipUri::getUserName() const {
	return userName;
}

void SipUri::setIp(string i){
	if( i[0] == '[' && i[i.length()-1] == ']' )
		this->ip=i.substr( 1, i.length() - 2 );
	else
		this->ip=i;
}

const string & SipUri::getIp() const {
	return ip;
}

void SipUri::setPort(int32_t p){
	this->port=p;
}

int32_t SipUri::getPort() const {
	return port;
}

void SipUri::setUserType(string type){
	setParameter( "user", type );
}

const string & SipUri::getUserType() const {
	return getParameter( "user" );
}

void SipUri::setTransport(string transp){
	setParameter( "transport", transp );
}

const string & SipUri::getTransport() const {
	return getParameter( "transport" );
}

void SipUri::setParameter(const std::string &key, const std::string &val){
	parameters[ key ] = val;
}

bool SipUri::hasParameter(const std::string &key) const{
	std::map<std::string, std::string>::const_iterator iter;
	iter = parameters.find( key );

	return iter != parameters.end();
}

const std::string & SipUri::getParameter(const std::string &key) const{
	static const string empty = "";

	std::map<std::string, std::string>::const_iterator iter;
	iter = parameters.find( key );
	if( iter != parameters.end() )
		return iter->second;
	else
		return empty;
}

void SipUri::removeParameter(const std::string &key){
	parameters.erase( key );
}

int SipUri::operator==(const SipUri &uri) const{
	locale loc("");

	if( getProtocolId() != uri.getProtocolId() ||
	    getUserName() != uri.getUserName() ||
	    strCaseCmp(getIp(), uri.getIp(), loc) ||
	    getPort() != uri.getPort() ){
		return false;
	}

	// RFC 3261 19.1.4
	// A URI omitting any component with a default value will not
        // match a URI explicitly containing that component with its
        // default value.
	const char *keys[] = { "transport", "user", "ttl",
			      "method", "maddr", NULL };

	for( int j = 0; keys[j]; j++ ){
		const char *key = keys[j];
		const string & value = getParameter( key );

		if( strCaseCmp( value, uri.getParameter( key ), loc) ){
			return false;
		}
	}

	map<string,string>::const_iterator i;
	map<string,string>::const_iterator last = parameters.end();

	for( i = parameters.begin(); i != last; i++ ){
		const string &key = i->first;
		const string &value = i->second;

		if( uri.hasParameter( key ) ){
			if( strCaseCmp( value, uri.getParameter( key ), loc ) ){
				return false;
			}
		}
	}

	last = uri.parameters.end();

	for( i = uri.parameters.begin(); i != last; i++ ){
		const string &key = i->first;
		const string &value = i->second;

		if( hasParameter( key ) ){
			if( strCaseCmp( value, getParameter( key ), loc ) ){
				return false;
			}
		}
	}

	return true;
}

bool SipUri::isValid() const { 
	return validUri;
}

void SipUri::makeValid( bool valid ) { 
	validUri = valid; 
}


ostream& operator << (ostream& os, const SipUri& uri){
	return os << uri.getString();
}
