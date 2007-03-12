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

#include<libminisip/signaling/p2t/P2T.h>

//Performance measurements
const string P2T::PERFORMANCE_FILE = "/home/erik/florian_performance/result";

//Group Member Lists
//Status:
const int P2T::STATUS_IDLE = 0;
const int P2T::STATUS_CONNECTED = 1;
const int P2T::STATUS_GRANT = 2;
const int P2T::STATUS_COLLISION = 3;
const int P2T::STATUS_RELEASED = 4;
const int P2T::STATUS_TALKING = 5;
const int P2T::STATUS_REQUESTING=6;
const int P2T::STATUS_WAITACCEPT = 100;
const int P2T::STATUS_NOTAVAILABLE = 7;

//Session type:
const int P2T::INSTANT_PERSONAL_TALK = 0;
const int P2T::ADHOC_INSTANT_GROUP = 1;
const int P2T::INSTANT_GROUP = 2;
const int P2T::CHAT_GROUP = 3;

//Membership:
const int P2T::MEMBERSHIP_OPEN=0;
const int P2T::MEMBERSHIP_RESTRICTED=1;

//Timer values
const float P2T::timerRESEND=1000.0;
const int P2T::timerREVOKE=2000;
const int P2T::timerGRANT = 300;   //300ms
const int P2T::timerIDLE = 300;
const int P2T::timerTAKEN = 300; //300ms
const int P2T::timerRelTIMEOUT = 30000; //30s
const int P2T::timerGetTIMEOUT = 30000;
const int P2T::timerGetFloorTERMINATE = 60000; //60s
const int P2T::timerGrantFloorTERMINATE = 60000;
const int P2T::timerRelFloorTERMINATE = 60000;
const int P2T::timerIdleFloorTERMINATE = 60000;
const int P2T::timerTakenFloorTERMINATE= 60000;


//RTCP APP packets
//name:
const string P2T::APP_NAME = "P2T_";
//subtype:
const int P2T::APP_REQUEST = 0;
const int P2T::APP_GRANT = 1;
const int P2T::APP_TAKEN = 2;
const int P2T::APP_DENY = 3;
const int P2T::APP_RELEASE = 4;
const int P2T::APP_IDLE = 5;
const int P2T::APP_REVOKE = 6;

 
string P2T::getStatus(int status){
	
	string ret = "";
	
	switch(status) {
		case STATUS_IDLE:	
			ret = "Idle";
			break;
	
		case STATUS_CONNECTED:	
			ret = "Connected";
			break;
	
		case STATUS_GRANT:
			ret = "Grant";
			break;
	
		case STATUS_COLLISION:
			ret= "Collision";
			break;
	
		case STATUS_RELEASED:
			ret= "Released";
			break;
	
		case STATUS_TALKING:
			ret= "Talking";
			break;
	
		case STATUS_WAITACCEPT:
			ret="Wait for accept";
			break;
	
		case STATUS_NOTAVAILABLE:
			ret="Not available";
			  break;
	
		default:
			ret= "not defined";
	}
	return ret;
}

string P2T::getSessionType(int type){
	
	string ret = "";
	
	switch(type) {
		case INSTANT_PERSONAL_TALK:	
			ret = "Instant Personal Talk";
			break;
	
		case ADHOC_INSTANT_GROUP:	
			ret = "Ad-Hoc Instant Group Talk";
			break;
	
		case INSTANT_GROUP:
			ret = "Instant Group Talk";
			break;
	
		case CHAT_GROUP:
			ret= "Chat Group Talk";
			break;
		default:
			ret= "not defined";
	}
	return ret;
}

string P2T::getMembership(int membership){
	
	string ret = "";
	
	switch(membership) {
		case MEMBERSHIP_OPEN:	
			ret = "Open";
			break;
	
		case MEMBERSHIP_RESTRICTED:	
			ret = "Restricted";
			break;

		default:
			ret= "not defined";
	}
	return ret;
}
