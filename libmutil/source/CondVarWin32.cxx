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
#include<libmutil/CondVar.h>
#include<libmutil/Mutex.h>
#include<libmutil/merror.h>
#include<windows.h>

#define INTERNAL_COND_WAIT ((HANDLE *)internalStruct)
#define INTERNAL_MUTEX ((HANDLE *)internalMutexStruct)


CondVar::CondVar(){
	internalStruct = new HANDLE;
	if ( (*INTERNAL_COND_WAIT = CreateEvent( NULL, TRUE, FALSE, NULL ))==NULL){
		merror("CondVar::CondVar: CreateEvent");
	}
}

CondVar::~CondVar(){
	if (!CloseHandle( *INTERNAL_COND_WAIT )){
		merror("CondVar::~CondVar: CloseHandle");
	}
	delete (HANDLE *)internalStruct;
	internalStruct=NULL;
}

std::string CondVar::getMemObjectType() const {
	return "StringAtom";
}

void CondVar::wait( Mutex &mutex, uint32_t timeout ){
	mutex.unlock();
	wait( timeout );
	mutex.lock();
}

void CondVar::wait( uint32_t timeout ){
	if( timeout == 0 ){
		if (WaitForSingleObject(*INTERNAL_COND_WAIT, INFINITE)==WAIT_FAILED){
			merror("CondVar::wait: WaitForSingleObject");
		}
	}
	else{
		if (WaitForSingleObject(*INTERNAL_COND_WAIT, timeout)==WAIT_FAILED){
			merror("CondVar::wait: WaitForSingleObject");
		}
	}
}

void CondVar::broadcast(){
	if (!SetEvent(*INTERNAL_COND_WAIT)){
		merror("CondVar::broadcast: SetEvent");
	}
	if (!ResetEvent(*INTERNAL_COND_WAIT)){
		merror("CondVar::broadcast: ResetEvent");
	}
}

