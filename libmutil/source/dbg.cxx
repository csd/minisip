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


#include<config.h>

#include<libmutil/dbg.h>

#include<iostream>

#include"libmutil/TextUI.h"
#include<libmutil/itoa.h>

TextUI *debugtextui=NULL;

Dbg mout;
Dbg merr(true);
Dbg mdbg(true, false);
DbgEndl end;

Dbg::Dbg(bool error_output, bool isEnabled):error_out(error_output), enabled(isEnabled){
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
#ifdef TEXT_UI
	//    std::cerr << "Doing textui output"<< std::endl;
	
	if (textui!=NULL){
		str +=s;
		if (str[str.size()-1]=='\n'){
			if (error_out)
				std::cerr << str << std::flush;
			else
				textui->displayMessage(str,0);
			str="";
		}
	}else{
		if (error_out)
			std::cerr << s;
		else
			std::cout<<s;
	}
#else
	if (error_out)
		std::cerr << s;
	else
		std::cout << s;
#endif
	return *this;
}

Dbg &Dbg::operator<<(DbgEndl &endl){

	if (!enabled)
		return *this;
	//    std::cerr << "DbgEndl called"<< std::endl;
#ifdef TEXT_UI
	if (textui!=NULL){
		str+="\n";
		(*this)<< "";
	}else{
		if (error_out)
			std::cerr << std::endl;
		else
			std::cout << std::endl;
	}
#else
	if (error_out)
		std::cerr << std::endl;
	else
		std::cout << std::endl;
#endif

}

Dbg& Dbg::operator<<(int i){

	if (!enabled)
		return *this;
    
#ifdef TEXT_UI
    if (textui!=NULL)
        str += itoa(i);
    else{
	if (error_out)
		std::cerr << i;
	else
		std::cout << i;
    }
#else
    if (error_out)
	    std::cerr << i;
    else
	    std::cout << i;
#endif
    return *this;
}

Dbg& Dbg::operator<<(char c){

	if (!enabled)
		return *this;

#ifdef TEXT_UI
	if (textui!=NULL)
		str += c;
	else{
		if (error_out)
			std::cerr << c;
		else
			std::cout << c;
	}
#else
	if (error_out)
		std::cerr << c;
	else
		std::cout << c;
#endif
	return *this;
}

//#endif
