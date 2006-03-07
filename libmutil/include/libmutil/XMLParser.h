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


#ifndef XMLPARSER_H
#define XMLPARSER_H

#include <libmutil/libmutil_config.h>

#include<libmutil/mtypes.h>

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


/**
 * TODO: Actually implement XML support ;) 
 */

class LIBMUTIL_API XMLException{
	public:
		XMLException(string msg){this->msg=msg;};
		string what(){return msg;};
	private:
		string msg;
};

class LIBMUTIL_API XMLElementNotFound: public XMLException{
	public:
		XMLElementNotFound(string msg):XMLException(msg){};
};

class LIBMUTIL_API XMLFileNotFound: public XMLException{
	public:
		XMLFileNotFound(string msg):XMLException(msg){};
};


class LIBMUTIL_API XMLParserCallback{
	public:
		virtual ~XMLParserCallback() {}
		virtual bool parsedElement(string path, string enclosedText)=0;
		virtual bool parsedAttribute(string path, string value)=0;
};

class LIBMUTIL_API XMLNode;

class LIBMUTIL_API XMLParser{
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

class LIBMUTIL_API XMLFileParser : public XMLParser{
	public:
		XMLFileParser(string filename="", XMLParserCallback *cb=NULL);
		void saveToFile(string file="");
	private:
		string filename;
};

class LIBMUTIL_API XMLstringParser : public XMLParser{
	public: 
		XMLstringParser(const string &s, XMLParserCallback *cb=NULL);
};

class LIBMUTIL_API XMLNode{
	public:
		XMLNode(int32_t type, string name, string value="");
		virtual ~XMLNode();

		int32_t getType(){return type;};

		void addNode(XMLNode *node);

		virtual void print(){
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


class LIBMUTIL_API XMLAttribute : public XMLNode {
	public:
		XMLAttribute(string name, string value);
		~XMLAttribute();
		
	private:
};

class LIBMUTIL_API XMLElement : public XMLNode {
	public:
		XMLElement(string name);
		~XMLElement();
		string getEnclosed();
		void setEnclosed(string encl);
	private:

};

#endif
