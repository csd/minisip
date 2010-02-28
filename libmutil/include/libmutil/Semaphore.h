/*
  Copyright (C) 2005, 2004 Erik Eliasson, Johan Bilien
  
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
 * Authors: Erik Eliasson <eliasson@it.kth.se>
 *          Johan Bilien <jobi@via.ecp.fr>
*/


/**
 * Semaphores are implemented differently depending on platform. This class
 * is a generalization of a semaphore implementation and only needs the
 * "Mutex" class. Only two operations are implemented, "inc" and "dec".
 *
 * @author Erik Eliasson, eliasson@it.kth.se
*/

#ifndef _MINISIPSEMAPHORE_H
#define _MINISIPSEMAPHORE_H

#include <libmutil/libmutil_config.h>

#include<string>
#include<libmutil/MemObject.h>
#include<libmutil/Exception.h>

class LIBMUTIL_API SemaphoreException : public Exception{
	public:
		SemaphoreException(std::string what);
};

class LIBMUTIL_API Semaphore : public MObject{
    public:
        Semaphore();
        ~Semaphore();
        
	std::string getMemObjectType() const {return "Semaphore";}

        /**
         * Put one resource into the set of resources.
         */
        void inc();
        
        /**
         * Wait until at least one resource is available and take it.
         */
        void dec();

    private:
	void *handlePtr;
};

#endif
