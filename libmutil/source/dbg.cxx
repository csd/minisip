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

#include<libmutil/dbg.h>

#include<iostream>

#include<libmutil/itoa.h>

Dbg mout;
Dbg merr(false);
Dbg mdbg(true, false);
DbgEndl end;

//#ifdef SM_DEBUG
LIBMUTIL_API bool outputStateMachineDebug = false;
//#endif

Dbg::Dbg(bool error_output, bool isEnabled):error_out(error_output), enabled(isEnabled), debugHandler(NULL){
}

void Dbg::setEnabled(bool e){
	enabled = e;
}

bool Dbg::getEnabled(){
	return enabled;
}

Dbg &Dbg::operator<<(std::string s){
	if (!enabled)
		return *this;
	//    std::cerr << "Doing textui output"<< std::endl;
	
	if (debugHandler!=NULL){
		str +=s;
		if (str[str.size()-1]=='\n'){
			if (error_out)
				std::cerr << str << std::flush;
			else
				debugHandler->displayMessage(str,0);
			str="";
		}
	}else{
		if (error_out)
			std::cerr << s;
		else
			std::cout<<s;
	}
	return *this;
}

Dbg &Dbg::operator<<(DbgEndl &){

	if (!enabled)
		return *this;
	//    std::cerr << "DbgEndl called"<< std::endl;
	if (debugHandler!=NULL){
		str+="\n";
		(*this)<< "";
	}else{
		if (error_out)
			std::cerr << std::endl;
		else
			std::cout << std::endl;
	}

	return *this;
}

Dbg& Dbg::operator<<(int i){

	if (!enabled)
		return *this;
    
    if (debugHandler!=NULL)
        str += itoa(i);
    else{
	if (error_out)
		std::cerr << i;
	else
		std::cout << i;
    }
    return (*this);
}

Dbg& Dbg::operator<<(char c){

	if (!enabled)
		return *this;

	if (debugHandler!=NULL)
		str += c;
	else{
		if (error_out)
			std::cerr << c;
		else
			std::cout << c;
	}
	return *this;
}

void Dbg::setExternalHandler(DbgHandler * debugHandler){
	this->debugHandler = debugHandler;
}

//#endif
