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
 * Authors:	Erik Eliasson <eliasson@it.kth.se>
 *		Johan Bilien <jobi@via.ecp.fr>
 *		Cesc Santasusana <cesc dot santa at g mail dot com>
*/




#ifndef _MEMOBJECT_H
#define _MEMOBJECT_H

#include <libmutil/libmutil_config.h>

#include<string>
#include<list>

#include<assert.h>
#include<libmutil/dbg.h>

#include <typeinfo>


#include<libmutil/minilist.h>

#include<libmutil/stringutils.h>
#include<libmutil/Exception.h>

#ifdef DEBUG_OUTPUT
#include <typeinfo>  //remove, for debug in ipaq
#endif

class Mutex;

/**
 * The MObject class contains a reference counter that is
 * used to determine when the object should be removed
 * from the heap (when no one is referencing it).
 *
 * Use this as superclass and use MRef instead of pointers.
 *
*/
class LIBMUTIL_API MObject{
	public:
	
		/**
		 * Constructor.
		 */
		MObject();
		
		/**
		 * Copy constructor
		 *
		 * Needed so that we don't get
		 * the reference count and mutex from the
		 * one we duplicate.
		 */
		MObject(const MObject &);


		/**
		 * Deconstructor.
		 */
		virtual ~MObject();

		/**
		 * Needed since reference count and the
		 * mutex that protects it should not be
		 * copied when MObjects are.
		 */
		void operator=(const MObject &);

		/**
		Decrease the reference counter (thread-safe)
		*/
		int decRefCount() const;
		
		/**
		Increase the reference counter (thread-safe)
		*/
		void incRefCount() const;

		/**
		Return the value of the reference counter
		*/
		int getRefCount() const;
		
		/**
		 * If mutil has been configured with --enable-memdebug
		 * (MDEBUG defined) the unique type name (with the format
		 * from typeinfo(...).name). An empty string will be
		 * returned if memory debugging was not implemented.
		 * Note that the format of the type name is compiler
		 * dependent.
		 *
		 * The purpose of this method is to allow debugging
		 * (such as showing which objects are allocated).
		 */
		virtual std::string getMemObjectType() const;

	private:
		/**
		Reference counter ... 
		*/
		mutable int refCount;
		
		/**
		Mutex, provides thread safety. 
		When MEMDEBUG is on, this is not used (instead a global mutex
		is).
		*/
		Mutex *refLock;
};


/**
This is the minisip Smart Pointer implementation: MRef
It is a template class, which uses reference counting for its "smartness".
The reference counter is attached to the MObject, thus we can only 
use MRef with MObjects. 

Pitfall 0: We can only MRef MObjects! I (cesc) tried to change it into an
	MObject-independent smart pointer class, but MObject is too widely
	used all over minisip. Specially painful are MRef conversions from
	one type to another ... i mean:
	MRef<X> x;
	MRef<Y> y;
	//if X inherits from Y
	y = new Y();
	x = *y;
	This works fine ... as the counter is attached to the MObject, thus
	both MRefs share the same counter.
	
Pitfall 1: You MAY _NOT_ create references to local variables. When the
	reference is deleted it will see that there is no other references
	to the object and try to delete it.
	Example of code with bug:
	void f(MRef<T> ref){
		...
	}
	
	...
	T obj;
	f(MRef<T>(&obj));
	
Pitfall 2: You MAY _NOT_ create a reference to "this" in a constructor.
	
Pitfall 3: If objects reference each other in such a way that
		it forms a circle (for example A->B, B->A or A->B, B->C, C->A)
		then the memory will not be freed automatically. Break the
		circle by putting one of the refecences to NULL and the whole
		list will be freed.
*/
template<class OPType>
class MRef{

	private:
		/**
		The non-smart pointer
		(it has to be a sub-class of MObject!)
		*/
		OPType objp;
#if 0		
		/**
		Keep a pointer to (MObject *)objp ... 
		(do not delete this one ... we will delete objp instead).
		*/
		MObject * mObj;
#endif	
	protected:
	
		/**
		Initialize the MRef and its counter for the first time.
		Call only when the objp we receive is freshly created with new
		*/
		inline bool init();
		
		/**
		Increase the reference counter ...
		*/
		inline bool increase();
		
		/**
		Decrease the reference counter ... and destroy if needed
		*/
		inline bool decrease();
		
		/**
		Return the raw pointer ... 
		*/
		inline OPType getPointer() const;
		
		/**
		Set the pointed object ... we perform a dynamic cast to check 
		if the received object is an MObject ..
		The boolean parameter (default true) tells wheather we want this
		   check to be performed or not (dynamic cast)
		*/
		inline void setPointer(OPType o);

	public:
		/**
		We receive the object freshly created with a new,
		or if without params, create an empty MRef
		*/
		inline MRef(OPType optr = NULL);
		
		/**
		We are copying an MRef object ... increase the counter
		*/
		inline MRef(const MRef<OPType> &r);

		/**
		Destructor.
		We must decrease the counter, and if needed, destroy the 
		allocated object and the counter.
		*/
		inline virtual ~MRef();
		
		/**
		Assign operator overloaded (check if object received is
		an MObject).
		We have to first get rid of the previous referred object,
		that is why first we decrease, then copy "o", then increase
		the counter.
		*/
		inline MRef<OPType>& operator=(const OPType o);

		/**
		Assign operator overloaded (copying an exhisting MRef)
		We have to first get rid of the previous referred object,
		that is why first we decrease, then copy "o", then increase
		the counter.
		*/
		inline MRef<OPType>& operator=(const MRef<OPType> &r);

		/**
		Overload the comparison operator (between MRefs).
		True if the referred objects are equal (not the MRefs)
		*/
		inline bool operator ==(const MRef<OPType> r) const;
		
		/**
		Overload the < operator (between MRefs).
		True if the referred objects are equal (not the MRefs)
		*/
		inline bool operator <(const MRef<OPType> r) const;

		/**
		Return true if contained object is null
		*/
		inline bool isNull() const;
		
		/**
		Overload the bool() operator
		*/
		inline operator bool() const;

		/**
		Overload the member access operator
		(this actually turns MRef into a smarter pointer
		*/
		inline OPType operator->() const;

		/**
		Overload the de-ref operator
		*/
		inline OPType operator*() const;
};


template<class OPType>
OPType MRef<OPType>::getPointer() const{
	return objp;
}


template<class OPType>
void MRef<OPType>::setPointer(const OPType o){
	if (o!=NULL){
		//check if the pointer is to an MObject ...
		if( dynamic_cast<OPType>(o) != NULL ) {
			objp = o;
		} else {
			std::cerr << "MRef: Trying to use an MRef for a non MObject class" << std::endl;
			std::cerr << "Stack trace: " << getStackTraceString() << std::endl;
		}	
	} else { 
		objp = NULL;
	}
}

template<class OPType>
bool MRef<OPType>::init() {
	bool ret = false;
	if ( objp!=NULL ) {
		ret = increase();
	}
	return ret;
}

template<class OPType>
bool MRef<OPType>::increase() {
	bool ret = false;
	if ( objp!=NULL ) {
		objp->incRefCount();
		ret = true;
	}
	return ret;
}

template<class OPType>
bool MRef<OPType>::decrease() {
	bool ret = false;

	if ( objp!=NULL ) {
		int rc = objp->decRefCount();
		if (rc<=0){
			if (rc<0){
#ifndef _MSC_VER
				merr << "MRef::~MRef: WARNING: deleteing object with negative reference count (" 
					<< rc
					<< ") - created without reference?" << std::endl;
#endif
			}       
			delete getPointer();
			setPointer(NULL);
			ret = true;
		}
	}
	return ret;
}

template<class OPType>
MRef<OPType>::MRef(OPType optr) { 
	setPointer(optr);
	init();
}

template<class OPType>
MRef<OPType>::MRef(const MRef<OPType> &r) {
	setPointer( r.getPointer() );
	increase();
}

template<class OPType>
MRef<OPType>::~MRef(){
	decrease();
	setPointer( NULL );
}

template<class OPType>
MRef<OPType>& MRef<OPType>::operator=(const OPType o){
	decrease();
	setPointer( o );
	increase();
	return *this;
}

template<class OPType>
MRef<OPType>& MRef<OPType>::operator=(const MRef<OPType> &r){
	decrease();
	setPointer( r.getPointer() );
	increase();
	return *this;
}

template<class OPType>
bool MRef<OPType>::operator ==(const MRef<OPType> r) const {
	return getPointer() == r.getPointer();
}

template<class OPType>
bool MRef<OPType>::operator <(const MRef<OPType> r) const {
	return getPointer() < r.getPointer();
}

template<class OPType>
bool MRef<OPType>::isNull() const{
	return getPointer()==NULL;
}

template<class OPType>
MRef<OPType>::operator bool() const {
	return !isNull();
}

template<class OPType>
OPType MRef<OPType>::operator->() const {
	OPType ret;
	ret = getPointer();
	if( ret == NULL ) {
		#ifdef DEBUG_OUTPUT
		std::cerr << "MRef::operator-> : ERROR: trying to access a null pointer (";
		std::cerr << typeid(OPType).name() << ").";
		std::cerr << std::endl;
		std::cerr << "Stack trace: "<< getStackTraceString() << std::endl;
		#endif
		assert( ret != NULL );
	}
	return getPointer(); 
}

template<class OPType>
OPType MRef<OPType>::operator*() const{
	OPType ret;
	ret = getPointer();
	if( ret == NULL ) {
		#ifdef DEBUG_OUTPUT
		std::cerr << "MRef::operator* : Warning: accessing a null pointer (" << typeid(OPType).name() << ")." << std::endl;
		std::cerr << "Stack trace: "<< getStackTraceString() << std::endl;
		#endif
	}
	return getPointer();
}


/**
 * if MDEBUG was defined when compiling libmutil it is possible 
 * to use following three debuging functions. "setDebugOutput"
 * can be used to see if this feature was enabled when compiling
 * libmutil. This functionality is only intended to be used
 * for debugging purposes since destructing an object has 
 * complexity O(n) where "n" is the number of MObject objects.
 *
 */


/**
 * @param on	If true, then a debug message will be printed
 * 		to the screen when a MObject that is allocated
 * 		on the heap will be removed. If false, no debug
 * 		messages will be output.
 * @return	True if debugging features was enabled when
 * 		compiling libmutil and false if it was not.
 */
LIBMUTIL_API bool setDebugOutput(bool on);

LIBMUTIL_API bool getDebugOutputEnabled();

/**
 *
 * @return	Number of MObject objects currently allocated on
 * 		the heap or on the stack.
 */
LIBMUTIL_API int getMemObjectCount();

/**
 * @return	A list of all allocated objects on the form
 * 		"String(count)" where "String" is what is returned
 * 		by "getMemObjectType" for the object and "count"
 * 		is the number of references to it or "on stack" if
 * 		there is no references to it (and it is most likely
 * 		not dynamically).
 */
LIBMUTIL_API minilist<std::string> getMemObjectNames();

/**
 * Same as getMemObjectNames() except that it reports the objects
 * as a count for each type.
 * Example:
 *   If getMemObjectNames would report the following objects:
 *     Name  Ref count
 *     TypeA 2
 *     TypeA 4
 *     TypeB 1
 *     TypeA <stack>
 *     TypeB 4
 *   then this function will report:
 *     Name  Count
 *     TypeA 3
 *     TypeB 2 
 */
LIBMUTIL_API minilist<std::string> getMemObjectNamesSummary();




#endif

