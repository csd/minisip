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

/* Copyright (C) 2005
 *
 * Authors: Erik Eliasson <eliasson@it.kth.se>
 *          Johan Bilien <jobi@via.ecp.fr>
*/

#include<config.h>

#include"ConfBackend.h"
#ifdef GCONF_SUPPORT
#include"GConfBackend.h"
#endif

#include"MXmlConfBackend.h"


MRef<ConfBackend *> ConfBackend::create(){
	try{
#ifdef GCONF_SUPPORT
		return new GConfBackend();
#else
		return new MXmlConfBackend();
#endif

	}
	catch( ConfBackendException & exc ){
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
