/*
  Copyright (C) 2007 Mikael Magnusson

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

#include <config.h>
#include <libmnetutil/init.h>
#include <udns.h>
#include <libmnetutil/NetworkException.h>

using namespace std;

static unsigned int g_initialized;

LIBMNETUTIL_API void libmnetutilInit()
{
	if( g_initialized++ > 0 )
		return;

	if( dns_init(0) < 0 ) {
		g_initialized--;
		throw Exception( "libmnetutil udns initialization failed" );
	}
}

LIBMNETUTIL_API void libmnetutilUninit()
{
	if( --g_initialized )
		return;
}


