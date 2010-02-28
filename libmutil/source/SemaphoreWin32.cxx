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


#include<config.h>

#include<libmutil/Semaphore.h>
#include<libmutil/merror.h>
#include<stdlib.h>
#include<stdio.h>
#include<errno.h>

#include<iostream>
using namespace std;

#include<libmutil/dbg.h>


#include<windows.h>
#define SEMHANDLE (*((HANDLE*)(handlePtr)))


SemaphoreException::SemaphoreException(std::string w)
	: Exception(w)
{

}


Semaphore::Semaphore(){
	handlePtr = (void*) new HANDLE;
	SEMHANDLE = CreateSemaphore(NULL, 0, 1000, NULL);
	if (SEMHANDLE == NULL){
		merror("Semaphore::Semaphore: CreateSemaphore");
		throw SemaphoreException("CreateSemaphore failed");
	}
}
 
Semaphore::~Semaphore(){
	if (!CloseHandle(SEMHANDLE)){
		merror("Semaphore::~Semaphore: CloseHandle");
	}
	delete (HANDLE*)handlePtr;
}

void Semaphore::inc(){
	if (!ReleaseSemaphore(SEMHANDLE, 1, NULL)){
		merror("Semaphore::inc: ReleaseSemaphore");
		throw SemaphoreException("ReleaseSemaphore failed");
	}
}

void Semaphore::dec(){
	int dwWaitResult = WaitForSingleObject( 
			SEMHANDLE,   // handle to semaphore
			INFINITE);          // zero-second time-out interval

	switch (dwWaitResult) 
	{ 
	case WAIT_OBJECT_0: 
		// OK .
		break; 

	case WAIT_TIMEOUT: 
		merror("Semaphore::dec: WaitForSingleObject");
		throw SemaphoreException("WaitForSingleObject failed");
		break; 
	}
}
