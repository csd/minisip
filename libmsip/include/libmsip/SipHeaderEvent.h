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

/* Name
 * 	SipHeaderEvent.h
 * Author
 * 	Erik Eliasson, eliasson@it.kth.se
 * Purpose
 * 
*/

#ifndef SIPHEADEREVENT_H
#define SIPHEADEREVENT_H

#include<libmsip/SipHeader.h>

/**
 * @author Erik Eliasson
*/


class SipHeaderEvent: public SipHeader{
	public:
		SipHeaderEvent();
		SipHeaderEvent(const string &build_from);

		virtual ~SipHeaderEvent();

                virtual std::string getMemObjectType(){return "SipHeaderEvent";}
		
		/**
		 * returns string representation of the header
		 */
		string getString(); 

		/**
		 * @return The IP address of the contact header.
		 */
		string getEvent();
		
		void setEvent(const string &event);

	private:
		string event;
};

#endif
