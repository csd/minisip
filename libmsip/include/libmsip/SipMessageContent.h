/*
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Library General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */
 
/* Copyright (C) 2004
 *
 * Authors: Erik Eliasson <eliasson@it.kth.se>
 *          Johan Bilien <jobi@via.ecp.fr>
*/

#ifndef _SIPMESSAGECONTENT_H
#define _SIPMESSAGECONTENT_H

#include<libmutil/MemObject.h>

/**
 * Interface for all content in the SIP message. A sip message content must
 * be able to provide a string that will be included in the content part of
 * the SIP message and a content type.
 * @author Erik Eliasson
*/
class SipMessageContent : public virtual MObject{
    public:
        
        /**
         * Returns the complete SDP message as a string.
         * @return SDP packet as a string.
         */
        virtual std::string getString()=0;

        /**
         * Returns the type of the content. For example, a SDP message
         * returns "application/sdp".
         * @return Content type
         */
        virtual std::string getContentType()=0;
};

#endif
