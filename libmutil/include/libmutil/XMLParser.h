/*
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Library General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

/* Copyright (C) 2004 
 *
 * Authors: Erik Eliasson <eliasson@it.kth.se>
 *          Johan Bilien <jobi@via.ecp.fr>
*/

#ifndef XMLPARSER_H
#define XMLPARSER_H

#ifdef _MSC_VER
#define int32_t __int32
#else
#include<stdint.h>
#endif

#include<list>

#ifdef DEBUG_OUTPUT
#include<iostream>
#endif

#define XML_NODE_TYPE_ROOT	 	0
#define XML_NODE_TYPE_ATTRIBUTE 	1
#define XML_NODE_TYPE_ELEMENT		2

#include<string>

using namespace std;

/*
In the following example "name" is a attribute and FILE is an element
   
<SETTINGS>
  <PROJECT name="tstproject">		 get(SETTINGS/PROJECT/name) -> tstproject
    <FILE name="tstfile1.txt" type="1"/> get(SETTINGS/PROJECT/FILE/type) -> 1
    <FILE name="tstfile1.txt" type="" /> get(SETTINGS/PROJECT/FILE[1]/name) -> tstfile1.txt
    <text>
    	Testtext			 getEnclosed(SETTINGS/PROJECT/text) -> Testtext
    </text>
  </PROJECT>
</SETTINGS>
*/


class XMLElementNotFound{
	public:
		XMLElementNotFound(string msg){this->msg=msg;};
		string what(){return msg;};
	private:
		string msg;
};

class XMLParserCallback{
	public:
		virtual bool parsedElement(string path, string enclosedText)=0;
		virtual bool parsedAttribute(string path, string value)=0;
};

class XMLNode;

class XMLParser{
	public:
		XMLParser(XMLParserCallback *cb=NULL);
		~XMLParser();
		
		string getValue(string path);
		string getValue(string path, string defaultValue);

		int32_t getIntValue(string path);
		int32_t getIntValue(string path, int32_t defaultValue);

		void print();
		
		string xmlstring();
		
		void addValue(string elementPath, string value);
		void changeValue(string elemPath, string value, bool addIfMissing=true);
		
	protected:
		void addValue(XMLNode *root, const char *elementPath, string &value, int32_t start=0);
		void parsestring(const string &s);
		
		XMLParserCallback *callback;
		XMLNode *root;
};

class XMLFileParser : public XMLParser{
	public:
		XMLFileParser(string filename="", XMLParserCallback *cb=NULL);
		void saveToFile(string file="");
	private:
		string filename;
};

class XMLstringParser : public XMLParser{
	public: 
		XMLstringParser(const string &s, XMLParserCallback *cb=NULL);
};

class XMLNode{
	public:
		XMLNode(int32_t type, string name, string value="");
		virtual ~XMLNode();

		int32_t getType(){return type;};

		void addNode(XMLNode *node);

		virtual void print(){
//#ifdef DEBUG_OUTPUT			
//			cerr << "root";
//#endif
		
		};

		std::list<XMLNode *>& getNodes();
		string getName(){return name;};
		string getValue(){return value;}
		void setValue(string v){value=v;}
		static XMLNode *getNode(XMLNode *searchNode, const char *path, int32_t start);

		static string generatestring(int32_t index, XMLNode *cur);

		std::list<XMLNode *> subnodes;
	protected:
		string name;
		string value;
		int32_t type;
	private:
};


class XMLAttribute : public XMLNode {
	public:
		XMLAttribute(string name, string value);
		~XMLAttribute();
		

//		virtual void print();
	private:
};

class XMLElement : public XMLNode {
	public:
		XMLElement(string name);
		~XMLElement();
		string getEnclosed();
		void setEnclosed(string encl);
	private:

};

#endif
