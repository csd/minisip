//
// C++ Interface: ConfMember
//
// Description: 
//
//
// Author: Max Loubser <loubser@kth.se>, (C) 2005
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef _CONFMEMBER_H
#define _CONFMEMBER_H

#include<config.h>

class ConfMember {
	public:
	
		ConfMember(string the_uri, string the_callid ) {
			cerr << "creating conf member" << endl;
			uri = the_uri ;
			callid = the_callid;
		}
		
		string uri;
		string callid;
};


#endif
