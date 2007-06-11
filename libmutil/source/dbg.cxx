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
#include<libmutil/Mutex.h>
#include<libmutil/massert.h>


Dbg mout("mout");
Dbg merr("merr",false);
Dbg mdbg("mdbg",true, false);

DbgEndl end;


LIBMUTIL_API bool outputStateMachineDebug = false;

Dbg::Dbg(std::string name_, bool error_output, bool isEnabled):
		name(name_),
		error_out(error_output),
		enabled(isEnabled),
		debugHandler(NULL),
		defaultInclude(true),
		filterBlocking(false),
		setLock(NULL)

{
	setLock = new Mutex;
}

Dbg::~Dbg(){
	delete setLock;
	setLock=NULL;
}

void Dbg::setEnabled(bool e){
	enabled = e;
}

bool Dbg::getEnabled(){
	return enabled;
}

Dbg &Dbg::operator<<(const std::string& s){

	bool doFlush = s.size()>0 && (s[s.size()-1]=='\n');
	str += s;

	if ( (!enabled || filterBlocking) && doFlush){
		str="";
		return *this;
	}

	if (doFlush){
		std::string prefix;
		if (curClass.size()>0)
			prefix = "[" + curClass + "] ";
		else
			prefix = "[" + name + "] ";
			
		if (debugHandler!=NULL){
			debugHandler->displayMessage(prefix+str,0);
			str="";
		}else{
			if (error_out){
				std::cerr << prefix+str<<std::flush;
			}else{
				std::cout << prefix+str<<std::flush;
			}
			str="";
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
static bool inSet( std::set< std::string > &set, std::string filter, Mutex *setLock ){
	setLock->lock();
	std::set< std::string >::const_iterator i;
	for (i=set.begin() ; i!=set.end(); i++){
		std::string setfilt = (*i);
		if ( setfilt[0]=='/' )
			setfilt = setfilt.substr(1);
		if ( filter.substr(0,setfilt.size()) == setfilt ){
			setLock->unlock();
			return true;
		}
	}
	setLock->unlock();
	return false;
}

void Dbg::updateFilter(){
	if (inSet( excludeSet, curClass, setLock ))
		filterBlocking=true;
	else if (inSet( includeSet, curClass, setLock ))
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
static void removeStartingWith( std::set< std::string > &set, std::string filter, Mutex *setLock ){
	std::set< std::string >::iterator i;
	if (filter.size()<=0)
		return;

	if (filter[0]=='/')
		filter = filter.substr(1);

	if (filter[filter.size()-1]=='/')
		filter = filter.substr(0, filter.size()-1);

	setLock->lock();
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
	setLock->unlock();
}

void Dbg::include(std::string s){

	enabled=true;
	if (s==""){
		defaultInclude=true;
		setLock->lock();
		includeSet.clear();
		excludeSet.clear();
		setLock->unlock();
	}else{
		setLock->lock();
		includeSet.insert(s);
		setLock->unlock();
		removeStartingWith(excludeSet, s, setLock);
	}
	updateFilter();
}

void Dbg::exclude(std::string s){
	if (s==""){
		defaultInclude=false;
		setLock->lock();
		includeSet.clear();
		excludeSet.clear();
		setLock->unlock();
	}else{
		setLock->lock();
		excludeSet.insert(s);
		setLock->unlock();
		removeStartingWith(includeSet, s, setLock);
	}
	updateFilter();
}

