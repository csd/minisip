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


#ifndef COMMANDSTRING_H
#define COMMANDSTRING_H

#include<libmutil/MemObject.h>
#include<string>
#include<map>

using namespace std;

class CommandString : public MObject{
	public:
		CommandString(const string destination_id, 
				const string operation, 
				const string parameter="", 
				const string parameter2="", 
				const string parameter3="");

		CommandString(const CommandString &c);
		
		string getDestinationId();
		void setDestinationId(string id);
		
		string getOp();
		void setOp(string op);

		string getParam();
		void setParam(string param);

		string getParam2();
		void setParam2(string param2);
		
		string getParam3();
		void setParam3(string param3);
		
		string getString();
                virtual std::string getMemObjectType(){return "CommandString";}

		string &operator[](string key);

	private:
//		string destination_id;
//		string op;
//		string param;
//		string param2;
//		string param3;
		
		map<string, string> keys;
};

class CommandStringReceiver{
	public:
		virtual void handleCommand(const CommandString &)=0;
};


#endif
