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
 * 	SdpHeaderI.cxx
 * Author
 * 	Erik Eliasson, eliasson@it.kth.se
 * Purpose
 * 
*/

#include<config.h>


#include<libminisip/signaling/sdp/SdpHeaderI.h>
#include<libmutil/stringutils.h>
#include<libmutil/massert.h>

using namespace std;

SdpHeaderI::SdpHeaderI(string buildFrom):SdpHeader(SDP_HEADER_TYPE_I, 9){
	massert(buildFrom.substr(0,2)=="i=");
	session_information = trim(buildFrom.substr(2, buildFrom.length()-2));
}

SdpHeaderI::~SdpHeaderI(){

}

string SdpHeaderI::getSessionInformation(){
	return session_information;
}
void SdpHeaderI::setSessionInformation(string s){
	this->session_information = s;
}

string SdpHeaderI::getString(){
	return "i="+session_information;
}
