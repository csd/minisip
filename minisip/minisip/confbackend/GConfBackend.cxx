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
#include"GConfBackend.h"

#include <gconf/gconf.h>
#include <gconf/gconf-client.h>

#ifdef MAEMO_SUPPORT
#define KEY_ROOT "/apps/maemo/minisip/"
#else
#define KEY_ROOT "/apps/minisip/"
#endif


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

void GConfBackend::save( std::string key, std::string value ){
	GError * err = NULL;
	sanitizeKey( key );

	const gchar * gkey = (const gchar *)(KEY_ROOT + key).c_str();
	const gchar * gvalue = (const gchar *)(value).c_str();

	if( !gconf_client_set_string( client, gkey, gvalue, &err ) ){
		g_clear_error( &err );
		throw ConfBackendException();
	}
}

void GConfBackend::save( std::string key, int32_t value ){
	GError * err = NULL;
	sanitizeKey( key );
	const gchar * gkey = (const gchar *)(KEY_ROOT + key).c_str();
	gint gvalue = (gint)value;

	if( !gconf_client_set_int( client, gkey, gvalue, &err ) ){
		g_clear_error( &err );
		throw ConfBackendException();
	}

}

std::string GConfBackend::loadString( std::string key, std::string defaultValue ){
	GError * err = NULL;
	sanitizeKey( key );
	const gchar * gkey = (const gchar *)(KEY_ROOT + key).c_str();
	gchar * gvalue;
	string ret;
	
	gvalue = gconf_client_get_string( client, gkey, &err );

	if( !gvalue ){
		if( err ){
			g_clear_error( &err );
			throw ConfBackendException();
		}
		ret = "";

	}
	else{
		ret = string( (char *)gvalue );
	}


	return ret;

}

int32_t GConfBackend::loadInt( std::string key, int32_t defaultValue ){
	GError * err = NULL;
	sanitizeKey( key );
	const gchar * gkey = (const gchar *)(KEY_ROOT + key).c_str();
	gint gvalue;
	
	gvalue = gconf_client_get_int( client, gkey, &err );

	return (int32_t)gvalue;

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
