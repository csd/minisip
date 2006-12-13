/*
  Copyright (C) 2005, 2004 Erik Eliasson, Johan Bilien
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
 * Authors: Erik Eliasson <eliasson@it.kth.se>
 *          Johan Bilien <jobi@via.ecp.fr>
 *          Mikael Magnusson <mikma@users.sourceforge.net>
*/


#ifndef OAKLEY_DH_H
#define OAKLEY_DH_H

#include<libmcrypto/config.h>

#define DH_GROUP_OAKLEY5 0
#define DH_GROUP_OAKLEY1 1
#define DH_GROUP_OAKLEY2 2

class LIBMCRYPTO_API OakleyDH {
	public:
		OakleyDH();
		OakleyDH( int group );
		~OakleyDH();

		bool setGroup( int group );
		int group() const;

		int computeSecret(const uint8_t *peerKey,
				  uint32_t peerKeyLenght,
				  uint8_t *secret,
				  uint32_t secretLength) const;
		
		/** Length of public key in bytes  */
		uint32_t publicKeyLength() const;
		uint32_t getPublicKey( byte_t *buf, uint32_t buflen) const;

		/** Length of secret in bytes  */
		uint32_t secretLength() const;
		
	protected:
		bool generateKey();

	private:
		int groupValue;
		void * priv;
};

#endif
