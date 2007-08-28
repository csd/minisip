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
#include<libminisip/config/UserConfig.h>

#include<libmutil/XMLParser.h>
#include<libmutil/stringutils.h>

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
MPlugin * mxmlconf_LTX_getPlugin( MRef<Library*> lib ){
	return new MXmlConfigPlugin( lib );
}

string searchReplace(const string &str,
			  const string &search,
			  const string &replace)
{
	string tmp = str;
	size_t pos = 0;
	size_t len = search.length();

	while( (pos = tmp.find(search, pos)) != string::npos ){
		tmp.erase(pos, len);
		tmp = tmp.insert(pos, replace);
		pos = pos + replace.length();
	}

	return tmp;
}

MXmlConfBackend::MXmlConfBackend( const string& path ){

	if (path.size()>0)
		fileName = path;
	else
		fileName = getDefaultConfigFilename();

	try{
		parser = new XMLFileParser( fileName );
	}
	catch( XMLFileNotFound & ){
		// Open a new one
		parser = new XMLFileParser( "" );
	}
	catch( XMLException &exc ){
		mdbg << "MXmlConfBackend caught XMLException: " << exc.what() << endl;
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
		string tmp = searchReplace( value, "&", "&amp;" );
		string xmlStr = searchReplace( tmp, "<", "&lt;" );
		parser->changeValue( key, xmlStr );
	}
	catch( XMLException &exc ){
		mdbg << "MXmlConfBackend caught XMLException: " << exc.what() << endl;
		throw ConfBackendException();
	}
	
}

void MXmlConfBackend::save( const std::string &key, const int32_t value ){
	try{
		parser->changeValue( key, itoa( value ) );
	}
	catch( XMLException &exc ){
		mdbg << "MXmlConfBackend caught XMLException: " << exc.what() << endl;
		throw ConfBackendException();
	}
}


std::string MXmlConfBackend::loadString( const std::string &key, const std::string &defaultValue ){
	std::string ret = "";
	
	try{
		string xmlStr = parser->getValue( key, defaultValue );
		string tmp = searchReplace( xmlStr, "&lt;", "<" );
		ret = searchReplace( tmp, "&amp;", "&" );
	}
	catch( XMLException &exc ){
		mdbg << "MXmlConfBackend caught XMLException: " << exc.what() << endl;
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
		mdbg << "MXmlConfBackend caught XMLException: " << exc.what() << endl;
		throw ConfBackendException();
	}

	return ret;
}

string MXmlConfBackend::getDefaultConfigFilename(){
	return UserConfig::getFileName( "minisip.conf" );
}

MXmlConfigPlugin::MXmlConfigPlugin( MRef<Library *> lib ): ConfigPlugin( lib ){
}

MRef<ConfBackend *> MXmlConfigPlugin::createBackend(const string &configPath)const{
	return new MXmlConfBackend(configPath);
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
