/*
  Copyright (C) 2006 Zachary T Welch

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
 * Authors: Zachary T Welch <zach-minisip@splitstring.com>
 */

#include <libmcrypto/init.h>
#ifdef HAVE_OPENSSL
#include <libmcrypto/openssl/init.h>
#endif
#ifdef HAVE_GNUTLS
#include <libmcrypto/gnutls/init.h>
#endif
#include <libmutil/Mutex.h>

#include <config.h>

void libmcryptoInit()
{
#if 0
#ifdef HAVE_OPENSSL
	OpensslThreadGuard::initialize();
#endif
#ifdef HAVE_GNUTLS
	GnutlsThreadGuard::initialize();
#endif
#endif
}

// ====================================================================

std::vector<Mutex*> CryptoThreadGuard::guards;

CryptoThreadGuard::CryptoThreadGuard()
{
	this->changeGuards(1);
}
CryptoThreadGuard::~CryptoThreadGuard()
{
	this->changeGuards(0);
}

void CryptoThreadGuard::changeGuards(bool onDuty)
{
	int num_locks = this->numLocks();
	this->guards.reserve(num_locks);
	for (int i = 0; i < num_locks; i++) {
		if (onDuty)
			this->guards[i] = new Mutex();
		else
			delete this->guards[i];
	}
}

void CryptoThreadGuard::setLock(int i, bool isSet)
{
	if (isSet) {
		this->guards[i]->lock();
	} else {
		this->guards[i]->unlock();
	}
}
