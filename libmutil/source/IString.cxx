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

#include<config.h>
#include<libmutil/IString.h>
#include<libmutil/massert.h>
#include <string.h>

#include<iostream>

using namespace std;

StringAtom::StringAtom(char *b, int len):buf(b),n(len){
	
}

StringAtom::StringAtom(std::string s){
#ifdef _MSC_VER
	buf = _strdup(s.c_str());
#else
	buf = strdup(s.c_str());
#endif
	n = (int)s.length();
}

StringAtom::StringAtom(const StringAtom &a):n(a.n){
	buf = new char[n];
	memcpy(buf,a.buf,n);
}

StringAtom::~StringAtom(){
	massert(buf);
	delete []buf;
	buf=NULL;
	n=0;
}

std::string StringAtom::getMemObjectType() const {
	return "StringAtom";
}

char *StringAtom::getBuf() const {
	return buf;
}

int StringAtom::getLength() const {
	return n;
}

IString::IString(MRef<StringAtom*> a) : atom(a), start(0){
	n = a->getLength();
}

IString::IString(MRef<StringAtom*> a, int startIndex, int length) : atom(a), start(startIndex), n(length){
	massert( startIndex+length <= a->getLength() );
}

IString::IString(const IString &s){
	atom = s.atom;
	start = s.start;
	n = s.n;
}

IString::~IString(){

}

std::string IString::getMemObjectType() const {
	return "IString";
}

MRef<IString*> IString::trim(){
	int newstart, newn;
	int oldend= start+n;
	newstart=start;
	newn=n;

	int bufsize = atom->getLength();
	char *buf = atom->getBuf();

	while ((buf[newstart]==' ' || buf[newstart]=='\n' || buf[newstart]=='\t' || buf[newstart]=='\r') && newstart<oldend && newstart<bufsize){
		newstart++;
		newn--;
	}
	
	while ( (buf[newstart+newn-1]==' ' || buf[newstart+newn-1]=='\n' || buf[newstart+newn-1]=='\t' || buf[newstart+newn-1]=='\r') && newn>0 ){
		newn--;
	}

	MRef<IString*> ret = new IString(atom, newstart,newn);
	return ret;
	
}

MRef<IString*> IString::substr(int i){
	massert(i<=n); //if i==n the result will be an empty string
	MRef<IString*> ret = new IString(atom, start+i,n-i);
	return ret;
}

MRef<IString*> IString::substr(int i, int newn){
	MRef<IString*> ret = new IString(atom, start+i ,newn);
	return ret;
}

struct strptr IString::getStringPointer() const {
	struct strptr p;
	p.str_start = atom->getBuf()+start;
	p.n = n;
	return p;
}

std::string IString::cpp_str(){
	std::string ret(atom->getBuf()+start, n);
	return ret;
}


