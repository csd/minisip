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


#ifndef _SIPMESSAGECONTENT_H
#define _SIPMESSAGECONTENT_H

#include<libmsip/libmsip_config.h>

#include<libmutil/MemObject.h>

/**
 * Interface for all content in the SIP message. A sip message content must
 * be able to provide a string that will be included in the content part of
 * the SIP message and a content type.
 * @author Erik Eliasson
*/
class LIBMSIP_API SipMessageContent : public virtual MObject{
    public:
        
        /**
         * Returns the complete SDP message as a string.
         * @return SDP packet as a string.
         */
        virtual std::string getString() const =0;

        /**
         * Returns the type of the content. For example, a SDP message
         * returns "application/sdp".
         * @return Content type
         */
        virtual std::string getContentType() const =0;
};





#endif
