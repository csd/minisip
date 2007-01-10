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

#ifndef MLIBMCRYPTO_OPENSSL_INIT_H
#define MLIBMCRYPTO_OPENSSL_INIT_H

#include <libmcrypto/init.h>
#include <libmutil/MSingleton.h>

void libmcryptoOpensslInit();
void libmcryptoOpensslUninit();

class LIBMCRYPTO_API OpenSSLThreadGuard : public CryptoThreadGuard, 
			public MSingleton<OpenSSLThreadGuard> 
{
public:
	OpenSSLThreadGuard();
	virtual ~OpenSSLThreadGuard();

	static void initialize();

protected:
	virtual int numLocks();

private:
	static void openssl_locker(int, int, const char *, int);
	static unsigned long openssl_thread_id(void);
};

#endif // MLIBMCRYPTO_OPENSSL_INIT_H
