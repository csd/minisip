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
  
*/

#include<string>
#include<list>

#include<assert.h>
#include<libmutil/dbg.h>
#include<libmutil/Mutex.h>


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
		
/*		MRef(std::string u, OPType optr){ 
			user=u;
			objp = optr; 
			if (objp!=NULL){
				assert(dynamic_cast<MObject*>(optr)!=NULL);
				objp->incRefCount(); 
			}
		}
*/
		MRef(const MRef<OPType> &r){
			this->objp = r.objp;
			if (objp!=NULL){
				objp->incRefCount();
			}
		}

		virtual ~MRef(){          
			if (objp!=NULL){
				assert(dynamic_cast<MObject*>(objp)!=NULL);
				int rc = objp->decRefCount();
				if (rc<=0){
					if (rc<0){
						merr << "MRef::~MRef: WARNING: deleteing object with negative reference count (" 
							<< rc
							<< ") - created without reference?" << end;
					}       
					delete objp;
				}
			}
			objp=NULL;
		}
		
		void operator=(const MRef &r){
			if (this->objp!=NULL){
				assert(dynamic_cast<MObject*>(objp)!=NULL);
				int rc = objp->decRefCount();

				if (rc<=0){
					if (rc<0){
						merr << "MRef::~MRef: WARNING: deleteing object with negative reference count - created without reference?"<<end;
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

		bool operator ==(const MRef r) const {
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
						merr << "MRef::~MRef: WARNING: deleteing object with negative reference count - created without reference?"<<end;
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


//		bool operator==(const MRef<OPType> ptr){return objp==ptr->objp; }
//		bool operator==(OPType o){return objp==o; }
//		bool operator!=(const MRef<OPType> ptr){return objp!=ptr->objp; }

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
 * used for Memory Handling.
 */
class MObject{
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
/*		{
			int ref;
			refLock.lock();
			ref=refCount--;
			refLock.unlock();
			return ref;
		}
*/
		void incRefCount();
/*		{

			refLock.lock();
			refCount++;
			refLock.unlock();
		}
*/
		//	virtual std::string getMemObjectType(){return "(unknown)";}

		virtual std::string getMemObjectType()=0;

	private:
		int refCount;
		Mutex refLock;
};


#endif

