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
 * 	SipHeader.h
 * Author
 * 	Erik Eliasson, eliasson@it.kth.se
 * Purpose
 * 
*/

#ifndef SIPHEADER_H
#define SIPHEADER_H


//#include<config.h>

#include<libmutil/MemObject.h>

#include<sys/types.h>
/**
 * @author Erik Eliasson
*/

#define SIP_HEADER_TYPE_ACCEPT			0
#define SIP_HEADER_TYPE_AUTHORIZATION		1
#define SIP_HEADER_TYPE_CALLID			2
#define SIP_HEADER_TYPE_CONTACT			3
#define SIP_HEADER_TYPE_CONTENTLENGTH		4
#define SIP_HEADER_TYPE_CONTENTTYPE		5
#define SIP_HEADER_TYPE_CSEQ			6
#define SIP_HEADER_TYPE_EVENT			7
#define SIP_HEADER_TYPE_EXPIRES			8
#define SIP_HEADER_TYPE_FROM			9
#define SIP_HEADER_TYPE_MAXFORWARDS		10
#define SIP_HEADER_TYPE_PROXYAUTHENTICATE	11
#define SIP_HEADER_TYPE_PROXYAUTHORIZATION	12
#define SIP_HEADER_TYPE_RECORDROUTE		13
#define SIP_HEADER_TYPE_ROUTE			14
#define SIP_HEADER_TYPE_SUBJECT			15
#define SIP_HEADER_TYPE_TO			16
#define SIP_HEADER_TYPE_USERAGENT		17
#define SIP_HEADER_TYPE_VIA			18
#define SIP_HEADER_TYPE_UNSUPPORTED             19
#define SIP_HEADER_TYPE_ACCEPTCONTACT		20

using namespace std;

class SipHeader : public MObject{
	public:
                SipHeader(int type);
		virtual ~SipHeader();

		virtual string getString()=0;

                virtual std::string getMemObjectType(){return "SipHeader";}

		int32_t getType(){return type;};

		static MRef<SipHeader *> parseHeader(const string &buildFrom);

	private:
		int32_t type;
};

#endif
