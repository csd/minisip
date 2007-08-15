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


#ifndef RAND_H
#define RAND_H

#include <libmcrypto/config.h>
#include <stddef.h>
#include <libmutil/MemObject.h>
#include <libmcrypto/SipSim.h>

class LIBMCRYPTO_API Rand {
	public:
		Rand();

		/**
		 * Generate cryptographically strong random data
		 * @return true on success, false otherwise
		 */
		static bool randomize(void *buffer, size_t length);
		static bool randomize(void *buffer, size_t length, MRef<SipSim *> sim);
};

#endif
