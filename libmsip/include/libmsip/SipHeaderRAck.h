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
 * 	SipHeaderRAck.h
 * Author
 * 	Erik Eliasson, eliasson@it.kth.se
 * Purpose
 * 
*/

#ifndef SIPHEADERRAck_H
#define SIPHEADERRAck_H

#include<libmsip/libmsip_config.h>

#include<libmsip/SipHeader.h>

/**
 * @author Erik Eliasson
*/

extern SipHeaderFactoryFuncPtr sipHeaderRAckFactory;

class LIBMSIP_API SipHeaderValueRAck: public SipHeaderValue{

	public:
		
		SipHeaderValueRAck(const string &method, int responseNum, int cseqNum);
		SipHeaderValueRAck(const string &build_from);

		virtual ~SipHeaderValueRAck();
		
                virtual std::string getMemObjectType(){return "SipHeaderRAck";}
		
		/**
		 * returns string representation of the header
		 */
		string getString(); 

		int32_t getResponseNum();

		int32_t getCSeqNum();


		string getMethod();
		void setMethod(const string &method);

	private:
		string method;
		int32_t cseqnum;
		int32_t respnum;
};

#endif
