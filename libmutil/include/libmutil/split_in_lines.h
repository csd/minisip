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
 * 	split_in_lines.h
 * Author
 * 	Erik Eliasson, eliasson@it.kth.se, 2003
 * Purpose
 * 	Takes a String as argument and splits it at any new line.
*/

#ifndef SPLIT_IN_LINES
#define SPLIT_IN_LINES

#include<vector>

using namespace std;

std::vector<string> split(string s, bool do_trim=true, char delim='\n', bool includeEmpty=false);

std::vector<string> split_in_lines(string s, bool do_trim = true);


#endif
