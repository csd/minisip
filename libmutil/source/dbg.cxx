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

Dbg::Dbg(bool error_output, bool isEnabled):
		error_out(error_output),
		enabled(isEnabled),
		debugHandler(NULL),
		defaultInclude(true),
		filterBlocking(false)
{
}

void Dbg::setEnabled(bool e){
	enabled = e;
}

bool Dbg::getEnabled(){
	return enabled;
}

Dbg &Dbg::operator<<(const std::string& s){

	if (!enabled || filterBlocking)
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
	(*this)<<"\n";
	curClass="";
	updateFilter();
	return (*this);
}

Dbg &Dbg::operator<<(const DbgEndl &){
	(*this)<<"\n";
	curClass="";
	updateFilter();
	return (*this);
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

/*
   merr("media/rtp") << "hello" << endl;






*/


/**
 *
 * set contains
 *   a/b
 * filter
 *   a
 * result: false
 *
 * set contains
 *  a
 * filter 
 *  a/b
 * result: true
 */
static bool inSet( std::set< std::string > &set, std::string filter ){
	std::set< std::string >::const_iterator i;
	for (i=set.begin() ; i!=set.end(); i++){
		std::string setfilt = (*i);
		if ( setfilt[0]=='/' )
			setfilt = setfilt.substr(1);
		if ( filter.substr(0,setfilt.size()) == setfilt ){
			return true;
		}
	}
	return false;
}

void Dbg::updateFilter(){
	if (inSet( excludeSet, curClass ))
		filterBlocking=true;
	else if (inSet( includeSet, curClass ))
		filterBlocking=false;
	else 
		filterBlocking = ! defaultInclude;
}

Dbg& Dbg::operator()(std::string oClass){
	curClass = oClass;
	updateFilter();
	return *this;
}


/**
 * 
 * Removes all filters from a set that is equal to or begins with
 * a string.
 * Example:
 *   If the set contains
 *      a
 *      a/b
 *      a/c
 *      b
 *   And you remove
 *      a
 *   then the resulting set contains
 *      b
 */
static void removeStartingWith( std::set< std::string > &set, std::string filter ){
	std::set< std::string >::iterator i;
	if (filter.size()<=0)
		return;

	if (filter[0]=='/')
		filter = filter.substr(1);

	if (filter[filter.size()-1]=='/')
		filter = filter.substr(0, filter.size()-1);

	for (i=set.begin() ; i!=set.end(); i++){
		std::string tmp = (*i);
		if ( tmp[0]=='/' )
			tmp = tmp.substr(1);
		if ( tmp.substr( 0, filter.size() ) == filter ){
			set.erase(i);
			i=set.begin();
		}
		if (i==set.end())
			break;
	}
}

void Dbg::include(std::string s){
	if (s==""){
		defaultInclude=true;
		includeSet.clear();
		excludeSet.clear();
	}else{
		includeSet.insert(s);
		removeStartingWith(excludeSet, s);
	}
	updateFilter();
}

void Dbg::exclude(std::string s){
	if (s==""){
		defaultInclude=false;
		includeSet.clear();
		excludeSet.clear();
	}else{
		excludeSet.insert(s);
		removeStartingWith(includeSet, s);
	}
	updateFilter();
}

