/*
 Copyright (C) 2004-2006 the Minisip Team
 
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
 Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
 */

/*
 * Authors: Zachary T Welch <zach-minisip@splitstring.com>
 *          Werner Dittmann <Werner.Dittmann@t-online.de>
 */

#include <libmcrypto/openssl/init.h>
#include <libmutil/Thread.h>

#include <vector>

#include <openssl/crypto.h>
#include <openssl/evp.h>
#ifdef OPENSSL_SYS_WIN32
#include <windows.h>
#endif

#include <config.h>

using namespace std;

void libmcryptoOpensslInit() {
// 	MRef<OpenSSLThreadGuard *> instance = OpenSSLThreadGuard::getInstance();
	// instance will be saved by singleton
	OpenSSL_add_all_algorithms();
}

void libmcryptoOpensslUninit(){
	EVP_cleanup();
}

OpenSSLThreadGuard::OpenSSLThreadGuard() : CryptoThreadGuard()
{
	CRYPTO_set_id_callback(&openssl_thread_id);
	CRYPTO_set_locking_callback(&openssl_locker);
}
OpenSSLThreadGuard::~OpenSSLThreadGuard() 
{ 
	CRYPTO_set_locking_callback(NULL);
	CRYPTO_set_id_callback(NULL);
}

int OpenSSLThreadGuard::numLocks() { 
	return CRYPTO_num_locks(); 
}

unsigned long OpenSSLThreadGuard::openssl_thread_id() 
{
	return Thread::getCurrent().asLongInt();
}

void OpenSSLThreadGuard::openssl_locker(int mode, int i, 
		const char *file, int line) 
{
	MRef<OpenSSLThreadGuard *> inst = OpenSSLThreadGuard::getInstance();
	inst->setLock(i, mode & CRYPTO_LOCK);
}

