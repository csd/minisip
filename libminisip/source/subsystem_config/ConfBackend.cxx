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

#include<libminisip/config/ConfBackend.h>
#include<libminisip/config/OnlineConfBackend.h>
#include"MXmlConfBackend.h"
using namespace std;

MRef<ConfBackend *> ConfigRegistry::createBackend( std::string backendName ){
	string backendArgument;
	if (backendName.find(':')!=string::npos){
		backendArgument = backendName.substr(backendName.find(':')+1);
		backendName = backendName.substr(0, backendName.find(':') );
		
	}
						     
	try{
		MRef<MPlugin *> plugin;

		if( !backendName.empty() ){
			plugin = findPlugin( backendName );
		}
		else {

			plugin = findPlugin( "onlineconf" );
			
			if( !plugin)
				plugin = findPlugin( "gconf" );
			
			if( !plugin){
				plugin = findPlugin( "mxmlconf" );
			}
		}

		if( !plugin ){
			merr << "ConfigRegistry: Can't create config backend " << backendName << ::endl;
			return NULL;
		}
		
		ConfigPlugin *config = dynamic_cast<ConfigPlugin*>(*plugin);
		if( !config ){
			merr << "ConfigRegistry: Not a config plugin " << plugin->getName() << ::endl;
			return NULL;
		}

		return config->createBackend( backendArgument );
	}
	catch( ConfBackendException & ){
		return NULL;
	}
}

void ConfBackend::saveBool( const std::string &key, bool value ){
	save( key, value ? string("yes") : string("no") );
}

bool ConfBackend::loadBool( const std::string &key, bool defaultValue ){
	return loadString( key, defaultValue ? "yes" : "no" ) == "yes";
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

void ConfBackend::saveBool( const char * key, bool value ){
	saveBool( std::string( key ), value );
}

ConfigPlugin::ConfigPlugin( MRef<Library *> lib ): MPlugin( lib ){
}


MRef<ConfigRegistry *> ConfigRegistry::instance;

ConfigRegistry::ConfigRegistry(){
}

ConfigRegistry::~ConfigRegistry(){
}

void ConfigRegistry::registerBuiltins(){
	registerPlugin( new MXmlConfigPlugin( NULL ) );
}

MRef<ConfigRegistry *> ConfigRegistry::getInstance(){
	if( !instance ){
		instance = new ConfigRegistry();
		instance->registerBuiltins();
	}
	return instance;
}

void ConfigRegistry::registerPlugin( MRef<MPlugin*> plugin ){
	ConfigPlugin *config = dynamic_cast<ConfigPlugin *>(*plugin);

	if( config ){
		MPluginRegistry::registerPlugin( plugin );
	}
	else {
		merr << "ConfigRegistry: Not a config plugin " << plugin->getName() << endl;
	}
}
