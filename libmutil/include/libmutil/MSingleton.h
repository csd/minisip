/*
 Copyright (C) 2006 the Minisip Team
 
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
 Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/

/* Copyright (C) 2006
 *
 * Authors: Mikael Magnusson <mikma@users.sourceforge.net>
 */

#ifndef MSINGLETON_H
#define MSINGLETON_H

#include <libmutil/MemObject.h>

template<class T>
class MSingleton{
	public:
		static MRef<T*> getInstance(){
			if( !instance ){
				instance = new T();
			}
			return instance;
		}

	protected:
		MSingleton(){}
		virtual ~MSingleton(){}

	private:
		MSingleton(const MSingleton &);
		MSingleton &operator=(const MSingleton &);

		static MRef<T *> instance;
};

template<class T>
MRef<T *> MSingleton<T>::instance;

#endif	// MSINGLETON_H
