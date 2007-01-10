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

#include<stdlib.h>
#include<gcrypt.h>
#include<gnutls/gnutls.h>
#include<errno.h>
#include<pthread.h>
#include<iostream>

GCRY_THREAD_OPTION_PTHREAD_IMPL;

using namespace std;

void libmcryptoGnutlsInit()
{
	/* The order matters.
	 */
	gcry_control(GCRYCTL_SET_THREAD_CBS, &gcry_threads_pthread);
	gnutls_global_init();
}

void libmcryptoGnutlsUninit()
{

	gnutls_global_deinit();
}
