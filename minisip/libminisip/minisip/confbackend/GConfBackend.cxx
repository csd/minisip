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

#include<config.h>
#include"GConfBackend.h"

#include <gconf/gconf.h>
#include <gconf/gconf-client.h>

#ifdef MAEMO_SUPPORT
#define KEY_ROOT "/apps/maemo/minisip/"
#else
#define KEY_ROOT "/apps/minisip/"
#endif

using namespace std;

GConfBackend::GConfBackend(){

	g_type_init();

	client = gconf_client_get_default();

	if( !client ){
		cerr << "Could not create GConf client" << endl;
		throw ConfBackendException();
	}

}

GConfBackend::~GConfBackend(){
	g_object_unref( client );
}

void GConfBackend::commit(){
}

void GConfBackend::save( const std::string &key, const std::string &value ){
	GError * err = NULL;
	string skey = key;
	sanitizeKey( skey );

	string rkey = KEY_ROOT + skey;
	const gchar * gkey = (const gchar *)rkey.c_str();
	const gchar * gvalue = (const gchar *)(value).c_str();

	if( !gconf_client_set_string( client, gkey, gvalue, &err ) ){
		g_clear_error( &err );
		throw ConfBackendException();
	}
}

void GConfBackend::save( const std::string &key, const int32_t value ){
	GError * err = NULL;
	string skey = key;
	sanitizeKey( skey );

	string rkey = KEY_ROOT + skey;
	const gchar * gkey = (const gchar *)rkey.c_str();
	gint gvalue = (gint)value;

	if( !gconf_client_set_int( client, gkey, gvalue, &err ) ){
		g_clear_error( &err );
		throw ConfBackendException();
	}

}

std::string GConfBackend::loadString( const std::string &key, 
		                      const std::string &defaultValue ){
	GError * err = NULL;
	string skey = key;
	sanitizeKey( skey );

	string rkey = KEY_ROOT + skey;
	const gchar * gkey = (const gchar *)rkey.c_str();
	gchar * gvalue;
	string ret;
	
	gvalue = gconf_client_get_string( client, gkey, &err );

	if( !gvalue ){
		if( err ){
			g_clear_error( &err );
			throw ConfBackendException();
		}
		ret = defaultValue;

	}
	else{
		ret = string( (char *)gvalue );
	}


	return ret;

}

int32_t GConfBackend::loadInt( const std::string &key, 
		               const int32_t defaultValue ){
	GError * err = NULL;
	string skey = key;
	sanitizeKey( skey );

	string rkey = KEY_ROOT + skey;
	const gchar * gkey = (const gchar *)rkey.c_str();
	GConfValue * gvalue;
	
	gvalue = gconf_client_get_without_default( client, gkey, &err );

	if( ! gvalue ){
		return defaultValue;
	}

	if( gvalue->type != GCONF_VALUE_INT ){
		throw ConfBackendException();
	}

	return (int32_t)( gconf_value_get_int( gvalue ) );

}

void GConfBackend::reset( const string &key ){
	GError * err = NULL;
	string skey = key;
	sanitizeKey( skey );

	string rkey = KEY_ROOT + skey;
	const gchar * gkey = (const gchar *)rkey.c_str();

	if( ! gconf_client_unset( client, gkey, &err ) ){
		// Unset failed, not sure what to do
	}
	
}

void GConfBackend::sanitizeKey( string &key ){
	size_t n = 0;
	do{
		n = key.find( '[', n );
		if( n!= string::npos ){
			key[n] = '_';
		}
	} while( n != string::npos );

	n = 0;
	do{
		n = key.find( ']', n );
		if( n!= string::npos ){
			key[n] = '_';
		}
	} while( n != string::npos );

}
