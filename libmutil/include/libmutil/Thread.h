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

#ifndef EERUNNABLE_H
#define EERUNNABLE_H

#include<string>

using namespace std;

class ThreadException{
public:
	ThreadException(std::string description);
	std::string what();
private:
	std::string desc;
};

class Runnable{
public:
	virtual void run()=0;
};


class Thread{
public:
	Thread(Runnable *runnable);
	static int createThread( void f());
	static int createThread( void* f(void*), void *arg);
private:
	void *handle_ptr;
};

#endif
