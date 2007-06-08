/*
  Copyright (C) 2005, 2004 Erik Eliasson, Johan Bilien
  Copyright (C) 2006 Mikael Magnusson
  
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
 *          Mikael Magnusson <mikma@users.sourceforge.net>
*/



#include<config.h>

#include<libmutil/dbg.h>

#include<iostream>

#include<libmutil/stringutils.h>


Dbg mout;
Dbg merr(false);
Dbg mdbg(true, false);

DbgEndl end;


LIBMUTIL_API bool outputStateMachineDebug = false;

Dbg::Dbg(bool error_output, bool isEnabled):error_out(error_output), enabled(isEnabled), debugHandler(NULL){
}

void Dbg::setEnabled(bool e){
	enabled = e;
}

bool Dbg::getEnabled(){
	return enabled;
}

Dbg &Dbg::operator<<(const std::string& s){
	if (!enabled)
		return *this;
	
	bool doFlush = s.size()>0 && (s[s.size()-1]=='\n');

	if (debugHandler!=NULL){
		str += s;
		if (doFlush){
			if (error_out)
				std::cerr << str << std::flush;
			else
				debugHandler->displayMessage(str,0);
			str="";
		}
	}else{
		if (error_out){
			std::cerr << s;
			if (doFlush)
				std::cerr << std::flush;
		}else{
			std::cout<<s;
			if (doFlush)
				std::cout << std::flush;
		}
	}
	return *this;
}

Dbg &Dbg::operator<<( std::ostream&(*)(std::ostream&) ){
	return (*this)<<"\n";
}

Dbg &Dbg::operator<<(const DbgEndl &){
	return (*this)<<"\n";
}

Dbg& Dbg::operator<<(int i){
	return (*this)<<itoa(i);
}


Dbg& Dbg::operator<<(unsigned int i){
	return (*this)<<itoa(i);
}

Dbg& Dbg::operator<<(long long ll){
	return (*this)<<itoa(ll);
}

Dbg& Dbg::operator<<(char c){
	return (*this)<<std::string("")+c;
}

Dbg& Dbg::operator<<(void *p){
	return (*this)<<(long long)p;
}

void Dbg::setExternalHandler(DbgHandler * dh){
	this->debugHandler = dh;
}

