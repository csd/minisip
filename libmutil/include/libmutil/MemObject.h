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



#ifndef _MEMOBJECT_H
#define _MEMOBJECT_H

/*
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

#include<string>
#include<list>

#include<assert.h>
#include<libmutil/dbg.h>
#include<libmutil/Mutex.h>

#ifdef _MSC_VER
#ifdef LIBMUTIL_EXPORTS
#define LIBMUTIL_API __declspec(dllexport)
#else
#define LIBMUTIL_API __declspec(dllimport)
#endif
#else
#define LIBMUTIL_API
#endif

class MObject;

template<class OPType>
class MRef{
	public:
		MRef(){
			objp=NULL;
		}

		MRef(OPType optr){ 
			objp = optr; 
			if (objp!=NULL){
				assert(dynamic_cast<MObject*>(optr)!=NULL);
				objp->incRefCount(); 
			}
		}
		
		MRef(const MRef<OPType> &r){
			this->objp = r.objp;
			if (objp!=NULL){
				objp->incRefCount();
			}
		}

		virtual ~MRef(){          
			if (objp!=NULL){
#ifndef _MSC_VER
				assert(dynamic_cast<MObject*>(objp)!=NULL);
#endif
				int rc = objp->decRefCount();
				if (rc<=0){
					if (rc<0){
#ifndef _MSC_VER
						merr << "MRef::~MRef: WARNING: deleteing object with negative reference count (" 
							<< rc
							<< ") - created without reference?" << end;
#endif
					}       
					delete objp;
				}
			}
			objp=NULL;
		}
		
		void operator=(const MRef<OPType> &r){
			if (this->objp!=NULL){
				assert(dynamic_cast<MObject*>(objp)!=NULL);
				int rc = objp->decRefCount();

				if (rc<=0){
					if (rc<0){
#ifndef _MSC_VER
						merr << "MRef::~MRef: WARNING: deleteing object with negative reference count - created without reference?"<<end;
#endif
					}       

					delete objp;
				}
			}

			this->objp = r.objp;

			if (objp!=NULL){
				assert(dynamic_cast<MObject*>(objp)!=NULL);
				objp->incRefCount();
			}
		}

		bool operator ==(const MRef<OPType> r) const {
			return this->objp == r.objp;
		}

		operator bool() const {
			return !isNull();
		}

		void operator=(const OPType o){

			if (this->objp!=NULL){
				assert(dynamic_cast<MObject*>(objp)!=NULL);
				int rc = objp->decRefCount();
				if (rc<=0){
					if (rc<0){
#ifndef _MSC_VER
						merr << "MRef::~MRef: WARNING: deleteing object with negative reference count - created without reference?"<<end;
#endif
					}       

					delete objp;
				}
			}

			this->objp = o;
			if (objp!=NULL){
				assert(dynamic_cast<MObject*>(objp)!=NULL);
				objp->incRefCount();
			}
		}


		OPType operator->() const {
			return objp; 
		}

		OPType operator*(){
			return objp;
		}

		bool isNull() const{
			return objp==NULL;
		}


	private:
		OPType objp;


};

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
		 * Deconstructor.
		 */
		virtual ~MObject();

		int decRefCount();
		
		void incRefCount();
		
//		virtual std::string getMemObjectType(){return "(unknown)";}

		virtual std::string getMemObjectType()=0;

	private:
		int refCount;
		Mutex refLock;
};


#endif

