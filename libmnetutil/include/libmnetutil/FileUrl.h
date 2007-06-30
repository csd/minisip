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

#ifndef _FILEURL_H_
#define _FILEURL_H_

#include<libmnetutil/libmnetutil_config.h>

#include <string>
#include <vector>

#define FILEURL_TYPE_UNKNOWN 	0
#define FILEURL_TYPE_UNIX 	1
#define FILEURL_TYPE_WINDOWS 	2

/**
 * Represents and parses file URLs.
 *
 * The class correctly handles Windows and UNIX addresses, with one exception: The class
 * incorrectly considers paths string with additional slashes ("/") as valid. The
 * incorrect URI "file:////applib/products/a%2Db/abc%5F9/4148.920a/media/start.swf"
 * is one exampel of this, as the URI considered valid by the class.
 *
 * When it says that this class handles "Windows and UNIX addresses" it means that the
 * class converts the URI to paths appropriate for each operating system. For Windows
 * URLs this means that all forward-slashes ("/") are converted to back-slashes ("\").
 *
 * The type attribute must be set to FILEURL_TYPE_WINDOWS in order for the function to
 * properly handle Windows file paths. Otherwise UNIX paths are assumed/returned.
 *
 * @author	Mikael Svensson
 */
class LIBMNETUTIL_API FileUrl {
	public:
		/**
		 * Constructor when we already have a URL but it is unclear for which system
		 * (e.g. Windows or Unix) the file adheres.
		 */
		FileUrl(const std::string url);

		/**
		 * Constructor when we already have a URL and we know it points to a file in
		 * on a Windows system or Unix system.
		 */
		FileUrl(const std::string url, const int32_t type);

		/**
		 * Generic constructor when we want to create a new URL from scratch.
		 */
		FileUrl();

		/**
		 * Restore URL parts to their default values
		 */
		void clear();

		/**
		 * Tests whether or not the currenct FileUrl instance represents a valid file URL.
		 *
		 * A URL is considered invalid if:
		 *  - it doesn't start with "file://".
		 *  - it doesn't have a "host part"
		 *  - it doesn't have a "path part"
		 */
		bool isValid() const;

		/**
		 * Parse URL
		 */
		void setUrl(const std::string url);
		void setUrl(const std::string url, const int32_t type);

		/**
		 * Get string representation of URL, e.g. the actual URL.
		 */
		std::string getString() const;

		/**
		 * Prints all information about the URL. Probably only useful when debugging.
		 */
		void printDebug();

		/* Getters and setters */
		std::string getHost() const;
		void setHost(const std::string host);

		/**
		 * Returns the path part of the URL. The returned value conforms to the
		 * path syntax of the operating system specified by the \c type property.
		 *
		 * Thus, if \c type = \c FILEURL_TYPE_WINDOWS then the function may return something like this:
		 * <tt>C:\Program Files\Music\funky_tune.mp3</tt>
		 */
		std::string getPath() const;
		void setPath(const std::string path);

		int32_t getType() const;
		void setType(const int32_t type);
	private:
		/**
		 * The type property is used to differentiate between URLs designed for
		 * different operating systems.
		 *
		 * This allows for automatic conversion of file paths that are not well-suited
		 * for simple "slash separation" (each part of the file path should be
		 * separated using the "/" character as specified by RFC 1738).
		 *
		 * Well-suited paths are e.g. UNIX and Windows paths.Not so well-suited paths
		 * are e.g. VMS paths (although this class currently does not support VMS paths).
		 *
		 * @note	Windows file URLs are interpreted as noted on http://blogs.msdn.com/ie/archive/2006/12/06/file-uris-in-windows.aspx
		 */
		int32_t 			type;

		std::string			host;
		std::string 			path;

		/**
		 * Tests whether or not a character is an "unreserved character".
		 *
		 * Unreserved Characters according to RFC 3986 (section 2.3):
		 *
		 * Characters that are allowed in a URI but do not have a reserved
		 * purpose are called unreserved.  These include uppercase and lowercase
		 * letters, decimal digits, hyphen, period, underscore, and tilde.
		 */
		bool isUnreservedChar(char in) const;

		/**
		 * Tests whether or not a character is a "reserved character".
		 *
		 * Reserved Charachters according to RFC 3986 (section 2.2):
		 *
		 * "URIs include components and subcomponents that are delimited by
		 * characters in the 'reserved' set.  These characters are called
		 * 'reserved' because they may (or may not) be defined as delimiters by
		 * the generic syntax, by each scheme-specific syntax, or by the
		 * implementation-specific syntax of a URI's dereferencing algorithm."
		 */
		bool isReservedChar(char in) const;

		/**
		 * Percent-encode a character.
		 *
		 * Converts a space to "%20" and so on. Note that the function converts the input character
		 * regardsless of whether or not a conversion is actually necessary (if an "a" is sent
		 * to the function it will be converted to "%61" even though it is within the US-ACII set).
		 */
		std::string encodeChar(const char in) const;

		/**
		 * Decode an percent-encoded character.
		 *
		 * Assumes that the input string is three characters long, that the first
		 * character is the "%" sign and that the last two charachters are hexadecimal digits.
		 */
		char decodeChar(const std::string in) const;

		/**
		 * Converts hexadecimal (or decial) digit into the corresponding integer value.
		 */
		int32_t charToNum(const char in) const;

		/**
		 * Parses string and converts all its percent-encoded characters to single characters.
		 */
		std::string percentDecode(const std::string & in) const;

		/**
		 * Percent-encodes string according to rules stated in section 2.1 of RFC 4516.
		 */
		std::string percentEncode(const std::string & in) const;
		std::string percentEncode(const std::string & in, bool escapeComma, bool escapeQuestionmark = true) const;

		bool validUrl;
};
#endif
