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

#include<libminisip/libminisip_config.h>

#include<string>
#include<iostream>

class LIBMINISIP_API ConfMember {
	public:
	
		ConfMember(std::string the_uri, std::string the_callid ) {
			std::cerr << "creating conf member" << std::endl;
			uri = the_uri ;
			callid = the_callid;
		}
		
		std::string uri;
		std::string callid;
};


#endif
