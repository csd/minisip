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

#include<libmutil/MemObject.h>
#include<libmutil/Mutex.h>
#include<libmutil/dbg.h>
#include<string>

#include<typeinfo>

using namespace std;

#ifdef MDEBUG
#include<libmutil/stringutils.h>
Mutex *globalLock=NULL;
minilist<MObject *> objs;
int ocount=0;
bool outputOnDestructor=false;

Mutex &global(){
	if (!globalLock)
		globalLock = new Mutex;
	return *globalLock;
}
#endif

MObject::MObject() : refCount(0){
#ifdef MDEBUG
	global().lock();
	ocount++;
	objs.push_front(this);
	refLock=NULL;
	global().unlock();
#else
	refLock = new Mutex();
#endif
}


// The reference count should be zero since
// any references to the argument object
// are not referencing us.
MObject::MObject(const MObject &):refCount(0){
#ifdef MDEBUG
	global().lock();
	ocount++;
	objs.push_front(this);
	refLock=NULL;
	global().unlock();
#else
	refLock = new Mutex();	//We don't want to share the mutex
#endif
}

MObject::~MObject(){
#ifdef MDEBUG
	global().lock();
	for (int i=0; i<objs.size(); i++){
		if (this == objs[i]){
			objs.remove(i);
			ocount--;
			break;
		}
	}
	global().unlock();
#else
	massert(refLock);
	delete refLock;
	refLock=NULL;
#endif
}

void MObject::operator=(const MObject &){
	// we don't copy the mutex handle - whe one we already
	// have protects the reference counter we in this object.
	// We also don't copy the reference counter. The value
	// we already have is the correct number of references.
	//
	// Don't delete this method even if it is empty.
}
	

int MObject::decRefCount() const{
	int refRet;
#ifdef MDEBUG
	global().lock();
#else
	refLock->lock();
#endif
	
	refCount--;
	refRet = refCount;
	
#ifdef MDEBUG
	global().unlock();
	if (refRet==0 && outputOnDestructor){
		string output = "MO (--):"+getMemObjectType()+ "; count=" + itoa(refRet) + "; ptr=" + itoa((int)this);
		mdbg("memobject") << output << endl;
	}
#else
	refLock->unlock();
#endif
	return refRet;
}

void MObject::incRefCount() const{
#ifdef MDEBUG
	global().lock();
#else
	refLock->lock();
#endif
	
	refCount++;
	
#ifdef MDEBUG
	global().unlock();
	if (refCount == 1 && outputOnDestructor ){
		string output = "MO (++):"+getMemObjectType()+ "; count=" + itoa(refCount);
		mdbg("memobject") << output << endl;
	}
#else
	refLock->unlock();
#endif
}

int MObject::getRefCount() const{
	return refCount;
}

string MObject::getMemObjectType() const {
#ifdef MDEBUG
	return (typeid(*this)).name();
#else
	return "(unknown)";
#endif
}

minilist<string> getMemObjectNames(){
#ifdef MDEBUG
	minilist<string> ret;
	global().lock();
	for (int i=0; i< objs.size(); i++){
		int count = objs[i]->getRefCount();
		string countstr = count?itoa(count):"on stack"; 
		ret.push_front(objs[i]->getMemObjectType()+"("+countstr+")" + "; ptr=" + itoa((int)objs[i]) );
	}
	global().unlock();
	return ret;
#else
	minilist<string> ret;
	return ret;
#endif
}

int getMemObjectCount(){
#ifdef MDEBUG
	return ocount;
#else
	return -1;
#endif
}

bool setDebugOutput(bool on){
#ifdef MDEBUG
	outputOnDestructor=on;
	return true;
#else
	return false;
#endif
}

bool getDebugOutputEnabled(){
#ifdef MDEBUG
	return outputOnDestructor;
#else
	return false;
#endif
}

minilist<string> getMemObjectNamesSummary(){
#ifdef MDEBUG
	minilist<string> ret;
	std::list<string> str;	// unique names
	std::list<int> count;   // count for corresponding name in str
	std::list<string>::iterator si;
	std::list<string>::iterator js;
	std::list<int>::iterator jc;
	global().lock();
	// get list of unique names, and count
	int i;
	for (i=0; i< objs.size(); i++){
		string oname = objs[i]->getMemObjectType();
		bool found=false;
		for (  jc=count.begin(), js=str.begin();  js!=str.end();  jc++, js++  ){
			if (*js==oname){
				(*jc)=*jc+1;
				found=true;
				break;
			}
		}
		if (!found){
			str.push_back(oname);
			count.push_back(1);
		}

	}

	// Bubble sort to arrange names is ascending order
	bool done;
	do {
		done=true;
		std::list<int>::iterator jc_last;
		std::list<string>::iterator js_last;
		jc_last = count.begin();
		js_last = str.begin();
		js=str.begin();
		jc=count.begin();
		if (js!=str.end()){ //start from second item
			js++;
			jc++;
		}

		for ( ; js!=str.end(); jc++, js++, jc_last++, js_last++){
			if ( (*jc) < (*jc_last) ){
				int tmpc=*jc;
				*jc = *jc_last;
				*jc_last=tmpc;

				string tmps=*js;
				*js = *js_last;
				*js_last=tmps;
				done=false;

			}
		}
	}while (!done);

	for (jc=count.begin(), js=str.begin(); js!=str.end(); jc++, js++){
		ret.push_back( *js + " " + itoa( *jc ) );
	}

	global().unlock();
	return ret;
#else
	minilist<string> ret;
	return ret;
#endif
}



