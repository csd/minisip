/*
  Copyright (C) 2005, 2004 Erik Eliasson, Johan Bilien
  
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
  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

/*
 * Authors: Erik Eliasson <eliasson@it.kth.se>
 *          Johan Bilien <jobi@via.ecp.fr>
*/

#include<config.h>
#include<libmutil/CommandString.h>

using namespace std;

CommandString::CommandString(string destination_id, 
		string operation, 
		string parameter, 
		string parameter2, 
		string parameter3)
{
	keys["destination_id"] = destination_id;
	keys["op"] = operation;
	keys["param"] = parameter;
	keys["param2"] = parameter2;
	keys["param3"] = parameter3;
}

CommandString::CommandString(const CommandString &smc){
	this->keys = smc.keys;
}

string CommandString::getDestinationId() const{
	return (*keys.find("destination_id")).second;
}

void CommandString::setDestinationId(string id){
	keys["destination_id"] = id;
}

string CommandString::getOp() const{
	return (*keys.find("op")).second;
}

void CommandString::setOp(string o){
	keys["op"] = o;
}

string CommandString::getParam() const{
	return (*keys.find("param")).second;
}

void CommandString::setParam(string p){
	keys["param"]=p;
}

string CommandString::getParam2() const{
	return (*keys.find("param2")).second;
}

void CommandString::setParam2(string p){
	keys["param2"] = p;
}

string CommandString::getParam3() const{
	return (*keys.find("param3")).second;
}

void CommandString::setParam3(string p){
	keys["param3"] = p;
}

string CommandString::getString() const{
	string ret;
	map<string,string>::const_iterator it;
	it = keys.begin();
	ret = "op=" + get("op") + "; ";
	for( it = keys.begin(); it!=keys.end(); it++ ) {
		if( it->first != "op" && it->second!="" )
			ret+= it->first + "=" + it->second + "; ";
	}
	return ret;
}

string &CommandString::operator[](string key){
	return keys[key];
}

string CommandString::get(const string &key) const{
	return (*keys.find(key)).second;
}
