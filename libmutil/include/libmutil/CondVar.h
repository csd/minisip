/*
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation; either
 *  version 2.1 of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

/* Copyright (C) 2004 
 *
 * Authors: Erik Eliasson <eliasson@it.kth.se>
 *          Johan Bilien <jobi@via.ecp.fr>
*/


#ifndef COND_VAR_H
#define COND_VAR_H

#include <libmutil/libmutil_config.h>

#include<libmutil/mtypes.h>

#include<string>
#include<libmutil/MemObject.h>

class Mutex;

class LIBMUTIL_API CondVar : public MObject{
	public:
		CondVar();
		~CondVar();


		LIBMUTIL_DEPRECATED
		void wait( uint32_t timeout_ms = 0);

		/**
		 * @arg mutex - protects the CondVar, must be locked by the
		 * thread making the call.
		 */
		void wait( Mutex &mutex, uint32_t timeout_ms = 0);
		void broadcast();
		
		//void signal(); //Deprecated - use semaphore instead to
		//get this functionality (this method was deprecated
		//because of difficulties of supporting it on Windows)

		std::string getMemObjectType() const;

	private:
		void * internalStruct;
		Mutex * condvarMutex; //Note: This mutex will not be used when
				      //compiling using the microsoft compiler.

};


#endif

