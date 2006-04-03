/*
 Copyright (C) 2004-2006 the Minisip Team
 
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
 Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
 */

/* Copyright (C) 2005
 *
 * Authors: Erik Eliasson <eliasson@it.kth.se>
 *          Johan Bilien <jobi@via.ecp.fr>
*/

#include"ConfBackend.h"

#ifdef GCONF_SUPPORT
#include"GConfBackend.h"
#endif

#include"MXmlConfBackend.h"

using namespace std;

MRef<ConfBackend *> ConfBackend::create(MRef<Gui*>){ // No configuration needs the GUI yet...
						     
	try{
#ifdef GCONF_SUPPORT
		return new GConfBackend();
#else
		return new MXmlConfBackend();
#endif

	}
	catch( ConfBackendException & ){
		return NULL;
	}
}

int32_t ConfBackend::loadInt( const char * key, const int32_t defaultValue ){
	return loadInt( std::string( key ), defaultValue );
}

string ConfBackend::loadString( const char * key, const string &defaultValue ){
	return loadString( std::string( key ), defaultValue );
}

void ConfBackend::save( const char * key, const int32_t value ){
	save( std::string( key ), value );
}

void ConfBackend::save( const char * key, const string &value ){
	save( std::string( key ), value );
}
