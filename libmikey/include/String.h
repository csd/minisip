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

#ifndef MINISTRING_H
#define MINISTRING_H

#include<config.h>

#ifdef USE_STL
#include<string>
#define String std::string
#else

#include"exception.h"
#include<stdio.h>
#include<stdlib.h>
#include<stdint.h>
#include<string.h>
#include<fstream>

class String{
	public:
		String();
		String(const char *);
		String(const char *, uint32_t);
		String(char);
		String(const String &);
		~String();

		String operator =(String s);
		
		String operator +(String s); 
		String operator +(char c);
		void operator +=(String s);
		void operator +=(char c);
		
		char operator [] (uint32_t pos);

		const char * c_str() const;

		uint32_t size();
		uint32_t length();

		bool operator == (const String s);
		bool operator != (const String s);

		String substr(uint32_t start=0, uint32_t length=npos);
		void    erase(uint32_t start=0, uint32_t length=npos);
		
		uint32_t find(const String s, uint32_t start_pos = 0);

		static const uint32_t npos = 0xFFFFFFFF;

		friend std::ostream &operator <<(std::ostream &, String);
		friend String operator +(const char *, String);

	private:
		char * buffer;
		uint32_t len;
};
#endif
#endif
