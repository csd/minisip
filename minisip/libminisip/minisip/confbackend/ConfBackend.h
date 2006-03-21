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

#ifndef CONF_BACKEND_H
#define CONF_BACKEND_H

#include<config.h>

#include<libmutil/MemObject.h>
#include<libmutil/mtypes.h>
#include"../gui/Gui.h"
#include<string>

class ConfBackend : public MObject{
	public:
		/**
		 * @param gui	A configuration backend can need to provide
		 * 		authentication information in order to 
		 * 		access the configuration. In that case it
		 * 		will ask the user via the gui object passed
		 * 		to this method for username and password.
		 * 		This is for example the case of the
		 * 		configuration is stored on server instead
		 * 		of on the local device.
		 */
		static MRef<ConfBackend *> create(MRef<Gui*> gui);

		virtual void save( const std::string &key, 
				const std::string &value )=0;
		virtual void save( const std::string &key, const int32_t value )=0;

		virtual std::string loadString( const std::string &key, const std::string &defaultValue="" )=0;
		virtual int32_t loadInt( const std::string &key, const int32_t defaultValue=0 )=0;

		virtual void reset( const std::string &key ){};

		virtual void commit()=0;

		void save( const char * key, const std::string &value );
		void save( const char * key, const int32_t value );
		
		std::string loadString( const char * key, const std::string &defaultValue="" );
		int32_t loadInt( const char * key, const int32_t defaultValue=0 );
};

class ConfBackendException{};


#endif
