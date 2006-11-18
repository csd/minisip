/*
  Copyright (C) 2006 Mikael Magnusson
  
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
 * Authors: Mikael Magnusson <mikma@users.sourceforge.net>
*/


#ifndef UUID_H
#define UUID_H

#include <libmcrypto/config.h>
#include <libmutil/MemObject.h>

/**
 * Universally Unique IDentifier (UUID) based on 
 * sample implementation in RFC 4122
 */
class LIBMCRYPTO_API Uuid: public MObject {
	public:
		~Uuid();

		/**
		 * Create random UUID
		 */
		static Uuid* create();

		int operator=(const Uuid &u);
		std::string toString();

	private:
		Uuid(const void *priv);
		void *m_priv;
};

#endif
