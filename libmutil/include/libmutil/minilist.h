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


#ifndef MINILIST_H
#define MINILIST_H

#include<assert.h>

#include<iostream>

#define MINILIST_FORWARD_ITERATOR_OPTIMIZE 1
//#undef MINILIST_FORWARD_ITERATOR_OPTIMIZE

#ifdef _MSC_VER
#ifdef LIBMUTIL_EXPORTS
#define LIBMUTIL_API __declspec(dllexport)
#else
#define LIBMUTIL_API __declspec(dllimport)
#endif
#else
#define LIBMUTIL_API
#endif


//Node for single linked list
template<class T>
class LIBMUTIL_API node{
	public:
		node(T v, node *next=NULL):value(v), next(next){}
		node *getNext(){return next;}
		void setNext(node *n){next = n;}
		T getValue(){return value;}
		
	private:
		T value;
		node *next;
		
};

template<class T>
class LIBMUTIL_API minilist{
	public:
		
		minilist():head(NULL),end(NULL),nelem(0)
#ifdef MINILIST_FORWARD_ITERATOR_OPTIMIZE
			,last_index(-2)
#endif
			{}

		minilist(const minilist &l2)
#ifdef MINILIST_FORWARD_ITERATOR_OPTIMIZE
			:last_index(-2)
#endif
		{
			nelem = l2.nelem;
			node<T> *l2cur=l2.head;
			node<T> *last=NULL;
			
			for (int i=0; i< nelem; i++){
				node<T> *tmp = new node<T>( l2cur->getValue(), l2cur->getNext() ) ;
				if (i==0)
					head = tmp;
				if (i==nelem-1)
					end = tmp;
				if (last!=NULL)
					last->setNext(tmp);
				last = tmp;
				l2cur = l2cur->getNext();
			}
		}
		
		~minilist(){
			empty();
		}

		void empty(){
			node<T> *cur = head;
			while (cur){
				node<T> *tmp = cur;
				cur = cur->getNext();
				delete tmp;
			}
		}

		void push_front(T item){
#ifdef MINILIST_FORWARD_ITERATOR_OPTIMIZE
			if (last_index>=0)
				last_index++;
#endif
			nelem++;
			node<T> *n = new node<T>(item, NULL);
			n->setNext(head);
			head = n;
			if (end==NULL)
				end=n;
		}
	
		void push_back(T item){
			nelem++;
			node<T> *n = new node<T>(item, NULL);
			if (head==NULL){
				head=end=n;
			}else{
				end->setNext(n);
				end = n;
			}
		}

		T pop_back(){
			assert(size()>0);
#ifdef MINILIST_FORWARD_ITERATOR_OPTIMIZE
			if (last_index>= size()-1)
				last_index=-2;
#endif
			nelem--;
			T ret = end->getValue();
			if (head==end){
				delete head;
				head = end = NULL;
				return ret;
			}
			node<T> *cur = head;
			for (int i=0; i<nelem-1; i++)
				cur = cur->getNext();
			cur->setNext(NULL);
			delete end;
			end = cur;
			return ret;
		}

		void insert(int i, T item){
#ifdef MINILIST_FORWARD_ITERATOR_OPTIMIZE
			last_index=-2;
#endif
			assert(i<=size());
			
			if (size()==0 && i==0){ // only item
				nelem++;
				node<T> *n = new node<T>(item, NULL);
				head = end = n;
				validate();
				return;
			}

			if (i==0){	// first, not only
				nelem++;
				node<T> *n = new node<T>(item, head);
//				node<T> *old_first = head;
				head = n;
				validate();
				return;
			}

			if (i==size()){ // last, not only
				nelem++;
				node<T> *n = new node<T>(item, NULL);
				end->setNext(n);
				end = n;
				return;
			}else{
				node<T> *cur =head;
				for (int j=0; j<i-1; j++)
					cur = cur->getNext();

				nelem++;
				node<T> *n = new node<T>(item, cur->getNext() );
				cur->setNext(n);
			}
			validate();
		}

		void remove(T val){
			for (int i=0; i<size(); i++){
				if (val == (*this)[i]){
					remove(i);
					i=0;
				}
			}

		}
			
		void remove(int index){
			assert(size()>0 && index>=0 && index<size());
			
#ifdef MINILIST_FORWARD_ITERATOR_OPTIMIZE
			last_index=-2;
#endif
			
			//only item
			if (index==0 && size()==1){
				nelem--;
				end=NULL;
				delete head;
				head=NULL;
				return;
			}
			
			//if first item
			if (index==0){
				nelem--;
				node<T> *tmp=head;
				head = head->getNext();
				delete tmp;
				return;
			}
			//middle or last
			node<T> *cur=head;
			node<T> *prev=head;
			for (int i=0; i<index; i++){
				prev = cur;
				cur=cur->getNext();
			}
			prev->setNext( cur->getNext() );
			if (cur==end)
				end=prev;
			nelem--;
			delete cur;
		}

		void validate(){
			node<T> *cur = head;
			int i;
			
			for (i=0; i<nelem; i++){
				if (i==nelem-1){
					assert(cur->getNext()==NULL);
				}
				cur= cur->getNext();
			}
			for (i=0; i<size(); i++){
				T t = (*this)[i];
			}
			for (i=nelem-1; i>=0; i--){
				T t = (*this)[i];
			}
			
			cur = head;
			int n=0;
			if (nelem>0){
				while (cur!=end){
					n++;
					cur= cur->getNext();
				};
				assert(n==nelem-1);
			}else{
				assert(head==NULL && end==NULL);
			}
			cur = head;
			n=0;
			if (nelem>0){
				while (cur!=NULL){
					n++;
					cur= cur->getNext();
				};
				assert(n==nelem);
			}else{
				assert(head==NULL && end==NULL);
			}

		}

		minilist &operator=(const minilist &l2) {
			nelem = l2.nelem;
			node<T> *l2cur=l2.head;
			node<T> *last=NULL;
			
			for (int i=0; i< nelem; i++){
				node<T> *tmp = new node<T>( l2cur->getValue(), l2cur->getNext() ) ;
				if (i==0)
					head = tmp;
				if (i==nelem-1)
					end = tmp;
				if (last!=NULL)
					last->setNext(tmp);
				last = tmp;
				l2cur = l2cur->getNext();
			}
			return *this;
		}

		
		T operator[](int i){
			node<T> *cur = head;
			assert(i>=0 && i<size());
			
			int j=0;
#ifdef MINILIST_FORWARD_ITERATOR_OPTIMIZE
			if (last_index>=0 && i>=last_index){
				j=last_index;
				cur=last_node;
			}
#endif
			for ( ; j<i; j++)
				cur = cur->getNext();
#ifdef MINILIST_FORWARD_ITERATOR_OPTIMIZE
			last_index=i;
			last_node=cur;
#endif
			return cur->getValue();
		}

		int size(){
			return nelem;
		}
		
	private:
		node<T> *head;
		node<T> *end;
		int nelem;
#ifdef MINILIST_FORWARD_ITERATOR_OPTIMIZE
		int last_index;
		node<T> *last_node;
#endif
};


#endif
