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

#include"MXmlConfBackend.h"

#include<libmutil/XMLParser.h>
#include<libmutil/itoa.h>

#include<stdlib.h>

using namespace std;

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

        string ret;
        char *home = NULL;
#ifndef _WIN32_WCE
		home = getenv("HOME");
#endif
        if (home==NULL){
                merr << "WARNING: Could not determine home directory"<<end;

#ifdef WIN32
                ret = string("c:\\minisip\\minisip.conf");
#else
                ret = string("/.minisip.conf");
#endif
        }else{
                ret = string(home)+ string("/.minisip.conf");
        }
        return ret;
}
