/*
  Copyright (C) 2006 Erik Eliasson
  
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

/* Name
 * 	SipHeaderRSeq.h
 * Author
 * 	Erik Eliasson, eliasson@it.kth.se
 * Purpose
 * 
*/

#ifndef SIPHEADERRSeq_H
#define SIPHEADERRSeq_H

#include<libmsip/libmsip_config.h>

#include<libmsip/SipHeader.h>

/**
 * @author Erik Eliasson
*/

extern SipHeaderFactoryFuncPtr sipHeaderRSeqFactory;

class LIBMSIP_API SipHeaderValueRSeq: public SipHeaderValue{
	public:
		SipHeaderValueRSeq(uint32_t rnum);
		SipHeaderValueRSeq(const std::string &build_from);

		virtual ~SipHeaderValueRSeq();

                virtual std::string getMemObjectType() const {return "SipHeaderRSeq";}
		
		virtual std::string getString() const; 

		uint32_t getRSeq() const;
		void setRSeq( uint32_t rseq );
		

	private:
		uint32_t seq;
};

#endif
