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

/* Copyright (C) 2004 
 *
 * Authors: Erik Eliasson <eliasson@it.kth.se>
 *          Johan Bilien <jobi@via.ecp.fr>
*/

#include<libmutil/CommandString.h>

CommandString::CommandString(string destination_id, 
		string operation, 
		string parameter, 
		string parameter2, 
		string parameter3)/* :
                                        destination_id(destination_id), 
					op(operation), 
					param(parameter), 
					param2(parameter2),
				       	param3(parameter3)*/
{
	keys["destination_id"] = destination_id;
	keys["op"] = operation;
	keys["param"] = parameter;
	keys["param2"] = parameter2;
	keys["param3"] = parameter3;
}

CommandString::CommandString(const CommandString &smc){
//	this->destination_id = smc.destination_id;
//	this->op = smc.op;
//	this->param = smc.param;
//	this->param2 = smc.param2;
//	this->param3 = smc.param3;
	this->keys = smc.keys;
}

string CommandString::getDestinationId(){
	return keys["destination_id"];
//	return destination_id;
}

void CommandString::setDestinationId(string id){
	keys["destination_id"] = id;
//	destination_id=id;
}

string CommandString::getOp(){
	return keys["op"];
//	return op;
}

void CommandString::setOp(string o){
	keys["op"] = o;
//	this->op=o;
}

string CommandString::getParam(){
	return keys["param"];
//	return param;
}

void CommandString::setParam(string p){
	keys["param"];
//	param=p;
}

string CommandString::getParam2(){
	return keys["param2"];
//	return param2;
}

void CommandString::setParam2(string p){
	keys["param2"] = p;
//	param2=p;
}

string CommandString::getParam3(){
	return keys["param3"];
//	return param3;
}

void CommandString::setParam3(string p){
	keys["param3"] = p;
//	param3=p;
}

string CommandString::getString(){
	return "op="+keys["op"]+"; param="+keys["param"]+" param2="+keys["param2"]+" param3="+keys["param3"]+" callid="+keys["destination_id"];
}

string &CommandString::operator[](string key){
	return keys[key];
}

