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

#include<libmutil/XMLParser.h>
#include<fstream>
#include<libmutil/stringutils.h>
#include<iostream>
#include<sys/types.h>
#include <stdlib.h>

using namespace std;

static bool is_blank(char c){
	if (c=='\t' || c==' ' || c=='\r' || c=='\n')
		return true;
	else
		return false;
			
}

static int32_t skipws(const char *s, int32_t i){
	while (is_blank(s[i]))
		i++;
	return i;
}

static string parseWord(const char *s, int32_t &i){
	i = skipws(s,i);
	string word;

	if (s[i]=='\"'){
		i++;
		while (s[i]!='\"')
			word = word + s[i++];
		i++;
	}else{
		while ( ! ( is_blank(s[i]) || s[i]=='<' || s[i]=='/' || s[i]=='>' || s[i]=='=' || s[i]=='\"' || s[i]==0))
			word+=s[i++];
	}
	
	return word;
	
}

XMLParser::XMLParser(XMLParserCallback *cb):callback(cb),root(NULL){

}

XMLParser::~XMLParser(){
	if (root)
		delete root;
}

void XMLParser::changeValue(string elementPath, string value, bool addIfMissing){
	XMLNode *n = XMLNode::getNode(root, elementPath.c_str(), 0);
	if (n!=NULL)
		n->setValue(value);
	else{
		if (addIfMissing)
			addValue(root, elementPath.c_str(), value);
	}

}

void XMLParser::addValue(string path, string value){
	addValue(root,path.c_str(),value,0);
}

void XMLParser::addValue(XMLNode *cur, const char *path, string &value, int32_t i){
//	cerr << "addValue with path="<<path<< " and i=" << i << " path+i="<<path+i<< endl;
	if (path[i]=='/')
		i++;
	if (path[i]==0){
		cur->setValue(value);
		return;
	}

	int32_t index=0;
	string part = parseWord(path, i);
	if (part[part.length()-1]==']'){
		string sindex;
		int32_t ii=(int32_t)part.length()-2;
		while (part[ii]!='['){
			sindex= part[ii]+sindex;
			ii--;
			if (i==0)
				throw XMLException("Parse error in key: Missing '['?");
		}
		part = part.substr(0,ii);
		index = atoi(sindex.c_str());
	}
	
//	cerr<<"In addValue with index="<<index<<", part ="<<part<<" and cur->getname="<<cur->getName()<<" and i="<< i<<" path+i="<<path+i<<endl;
	
	for (list<XMLNode *>::iterator itt = cur->subnodes.begin(); itt!= cur->subnodes.end(); itt++)
		if ( (*itt)->getName()==part )
			if ((index--)==0) {
				addValue( *itt, path, value, i);
				return;
			}
//	cerr << "WARNING: node not found"<< endl;
	XMLNode *newnode = new XMLElement(part);
	cur->addNode(newnode);
	addValue(newnode, path, value, i/*+part.length()*/);
	return;
}

string XMLParser::getValue(string path){
	XMLNode *cur = root;

//	cerr << "Searching for path "+path<< endl;
	cur = XMLNode::getNode(root, path.c_str(), 0);
	if (cur==NULL)
		throw XMLElementNotFound("Element does not exist: "+path);
	//	throw new XMLElementNotFound("Element does not exist: "+path);
	return cur->getValue();
}

string XMLParser::getValue(string path, string defaultValue){
	XMLNode *cur = root;

//	cerr << "Searching for path "+path<< endl;
	cur = XMLNode::getNode(root, path.c_str(), 0);
	if (cur==NULL)
		return defaultValue;
	return cur->getValue();
}

int32_t XMLParser::getIntValue(string path){
	return atoi(getValue(path).c_str());
}

int32_t XMLParser::getIntValue(string path, int32_t defaultValue){
	return atoi(getValue(path, itoa(defaultValue)).c_str());
}




string XMLNode::generatestring(int32_t indent, XMLNode *cur){
	string ret;
	int32_t j;
	for (j=0; j<indent; j++)
		ret = ret+"\t";
	if (indent>=0)
		ret=ret+"<"+cur->getName();
	
	for (list<XMLNode *>::iterator i=cur->subnodes.begin(); i!=cur->subnodes.end(); i++)
		if ((*i)->getType()==XML_NODE_TYPE_ATTRIBUTE){
			ret=ret+" "+(*i)->getName()+"=\""+(*i)->getValue()+"\"";
		}
	if (indent>=0)
		ret = ret+">\n";
	if (cur->getValue().length()>0){
		for (j=0; j<indent+1; j++)
			ret = ret+"\t";
		ret = ret +cur->getValue()+"";
		ret = ret+'\n';
	}
	bool hassub=false;
	for (list<XMLNode *>::iterator itt=cur->subnodes.begin(); itt!=cur->subnodes.end(); itt++)
		if ((*itt)->getType()==XML_NODE_TYPE_ELEMENT){
			ret = ret + generatestring(indent+1, *itt);
			hassub=true;
		}
	
	for (j=0; j<indent; j++)
		ret = ret+"\t";
	
	if (indent>=0)
		ret = ret +"</"+cur->getName()+">\n";

	return ret;
}

string XMLParser::xmlstring(){
	
	return XMLNode::generatestring(-1, root);

}

XMLFileParser::XMLFileParser(string filename_, XMLParserCallback *cb):
		XMLParser(cb), 
		filename(filename_)
{

	fs = new LocalFileSystem();
	init();
}

XMLFileParser::XMLFileParser(string filename_, MRef<FileSystem*> fs_, XMLParserCallback *cb) : 
	XMLParser(cb), 
	filename(filename_), fs(fs_)
{
	init();
}

void XMLFileParser::init(){
	
	string s = "";
	if (filename != ""){
		MRef<File*> file;
		try{
			file = fs->open(filename);
		}catch(const FileException &e){
			throw XMLFileNotFound( "Could not read file " + filename );
		}


		int32_t bufsize=20; 
		char *buf = (char *)calloc(bufsize,1);
		do{
			for (int32_t i=0; i<bufsize; i++)
				buf[i]=0;
			//file.read(buf,bufsize-1);
			file->read(buf, bufsize-1);
			s = s+string(buf);
		}while(!file->eof());
		free(buf);

	}
	parsestring(s);
}


static XMLNode *parseAttribute(const char *s, int32_t &i){
//	cerr << "parseAttribute with s="<<s+i<<" and i=" << i <<endl;
	i = skipws(s,i);
	string name = parseWord(s,i);
	i = skipws(s,i);
	if (s[i]!='=')
		cerr << "Error: attribute "<< name << " has no value."<< endl;

	i++;
	i=skipws(s,i);

	string value = parseWord(s,i);
	i=skipws(s,i);
	return new XMLAttribute(name, value);
}



static XMLNode *parseElement(const char *s, int32_t &i){
	i = skipws(s, i);
	if (s[i]=='<' && s[i+1]=='!' && s[i+2]=='-' && s[i+2]=='-'){
		while (s[i]!='>')
			i++;
		i++;
		return parseElement(s, i);
	}
	
	if (s[i]=='<' && s[i+1]=='?'){
		while (s[i]!='>')
			i++;
		i++;
		return parseElement(s, i);
	}

	if (s[i]=='<' && s[i+1]=='!'){
		while (s[i]!='>')
			i++;
		i++;
		return parseElement(s, i);
	}
	
	//cerr << "parseElement with s="<<s+i<<" and i=" << i <<endl;
	if (s[i]==0)
		return NULL;
	if (s[i]=='<'){
		i++;
		i = skipws(s, i);
	}
	string name = parseWord(s,i);
	
	//cerr<<"Parsed word <"<<name <<"> - creating XMLElement."<< endl;
	XMLElement *elem = new XMLElement(name);

	i=skipws(s, i);
	
	while (! (s[i] == '/' || s[i]=='>')){
		elem->addNode( parseAttribute(s, i) );

		i=skipws(s, i);
	}
			
	if (s[i]=='/' && s[i+1]=='>'){
		i+=2;
		return elem;
	}

	i++; // pass '>'

	i=skipws(s,i);
	while (!(s[i]=='<' && s[i+1]=='/')){
		if (s[i]=='<')
			elem->addNode( parseElement(s, i));
		else{
			string encl;
			while (s[i]!='<')
				encl+=s[i++];
			elem->setEnclosed(trim(encl));
		}
		
		i=skipws(s,i);
	}
	if (s[i]=='<')
		i++;
	if (s[i]=='/')
		i++;
	i=skipws(s, i);
	string endname = parseWord(s,i);
	if (name!=endname){
		cerr << "ERROR: XML format error - closing tag does not match ("<<name<<"!="<<endname<<")"<< endl;

	}
	i=skipws(s,i);
	if (s[i]!='>')
		cerr << "ERROR: XML format error - closing tag contains attribute"<<endl;

	i++;
	return elem;
}


void XMLParser::parsestring(const string &s){
	int32_t i=0;
	root = new XMLNode(XML_NODE_TYPE_ROOT,"root");
	
	XMLNode *n;
	
	while ( (n = parseElement(s.c_str(),i))     ){
		root->addNode(n);
	}
}

XMLstringParser::XMLstringParser(const string &s, XMLParserCallback *cb) 
		: XMLParser(cb)
{
	parsestring(s);
}

void XMLFileParser::saveToFile(string fname){
	if (fname=="")
		fname = filename;
	ofstream file(fname.c_str());
	
	if (!file){
		throw XMLFileNotFound( "Could not write file " + fname );
	}

	file << xmlstring();
}


XMLNode *XMLNode::getNode(XMLNode *cur, const char *path, int32_t i){
	if (path[i]=='/')
		i++;
	if (path[i]==0)
		return cur;
	int32_t index=0;
	string part = parseWord(path, i);
	if (part[part.length()-1]==']'){
		string sindex;
		int32_t ii=(int32_t)part.length()-2;
		while (part[ii]!='['){
			sindex= part[ii]+sindex;
			ii--;
		}
		part = part.substr(0,ii);
		index = atoi(sindex.c_str());
	}
	
//	cerr<<"In getNode with index="<<index<<", part ="<<part<<" and cur->getname="<<cur->getName()<<" and i="<< i<<" path+i="<<path+i<<endl;
	
	for (list<XMLNode *>::iterator itt = cur->subnodes.begin(); itt!= cur->subnodes.end(); itt++)
		if ( (*itt)->getName()==part )
			if ((index--)==0)
				return getNode( *itt, path, i);
			
//	cerr << "WARNING: node not found"<< endl;
	return NULL;
}

list<XMLNode *> &XMLNode::getNodes(){
	return subnodes;
}

static void xmlprint(string path, XMLNode *n){
	if (!n)
		cerr << "NULL"<< endl;

	
//	cerr << "Path "<<path+"/"+n->getName() << ": "; 
	n->print();
	cerr << endl;

	list<XMLNode *>nodes = n->getNodes();

	list<XMLNode *>::iterator i;
	for (i = nodes.begin(); i!= nodes.end(); i++)
		xmlprint(path+"/"+n->getName(), *i);
	
}

void XMLParser::print(){
	xmlprint("", root);
}

XMLNode::XMLNode(int32_t type_, string name_, string value_):name(name_), value(value_),type(type_){

}

XMLNode::~XMLNode(){
	for (list<XMLNode *>::iterator i= subnodes.begin(); i!=subnodes.end(); i++)
		if (*i)
			delete (*i);
}

XMLAttribute::~XMLAttribute(){
}

XMLElement::~XMLElement(){
}

void XMLNode::addNode(XMLNode *n){
	subnodes.push_back(n);
}

XMLElement::XMLElement(string name_):XMLNode(XML_NODE_TYPE_ELEMENT, name_){
	
}

string XMLElement::getEnclosed(){
	return value;
}

void XMLElement::setEnclosed(string s){
	value= s;
}

/*
void XMLElement::print(){
#ifdef DEBUG_OUTPUT
	cerr << "name="<< name <<"; value="<< value;
#endif

}
*/
XMLAttribute::XMLAttribute(string name_, string value_):XMLNode(XML_NODE_TYPE_ATTRIBUTE,name_,value_){

}

/*
void XMLAttribute::print(){
#ifdef DEBUG_OUTPUT
	cerr << "name="<< name<<"; value="<< value;
#endif

}
*/
