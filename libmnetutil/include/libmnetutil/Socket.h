/*
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Library General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

/* Copyright (C) 2004 
 *
 * Authors: Erik Eliasson <eliasson@it.kth.se>
 *          Johan Bilien <jobi@via.ecp.fr>
*/

#ifndef SOCKET_H
#define SOCKET_H

//#include<config.h>

#include<sys/types.h>
#include<unistd.h>

#define SOCKET_TYPE_STREAM	0x10
#define SOCKET_TYPE_TCP		0x11
#define SOCKET_TYPE_TLS		0x12

#define SOCKET_TYPE_UDP		0x20

class Socket{
	public:
		virtual int32_t getFd();
		int32_t getType();

		void close();
	
	protected:
		int32_t type;
		int32_t fd;
};

#endif
