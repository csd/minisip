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


#ifndef COND_VAR_H
#define COND_VAR_H

#include<libmutil/mtypes.h>

#include<libmutil_config.h>

#include<string>
#include<libmutil/MemObject.h>

class Mutex;

class LIBMUTIL_API CondVar : public MObject{
	public:
		CondVar();
		~CondVar();


		void wait( uint32_t timeout_ms = 0);
		void broadcast();
		
		//void signal(); //Deprecated - use semaphore instead to
		//get this functionality (this method was deprecated
		//because of difficulties of supporting it on Windows)

		std::string getMemObjectType(){return "CondVar";}

	private:
		void * internalStruct;
		Mutex * condvarMutex; //Note: This mutex will not be used when
				      //compiling using the microsoft compiler.

};


#endif

