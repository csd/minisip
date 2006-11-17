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

#ifndef _IString_H
#define _IString_H

#include <libmutil/libmutil_config.h>

#include<libmutil/MemObject.h>

#include<string>

#include<libmutil/mtypes.h>

/**
 * Describes a string as a pointer to the start of the string characters
 * and the number of them. Note that the string is not a C-string that
 * ends with a NULL-character.
 * Purpose: This is what "writev" takes as argument.
 */
struct strptr{
	void *str_start;
	uint32_t n;
};

/**
 * A StringBuffer is a buffer of characters that must not be modified.
 * A StringAtom can be used by one or more IString. 
 * The buffer passed to the StringAtom object must not be modified
 * or freed - it is managed by the StringAtom object and will be
 * released when the StringAtom is deleted.
 * @author Erik Eliasson, eliasson@imit.kth.se
 */
class StringAtom : public MObject{
public:
	/**
	 * Creates a StringAtom object giving it a character buffer
	 * that will be managed by the StringAtom object.
	 * 
	 * @param buf	Pointer to the character content of the StringAtom
	 * 		object. The buffer must not be modified or deleted
	 * 		- it is managed by the StringAtom object and will
	 * 		be released when the StirngAtom is deleted.
	 * @param n	Number of charactes is the StringAtom buffer.
	 */
	StringAtom(char *buf, int n);

	StringAtom(const StringAtom &);

	/**
	 * Avoid using this constructor if possible
	 */
	StringAtom(std::string);
	
	/**
	 * Deletes the internal character buffer that was passed to 
	 * the constructor.
	 */
	~StringAtom();

	/**
	 * Implemented only for debugging purposes (see MObject/MRef for
	 * more information on this method).
	 */
	std::string getMemObjectType() const;

	/**
	 * @return	The internal character buffer. A user is not
	 * 		allowed to modify the content of the buffer.
	 */
	char* getBuf() const;

	/**
	 * @return 	The size of the buffer pointer returned by getBuf.
	 */
	int getLength() const;

private:
	char *buf;
	int n;
};

/**
 * IString (Indirect String) is a string, but it does not contain a
 * private copy of the string characters. Instead it has a StringAtom
 * object and the string is a part of it. Several IString objects
 * can use the same StringAtom.
 *
 * The following ASCII drawing shows how three IString objects, fn (first
 * name), ln (last name) and fulln (full name) uses the same StringAtom
 * object, sa. Since the StringAtom class (the IString as well) is an
 * MObject, it will be freed when noone is referencing it any longer (i.e.
 * all IString objects have been freed).
 *
 *         fn     ln
 *           |    |
 *           +--. +------.
 *           v  v v      v
 *      sa ["Erik Eliasson"]
 *           ^           ^
 *           +-----------'
 *           |
 *           fulln
 *      
 * The IString class has been implemented to help when implementing parsing
 * of text based protocols (i.e. libmsip). Using StringAtom/IString the
 * characters from an incoming packet does not have to be copied several
 * times when constructing a response. The getStringPointer gives a
 * representation of the string in a format that is possible to send to the
 * "writev" function.
 * 
 * @author Erik Eliasson, eliasson@imit.kth.se
 */
class IString : public MObject{
public:

	/**
	 * Makes the string cover the entire atom (the content of the atom
	 * is the same as the content of the string).
	 */
	IString(MRef<StringAtom*> a);
	
	/**
	 * Creates a IString object that represents a string
	 * that is part (or all) of a StringAtom object.
	 *
	 * @param a	StringAtom buffer that contains
	 * 		the characters of the string.
	 * @param startIndex	Index in the StringAtom, a, where
	 * 			this IString starts.
	 * @param length	Length of this IString.
	 */
	IString(MRef<StringAtom*> a, int startIndex, int length);

	/**
	 * Copy constructor. Makes this string identical to "a".
	 */
	IString(const IString &s);

	/**
	 * If this is the last object using the StringAtom containing
	 * the string characters, then the StringAtom object will
	 * be freed as well (this functionality is provided by the
	 * MObject/MRef - see MemObject.h).
	 */
	~IString();
	
	/**
	 * Implemented only for debugging purposes (see MObject/MRef for
	 * more information on this method).
	 */
	std::string getMemObjectType() const;

	/**
	 * Returns a C++ style string (std::string). This method
	 * should mainly be used for debugging purposes. If this
	 * method is used, perhaps std::string should be used where
	 * the IString is used since most benefits of the IString
	 * class is lost.
	 */
	std::string cpp_str();

	/**
	 * Returns a string representation that can be used together
	 * with the writev function.
	 */
	struct strptr getStringPointer() const;

	int getLength(){return n;}
	char* getBuffer(){return atom->getBuf()+start;}

	/**
	 * Returns a string that has removed ' ', '\n' and '\t' from
	 * the front and end of the string.
	 */
	MRef<IString*> trim();

	MRef<IString*> substr(int i);

	MRef<IString*> substr(int i, int n);
	
	
private:
	MRef<StringAtom*> atom;
	int start;
	int n;
};

#endif
