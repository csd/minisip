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
 * 	SipResponse.h
 * Author
 * 	Erik Eliasson, eliasson@it.kth.se
 * Purpose
 * 
*/


#ifndef SIPRESPONSE_H
#define SIPRESPONSE_H

#include<libmsip/libmsip_config.h>

#include<libmsip/SipMessage.h>
#include<libmsip/SipRequest.h>

/**
 * 
 * @author Erik Eliasson, eliasson@it.kth.se
 * @version 0.01
 */
class LIBMSIP_API SipResponse : public SipMessage{

	public:
		static const std::string type;

		SipResponse(int32_t status, std::string status_desc, MRef<SipRequest*> inv);


		virtual std::string getMemObjectType() const {return "SipResponse";}
		
		/**
		 * Parses response packet from string representation.
		 * @param respstr string representation of response packet.
		 */
		SipResponse(std::string &respstr);

		/**
		 * @returns Status code of this response.
		 */
		int32_t getStatusCode();

		/**
		 * @resutns Returns status description of this response.
		 */
		std::string getStatusDesc();

		std::string getString() const;

		virtual const std::string& getType(){return type;}

		/**
		 * Insert "Unupported" header in a 420 (Bad Extension)
		 * response containing unsupported extensions.
		 * @param unsupported list of unsupported extensions.
		 */
		void addUnsupported(const std::list<std::string> &unsupported);

		/**
		 * @return the request supplied when creating this response
		 */
		MRef<SipRequest*> getRequest() const;

	private:
		int32_t status_code;
		std::string status_desc;

		std::string tag;
		MRef<SipRequest*> request;
};


#endif
