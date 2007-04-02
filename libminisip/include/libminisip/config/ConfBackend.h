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

#ifndef CONF_BACKEND_H
#define CONF_BACKEND_H

#include<libminisip/libminisip_config.h>

#include<libmutil/MemObject.h>
#include<libmutil/mtypes.h>
#include<libmutil/MPlugin.h>
#include<string>

class OnlineConfBack;

class LIBMINISIP_API ConfBackend : public MObject{
	public:
		virtual void save( const std::string &key, 
				const std::string &value )=0;
		virtual void save( const std::string &key, const int32_t value )=0;
		virtual void saveBool( const std::string &key, bool value );

		virtual std::string loadString( const std::string &key, const std::string &defaultValue="" )=0;
		virtual int32_t loadInt( const std::string &key, const int32_t defaultValue=0 )=0;
		virtual bool loadBool( const std::string &key, bool defaultValue=false );

		virtual void reset( const std::string & /* key */ ){};

		virtual void commit()=0;
   
                virtual OnlineConfBack * getConf(){return NULL;}

		void save( const char * key, const std::string &value );
		void save( const char * key, const int32_t value );
		void saveBool( const char * key, bool value );
		
		std::string loadString( const char * key, const std::string &defaultValue="" );
		int32_t loadInt( const char * key, const int32_t defaultValue=0 );
};

class LIBMINISIP_API ConfBackendException{};

class LIBMINISIP_API ConfigPlugin : public MPlugin{
	public:
		/**
		 * @param argument  A configuration backend might want
		 *		to be started with an argument. An example
		 *		is a path to a file containing the 
		 *		configuration data.
		 */
		virtual MRef<ConfBackend *> createBackend( const std::string& argument=NULL )const=0;

		virtual std::string getPluginType()const{ return "Config"; }

	protected:
		ConfigPlugin( MRef<Library *> lib );
};

/**
 * Registry of config plugins.
 */
class ConfigRegistry: public MPluginRegistry{
	public:
		virtual ~ConfigRegistry();

		virtual std::string getPluginType(){ return "Config"; }

		static MRef<ConfigRegistry*> getInstance();

		MRef<ConfBackend*> createBackend( std::string backendName="" );

		virtual void registerPlugin( MRef<MPlugin*> plugin );

	protected:
		ConfigRegistry();
		void registerBuiltins();

	private:
		static MRef<ConfigRegistry *> instance;
};

#endif
