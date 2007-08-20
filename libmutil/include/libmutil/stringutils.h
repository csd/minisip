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
 * 	stringutils.h
 * Author
 * 	Erik Eliasson, eliasson@it.kth.se, 2003
*/

#ifndef SPLIT_IN_LINES
#define SPLIT_IN_LINES

#include<vector>
#include<string>

#include <libmutil/libmutil_config.h>
#include<locale>
#include<libmutil/mtypes.h>


/**
 * If the string does not already start and end with the " character, then
 * it is appended first and last to the string and the result is returned.
 */
LIBMUTIL_API std::string quote(const std::string &str);

/**
 * If a string starts and ends with the " character, then they are removed
 * and the retult is returned.
 */
LIBMUTIL_API std::string unquote(std::string str);


/**
 * Creates a string representing an integer number
 */
LIBMUTIL_API std::string itoa(int64_t i);

/**
 * Splits a string into multiple parts.
 *
 * @return	If s is an empty string the function will return an empty
 * 		vector (no matter if includeEmpty is true or false)
 */
LIBMUTIL_API std::vector<std::string> split(const std::string &s, bool do_trim=true, char delim='\n', bool includeEmpty=false);

/**
 * Splits a string on new line character ('\n').
 *
 * @return 	Empty lines are not included in the output. If do_trim is
 * 		true then lines containing only whitespace will not be
 * 		included in the output.
 * 		An empty input string returns an empty vector.
 */
LIBMUTIL_API std::vector<std::string> splitLines(const std::string &s, bool do_trim = true);

LIBMUTIL_API std::string upCase(const std::string &s);

LIBMUTIL_API int upCase(char c);

LIBMUTIL_API int strCaseCmp(const char *s1, const char* s2);

LIBMUTIL_API int strNCaseCmp(const char *s1, const char* s2, int n);

/**
 * @return true if the parameter is a white space (' ', '\n' or '\t')
 */
LIBMUTIL_API bool isWS(char c);

/**
 * Removes whitespace from beginning and end of Strings.
 * 	@param s	String from which to remove whitespace (or rather copy and remove whitespace from)
 * 	@author Erik Eliasson
*/
LIBMUTIL_API std::string trim(const std::string &s);

/**
 * Converts an array of raw data to a hex string representation.
 */
LIBMUTIL_API std::string binToHex( const unsigned char * data, int length );

/**
 * Available instantiations: string
 */
template <class charT, class traits, class Alloc>
LIBMUTIL_API int strCaseCmp( const std::basic_string<charT, traits, Alloc>& s1,
			     const std::basic_string<charT, traits, Alloc>& s2,
			     const std::locale& loc );

/**
 * Tests if "haystack" ends with "needle".
 */
LIBMUTIL_API bool stringEndsWith(const std::string & haystack, const std::string & needle);

#endif
