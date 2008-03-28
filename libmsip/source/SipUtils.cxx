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



#include<ctype.h>

#include<config.h>
#include<libmsip/SipUtils.h>

#include<iostream>

using namespace std;

bool SipUtils::startsWith(std::string line, std::string part){
	if (part.length() > line.length())
		return false;
	for (uint32_t i=0; i< part.length(); i++){
		if ( toupper(part[i]) != toupper(line[i]) )
			return false;
	}
	return true;
}

int SipUtils::findEndOfHeader(const string &buf, int &startIndex){
	return findEndOfHeader(buf.c_str(), (unsigned)buf.size(), startIndex);
}

/**
 * @startIndex will be adjusted to the start of the header (i.e.
 * 	incremented past any whitespace).
 */
int SipUtils::findEndOfHeader(const char *buf, unsigned bufSize,  int &startIndex){
	int endi=bufSize;
	unsigned i;
	int parserState=0;  	// Parser states:
				//	0: last char seen was part of
				//         header
				//      1: last char seen was new line char
				//         and before that char in header
				//      2: last char seen was return and 
				//         before that char in header
				//      3: last chars was return and
				//         new line
				//      4: last chars was new line and
				//         return

	i=startIndex;
	while (i<bufSize && (buf[i]=='\r' || buf[i]=='\n' || buf[i]==' '))
		i++;
	startIndex = i;
	
	for( ; i<bufSize; i++){
		char c= buf[i];
		if (parserState==0){
			if (c=='\n')
				parserState=1;
			else if (c=='\r')
				parserState=2;
		}else if (parserState==1){
			if (c=='\r')
				parserState=4;
			else if (c=='\t' || c==' ')
				parserState=0;
			else
				return i-2;
		}else if (parserState==2){
			if (c=='\n')
				parserState=3;
			else 
				parserState=0;
		}else{ //parserState is three or four
			if (c=='\t' || c==' ')
				parserState=0;
			else 
				return i-3;
		
		}
/* alt. impl. that might be easier to understand (left here for documentation purposes)
		switch (parserState){
			case 0:
				switch (c){
					case '\r':
						parserState=2;
						break;
					case '\n':
						parserState=1;
						break;
				}
				break;
				
			case 1:
				switch (c){
					case '\r':
						parserState=4;
						break;
					case ' ':
					case '\t':
						parserState=0;
						break;
					default:
						endi=i-2;
						return endi;
						break;
				}
				break;
				
			case 2:
				switch (c){
					case '\n':
						parserState=3;
						break;
					default:
						parserState=0;
						break;
				}
				break;

			case 3:
			case 4:
				switch (c){
					case ' ':
					case '\t':
						parserState=0;
						break;
					default:
						endi=i-3;
						return endi;
				}
			
		}
*/		
	}
	
		//endi is index of one beyond last char
	endi--; //last index can not be beyond last char
	
	while (endi>startIndex && (buf[endi]=='\n' || buf[endi]=='\r') )
		endi--;

	return endi;
}

