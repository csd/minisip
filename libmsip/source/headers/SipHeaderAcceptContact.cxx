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
 * 	SipHeaderValueAcceptContact.cxx
 * Author
 * 	Florian Maurer, florian.maurer@floHweb.ch
 * Purpose
 *	The Accept-Contact header field allows the UAC to specify that a UA
 *	should be contacted if it matches some or all of the values of the
 *	header field.  Each value of the Accept-Contact header field contains
 *	a "*" and is parameterized by a set of feature parameters.  Any UA
 *	whose capabilities match the feature set described by the feature
 *	parameters matches the value. The precise behavior depends heavily on
 *	whether the "require" and "explicit" feature parameters are present.
 *	When both of them are present, a proxy will only forward the request
 *	to contacts which have explicitly indicated that they support the
 *	desired feature set. Any others are discarded.
 *	http://www.ietf.org/internet-drafts/draft-ietf-sip-callerprefs-10.txt
 *	
 *	At the moment the Accept-Contact header is used to distinguish in 
 *	a INVITE message between a regular call and a P2T call.
 *	
 *	Note: Only one feature tag is supported. Use multiple
 *      Accept-Contact headers for supporting more feature tags.
 *	
 * Version
 *	2004-05-28
*/

#include<config.h>

#include<libmsip/SipHeaderAcceptContact.h>

using namespace std;

MRef<SipHeaderValue *> acceptContactFactory(const string &build_from){
	return new SipHeaderValueAcceptContact(build_from);
}

SipHeaderFactoryFuncPtr sipHeaderAcceptContactFactory=acceptContactFactory;


const string sipHeaderValueValueTypeStr = "Accept-Contact";

/**
 * Constructor. Parses the header from a string
 * @param build_from	the header is parsed from this string.
 *			Expected form:
 *			  Accept-Contact: *;featuretag;require;explicit
 *			  'request' and 'explicit' are optional
 * FIXME ... this parsing can be improved ... 
 */
SipHeaderValueAcceptContact::SipHeaderValueAcceptContact(string build_from):SipHeaderValue(SIP_HEADER_TYPE_ACCEPTCONTACT,sipHeaderValueValueTypeStr) {
	
	//Initialisation
	featuretag="NOT DEFINED";
	set_require = false;
	set_explicit = false;
	unsigned i=0;
	//cerr<<"SipHeaderValueAcceptContact"+build_from<<endl;
	while ( build_from[i]!=';' && i<build_from.length() ) {
		i++;
	}
	
	//Parse featuretag
	i++;
	string value="";
	while ( build_from[i]!=';' && i<build_from.length() ) {
		value+=build_from[i];
		i++;
	}
	featuretag = value;

	//Parse request, explicit
	i++;
	value="";
	while ( build_from[i]!=';' && i<build_from.length() ) {
		value+=build_from[i];
		i++;
	}	
	
	if(value=="request")
		set_require = true;
	else if (value=="explicit")
		set_explicit = true;
	
	value="";
	while ( build_from[i]!=';' && i<build_from.length() ) {
		i++;
		value+=build_from[i];
	}
	
	if(value=="require")
		set_require = true;
	else if (value=="explicit")
		set_explicit = true;
}


/**
 * Constructor
 * @param featuretag   the feature that should be supported
 * @param set_require  set the require flag
 * @param set_explicit set the explicit flag
 */

SipHeaderValueAcceptContact::SipHeaderValueAcceptContact(string ft, 
		bool sr, bool se)
		:SipHeaderValue(SIP_HEADER_TYPE_ACCEPTCONTACT,sipHeaderValueValueTypeStr){
	this->featuretag = ft;
	this->set_require = sr;
	this->set_explicit = se;
}

/**
 * Deconstructor
 */
SipHeaderValueAcceptContact::~SipHeaderValueAcceptContact(){

}

/**
 * returns the string representation of
 * the Accept-Contact header field
 */
string SipHeaderValueAcceptContact::getString() const{
	string answer;
	answer = /*"Accept-Contact: */ "*;" + featuretag;
	
	if(set_require)
		answer = answer + ";require";
	
	if(set_explicit)
		answer = answer + ";explicit";
		
	return answer;
} 


