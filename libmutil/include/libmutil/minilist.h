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

#include <libmutil/libmutil_config.h>

#include<libmutil/massert.h>

#include<iostream>

#define MINILIST_FORWARD_ITERATOR_OPTIMIZE 1
//#undef MINILIST_FORWARD_ITERATOR_OPTIMIZE

//#define MINILIST_VALIDATE

//Node for single linked list
template<class T>
class MiniListNode{
	public:
		MiniListNode( T v, MiniListNode *nxt=NULL):value(v), next(nxt){}
		MiniListNode * getNext(){ return next;} 
		void setNext( MiniListNode *n ){ next = n; }
		T getValue(){return value;}
		
	private:
		T value;
		MiniListNode *next;
		
};

/**
 * Template implementation of a signle-linked list. If
 * MINILIST_FORWARD_ITERATOR_OPTIMIZE is defined, then
 * iterating over the list is O(1) in each access instead 
 * of the expected O(n).
 * (mylist[x] -> x-lastx iterations iff x>lastx)
*/
template<class T>
class minilist{
	public:
		
		/**
		 * Creates an empty list
		 */
		minilist():head(NULL),end(NULL),nelem(0)
#ifdef MINILIST_FORWARD_ITERATOR_OPTIMIZE
			,last_index(-2),last_node(NULL)
#endif
			{}

		/**
		 * Copy constructor that initializes a list to be equeal
		 * to another list.
		 * Complexity: O(n) where n is the number of items in the
		 * list.
		 */
		minilist(const minilist &l2)
#ifdef MINILIST_FORWARD_ITERATOR_OPTIMIZE
			:last_index(-2),last_node(NULL)
#endif
		{
			head=end=NULL;
			nelem = l2.nelem;
			MiniListNode<T> *l2cur=l2.head;
			MiniListNode<T> *last=NULL;
			
			for (int i=0; i< nelem; i++){
				MiniListNode<T> *tmp = new MiniListNode<T>( l2cur->getValue(), NULL ) ;
				if (i==0)
					head = tmp;
				if (i==nelem-1)
					end = tmp;
				if (last!=NULL) // last==NULL on first itteration
					last->setNext(tmp);
				last = tmp;
				l2cur = l2cur->getNext();
			}
		}
		
		/**
		 * Frees all allocated space by calling the empty() method.
		 */
		~minilist(){
			empty();
		}
		
		/**
		 * Complexity: O(n) where n is the number of elements in
		 * the list.
		 */
		void empty(){
			MiniListNode<T> *cur = head;
			while (cur){
				MiniListNode<T> *tmp = cur;
				cur = cur->getNext();
				delete tmp;
			}
			nelem=0;
			head=NULL;
			end=NULL;
		}

		/**
		 * Inserts an element into the beginning of the list.
		 * Complexity: O(1)
		 */
		void push_front(T item){
#ifdef MINILIST_FORWARD_ITERATOR_OPTIMIZE
			if (last_index>=0)
				last_index++;
#endif
			nelem++;
			MiniListNode<T> *n = new MiniListNode<T>(item, NULL);
			n->setNext(head);
			head = n;
			if (end==NULL)
				end=n;
		}
	
		/**
		 * Adds an element to the end of the list.
		 * Complexity: O(1)
		 */
		void push_back(T item){
			nelem++;
			MiniListNode<T> *n = new MiniListNode<T>(item, NULL);
			if (head==NULL){
				head=end=n;
			}else{
				end->setNext(n);
				end = n;
			}
		}

		/**
		 * Removes and returns the last element in the list.
		 * Complexity: O(n)
		 * TODO: Improve performance by using the "last_index".
		 */
		T pop_back(){
			massert(size()>0);
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
			MiniListNode<T> *cur = head;
			for (int i=0; i<nelem-1; i++)
				cur = cur->getNext();
			cur->setNext(NULL);
			delete end;
			end = cur;
			return ret;
		}

		/**
		 * Inserts an item at the i:th position.
		 * Complexity: O(n)
		 */
		void insert(int i, T item){
#ifdef MINILIST_FORWARD_ITERATOR_OPTIMIZE
			last_index=-2;
#endif
			massert(i<=size());
			
			if (size()==0 && i==0){ // only item
				nelem++;
				MiniListNode<T> *n = new MiniListNode<T>(item, NULL);
				head = end = n;
#ifdef MINILIST_VALIDATE
				validate();
#endif
				return;
			}

			if (i==0){	// first, not only
				nelem++;
				MiniListNode<T> *n = new MiniListNode<T>(item, head);
//				MiniListNode<T> *old_first = head;
				head = n;
#ifdef MINILIST_VALIDATE
				validate();
#endif
				return;
			}

			if (i==size()){ // last, not only
				nelem++;
				MiniListNode<T> *n = new MiniListNode<T>(item, NULL);
				end->setNext(n);
				end = n;
				return;
			}else{
				MiniListNode<T> *cur =head;
				for (int j=0; j<i-1; j++)
					cur = cur->getNext();

				nelem++;
				MiniListNode<T> *n = new MiniListNode<T>(item, cur->getNext() );
				cur->setNext(n);
			}
#ifdef MINILIST_VALIDATE
			validate();
#endif
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
			massert(size()>0 && index>=0 && index<size());
			
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
				MiniListNode<T> *tmp=head;
				head = head->getNext();
				delete tmp;
				return;
			}
			//middle or last
			MiniListNode<T> *cur=head;
			MiniListNode<T> *prev=head;
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

#ifdef MINILIST_VALIDATE
		void validate() const{
			MiniListNode<T> *cur = head;
			int i;
			
			for (i=0; i<nelem; i++){
				if (i==nelem-1){
					massert(cur->getNext()==NULL);
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
				massert(n==nelem-1);
			}else{
				massert(head==NULL && end==NULL);
			}
			cur = head;
			n=0;
			if (nelem>0){
				while (cur!=NULL){
					n++;
					cur= cur->getNext();
				};
				massert(n==nelem);
			}else{
				massert(head==NULL && end==NULL);
			}

		}
#endif

		minilist &operator=(const minilist &l2) {
			nelem = l2.nelem;
			MiniListNode<T> *l2cur=l2.head;
			MiniListNode<T> *last=NULL;
			
			for (int i=0; i< nelem; i++){
				MiniListNode<T> *tmp = new MiniListNode<T>( l2cur->getValue(), l2cur->getNext() ) ;
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

		
		T operator[](int i) const{
			MiniListNode<T> *cur = head;
			massert(i>=0 && i<size());
			
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

		int size() const{
			return nelem;
		}
		
	private:
		MiniListNode<T> *head;
		MiniListNode<T> *end;
		int nelem;
#ifdef MINILIST_FORWARD_ITERATOR_OPTIMIZE
		mutable int last_index;
		mutable MiniListNode<T> *last_node;
#endif
};


#endif
