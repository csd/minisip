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
 * 	split_in_lines.cxx
 * Author
 * 	Erik Eliasson, eliasson@it.kth.se, 2003
 * Purpose
 * 	Takes a string as argument and splits it at any new line.
*/

#include<libmutil/split_in_lines.h>
#include<libmutil/trim.h>
#include<string>
#include<iostream>

using namespace std;

std::vector<string> split(string s, bool do_trim, char delim, bool includeEmpty){
	std::vector<string> ret;
	unsigned i=0;
	do{
		string line="";
		while (!(i>(s.length()-1)) && s[i]!=delim){	// 1.1
			line+=s[i++];
		}
		if (do_trim)
			line=trim(line);
		if (line.length()>0 || includeEmpty){
			ret.push_back(line); 		// 1.2
		}
							// 1.3
//		while ((!(i>=s.length()-1)) && s[i]==delim)
			i++;
	}while (!(i>=s.length()));
	
	return ret;
}

/*
 * 1. until end of input stream
 *   1.1 read line until new line
 *   1.2 add line
 *   1.3 move past new line chars
 *
 * */
std::vector<string> split_in_lines(string s, bool do_trim){
	std::vector<string> ret;
	unsigned i=0;
	do{
		string line="";
		while (!(i>s.length()-1) && s[i]!='\r' && s[i]!='\n'){	// 1.1
			line+=s[i++];
		}
		if (do_trim)
			line=trim(line);
		if (line.length()>0){
			ret.push_back(line);			// 1.2
		}
							// 1.3
		while ((!(i>=s.length()-1)) && (s[i]=='\r' || s[i]=='\n'))
			i++;
	}while (!(i>=s.length()-1));
	
	return ret;
}

