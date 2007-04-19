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

#ifndef G_CONF_BACKEND_H
#define G_CONF_BACKEND_H

#include<libminisip/libminisip_config.h>

#include<libminisip/config/ConfBackend.h>

typedef struct _GConfClient GConfClient;

class GConfBackend : public ConfBackend {
	public:
		virtual void save( const std::string &key, 
				const std::string &value );
		virtual void save( const std::string &key, 
				const int32_t value );
		void saveBool( const std::string &key, bool value );


		virtual std::string loadString( const std::string &key, 
				const std::string &defaultValue="" );
		virtual int32_t loadInt( const std::string &key, 
				const int32_t defaultValue=0 );
		bool loadBool( const std::string &key, bool defaultValue=false );

		void reset( const std::string &key );

		virtual void commit();

		~GConfBackend();
		GConfBackend();

	private:
		GConfClient * client;

		void sanitizeKey( std::string &key );

};

class GConfigPlugin : public ConfigPlugin{
	public:
		GConfigPlugin( MRef<Library *> lib );

		/**
		 * @param gui	A configuration backend can need to provide
		 * 		authentication information in order to 
		 * 		access the configuration. In that case it
		 * 		will ask the user via the gui object passed
		 * 		to this method for username and password.
		 * 		This is for example the case of the
		 * 		configuration is stored on server instead
		 * 		of on the local device.
		 * @param arg	Not used by this backend.
		 */
		virtual MRef<ConfBackend *> createBackend(const std::string &arg)const;

		virtual std::string getMemObjectType() const { return "GConfBackend"; }

		virtual std::string getName()const;

		virtual std::string getDescription()const;

		virtual uint32_t getVersion()const;
};

#endif
