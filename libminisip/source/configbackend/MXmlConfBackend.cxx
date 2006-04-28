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

#include"MXmlConfBackend.h"
#include<libminisip/configbackend/UserConfig.h>

#include<libmutil/XMLParser.h>
#include<libmutil/itoa.h>

#include<stdlib.h>

using namespace std;

static std::list<std::string> pluginList;
static bool initialized;


extern "C" LIBMINISIP_API
std::list<std::string> *mxmlconf_LTX_listPlugins( MRef<Library*> lib ){
	if( !initialized ){
		pluginList.push_back("getPlugin");
		initialized = true;
	}

	return &pluginList;
}

extern "C" LIBMINISIP_API
MRef<MPlugin *> mxmlconf_LTX_getPlugin( MRef<Library*> lib ){
	return new MXmlConfigPlugin( lib );
}

MXmlConfBackend::MXmlConfBackend(){
	fileName = getDefaultConfigFilename();

	try{
		parser = new XMLFileParser( fileName );
	}
	catch( XMLFileNotFound & ){
		// Open a new one
		parser = new XMLFileParser( "" );
	}
	catch( XMLException &exc ){
		mdbg << "MXmlConfBackend caught XMLException: " << exc.what() << end;
		cerr << "Caught XMLException" << endl;
		throw ConfBackendException();
	}
}

MXmlConfBackend::~MXmlConfBackend(){
	delete parser;
}

void MXmlConfBackend::commit(){
	parser->saveToFile( fileName );
}

void MXmlConfBackend::save( const std::string &key, const std::string &value ){
	try{
		parser->changeValue( key, value );
	}
	catch( XMLException &exc ){
		mdbg << "MXmlConfBackend caught XMLException: " << exc.what() << end;
		throw ConfBackendException();
	}
	
}

void MXmlConfBackend::save( const std::string &key, const int32_t value ){
	try{
		parser->changeValue( key, itoa( value ) );
	}
	catch( XMLException &exc ){
		mdbg << "MXmlConfBackend caught XMLException: " << exc.what() << end;
		throw ConfBackendException();
	}
}


std::string MXmlConfBackend::loadString( const std::string &key, const std::string &defaultValue ){
	std::string ret = "";
	
	try{
		ret = parser->getValue( key, defaultValue );
	}
	catch( XMLException &exc ){
		mdbg << "MXmlConfBackend caught XMLException: " << exc.what() << end;
		throw ConfBackendException();
	}

	return ret;

}

int32_t MXmlConfBackend::loadInt( const std::string &key,
		                  const int32_t defaultValue ){
	int32_t ret = -1;
	
	try{
		ret = parser->getIntValue( key, defaultValue );
	}
	catch( XMLException &exc ){
		mdbg << "MXmlConfBackend caught XMLException: " << exc.what() << end;
		throw ConfBackendException();
	}

	return ret;
}

string MXmlConfBackend::getDefaultConfigFilename(){
	return UserConfig::getFileName( "minisip.conf" );
}

MXmlConfigPlugin::MXmlConfigPlugin( MRef<Library *> lib ): ConfigPlugin( lib ){
}

MRef<ConfBackend *> MXmlConfigPlugin::createBackend(MRef<Gui*> gui)const{
	return new MXmlConfBackend();
}

std::string MXmlConfigPlugin::getName()const{
	return "mxmlconf";
}

std::string MXmlConfigPlugin::getDescription()const{
	return "MXmlConf backend";
}

uint32_t MXmlConfigPlugin::getVersion()const{
	return 0x00000001;
}
