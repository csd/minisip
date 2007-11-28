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
#include<libmutil/MemObject.h>
#include<libmutil/Exception.h>
#include<libmutil/FileSystem.h>

#include<list>

#ifdef DEBUG_OUTPUT
#include<iostream>
#endif

#define XML_NODE_TYPE_ROOT	 	0
#define XML_NODE_TYPE_ATTRIBUTE 	1
#define XML_NODE_TYPE_ELEMENT		2

#include<string>

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

class LIBMUTIL_API XMLException : public Exception{
	public:
		XMLException(std::string m):
				Exception(m.c_str()) {};
	private:
};

class LIBMUTIL_API XMLElementNotFound: public XMLException{
	public:
		XMLElementNotFound(std::string m):XMLException(m){};
};

class LIBMUTIL_API XMLFileNotFound: public XMLException{
	public:
		XMLFileNotFound(std::string m):XMLException(m){};
};


class LIBMUTIL_API XMLParserCallback{
	public:
		virtual ~XMLParserCallback() {}
		virtual bool parsedElement(std::string path, std::string enclosedText)=0;
		virtual bool parsedAttribute(std::string path, std::string value)=0;
};

class LIBMUTIL_API XMLNode;

class LIBMUTIL_API XMLParser : public MObject{
	public:
		XMLParser(XMLParserCallback *cb=NULL);
		~XMLParser();
		
		std::string getValue(std::string path);
		std::string getValue(std::string path, std::string defaultValue);

		int32_t getIntValue(std::string path);
		int32_t getIntValue(std::string path, int32_t defaultValue);

		void print();
		
		std::string xmlstring();
		
		void addValue(std::string elementPath, std::string value);
		void changeValue(std::string elemPath, std::string value, bool addIfMissing=true);

		std::string getMemObjectType(){return "XMLParser";}
		
	protected:
		void addValue(XMLNode *root, const char *elementPath, std::string &value, int32_t start=0);
		void parsestring(const std::string &s);
		
		XMLParserCallback *callback;
		XMLNode *root;
};

class LIBMUTIL_API XMLFileParser : public XMLParser{
	public:
		XMLFileParser(std::string filename="", XMLParserCallback *cb=NULL);
		XMLFileParser(std::string filename, MRef<FileSystem*> fs, XMLParserCallback *cb=NULL);
		void saveToFile(std::string file="");
	private:
		void init();

		std::string filename;
		MRef<FileSystem*> fs;
};

class LIBMUTIL_API XMLstringParser : public XMLParser{
	public: 
		XMLstringParser(const std::string &s, XMLParserCallback *cb=NULL);
};

class LIBMUTIL_API XMLNode{
	public:
		XMLNode(int32_t type, std::string name, std::string value="");
		virtual ~XMLNode();

		int32_t getType(){return type;};

		void addNode(XMLNode *node);

		virtual void print(){
		};

		std::list<XMLNode *>& getNodes();
		std::string getName(){return name;};
		std::string getValue(){return value;}
		void setValue(std::string v){value=v;}
		static XMLNode *getNode(XMLNode *searchNode, const char *path, int32_t start);

		static std::string generatestring(int32_t index, XMLNode *cur);

		std::list<XMLNode *> subnodes;
	protected:
		std::string name;
		std::string value;
		int32_t type;
	private:
};


class LIBMUTIL_API XMLAttribute : public XMLNode {
	public:
		XMLAttribute(std::string name, std::string value);
		~XMLAttribute();
		
	private:
};

class LIBMUTIL_API XMLElement : public XMLNode {
	public:
		XMLElement(std::string name);
		~XMLElement();
		std::string getEnclosed();
		void setEnclosed(std::string encl);
	private:

};

#endif
