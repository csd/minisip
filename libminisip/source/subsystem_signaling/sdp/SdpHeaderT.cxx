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
 * 	SdpHeaderT.cxx
 * Author
 * 	Erik Eliasson, eliasson@it.kth.se
 * Purpose
 * 
*/

#include<config.h>

#include<libminisip/signaling/sdp/SdpHeaderT.h>
#include<libmutil/stringutils.h>

#ifdef DEBUG_OUTPUT
#include<iostream>
#endif

#include<stdlib.h> //atoi()

using namespace std;

SdpHeaderT::SdpHeaderT(string buildFrom):SdpHeader(SDP_HEADER_TYPE_T, 5){
	size_t len=buildFrom.length();
	if (buildFrom.substr(0,2)!="t="){
#ifdef DEBUG_OUTPUT
		std::cerr << "ERROR: Origin sdp header is not starting with <o=>"<< std::endl;
#endif
	}
	
	size_t i=2;
	while ( buildFrom[i]==' ' && i<len )
		i++;

	string startstr="";
	while ( buildFrom[i]!=' ' && i<len )
		startstr+=buildFrom[i++];

	while ( buildFrom[i]==' ' && i<len )
		i++;

	string stopstr="";
	while ( buildFrom[i]!=' ' && i<len )
		stopstr+=buildFrom[i++];

	start_time = atoi(startstr.c_str());
	stop_time  = atoi(stopstr.c_str());
}

SdpHeaderT::SdpHeaderT(int32_t start, int32_t stop):SdpHeader(SDP_HEADER_TYPE_T, 5){
	this->start_time=start;
	this->stop_time=stop;
}

SdpHeaderT::~SdpHeaderT(){

}

int32_t SdpHeaderT::getStartTime(){
	return start_time;
}

void SdpHeaderT::setStartTime(int32_t start){
	this->start_time=start;
}

int32_t SdpHeaderT::getStopTime(){
	return stop_time;
}
void SdpHeaderT::setStopTime(int32_t stop){
	this->stop_time=stop;
}

string SdpHeaderT::getString(){
	return "t="+itoa(start_time)+" "+itoa(stop_time);
}


