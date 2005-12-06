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
 * 	trim.h
 * Author
 * 	Erik Eliasson, eliasson@it.kth.se, 2003
 *	Cesc Santasusana c e s c DOT sa n ta [AT] g ma i l dot c o m;  2005
 * Purpose
 * 	Removes whitespace in the start and end of a String.
*/

#include<config.h>

#include<libmutil/trim.h>
#include<libmutil/mtypes.h>
#include<ctype.h>

#include<string>

using namespace std;

LIBMUTIL_API string trim(string line){
	size_t spacesFront = 0, spacesEnd = 0;
	int32_t idx;

	idx = 0;
	while( idx < (int)line.size() && isspace(line[idx]) ) {
		spacesFront++;
		idx++;
	}
	
	idx = (int)line.size() - 1 ;
	while( idx >= 0 && isspace(line[idx]) ) {
		spacesEnd++;
		idx--;
	}
	line = line.substr( spacesFront, line.size() - ( spacesFront + spacesEnd ) );
	return line;
}

