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

#ifndef STREAMSOCKET_H
#define STREAMSOCKET_H

#include"Socket.h"

#include<string>
using namespace std;

class StreamSocket : public Socket{
	public:
		virtual ~StreamSocket(){}
		virtual int32_t write(string)=0;
		virtual int32_t write(void *buf, int32_t count)=0;
		virtual int32_t read(void *buf, int32_t cound)=0;

		// Buffer of the received data;
		string received;
};

#endif
