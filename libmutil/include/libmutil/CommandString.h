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

#include <libmutil/libmutil_config.h>

#include<libmutil/MemObject.h>
#include<string>
#include<map>

#define LIBMUTIL_DEFAULT "LIBMUTIL_DEFAULT_RVAL_7D1PPX9Q"

class LIBMUTIL_API CommandString : public virtual MObject{
	public:
		CommandString();

		CommandString(const std::string destination_id, 
				const std::string operation, 
				const std::string parameter="", 
				const std::string parameter2="", 
				const std::string parameter3="");

		CommandString(const CommandString &c);
		
		std::string getDestinationId() const;
		void setDestinationId(std::string id);
		
		std::string getOp() const;
		void setOp(std::string op);

		std::string getParam() const;
		void setParam(std::string param);

		std::string getParam2() const;
		void setParam2(std::string param2);
		
		std::string getParam3() const;
		void setParam3(std::string param3);
		
		std::string getString() const;
                virtual std::string getMemObjectType() const;

		std::string &operator[](std::string key);

		std::string get(const std::string &key, const std::string &dfltval=LIBMUTIL_DEFAULT) const;

	private:
		std::map<std::string, std::string> keys;
};

#endif
