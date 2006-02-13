/*
  Copyright (C) 2005, 2004 Erik Eliasson, Johan Bilien
  Copyright (C) 2006 Mikael Magnusson
  
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
 *          Mikael Magnusson <mikma@users.sourceforge.net>
*/



#include<config.h>

#include<libmutil/dbg.h>

#include<iostream>

#include<libmutil/itoa.h>

Dbg mout;
Dbg merr(false);
Dbg mdbg(true, false);

//#ifdef SM_DEBUG
LIBMUTIL_API bool outputStateMachineDebug = false;
//#endif

DbgBuf::DbgBuf( DbgHandler * dbgHandler ) : 
		std::stringbuf( std::ios_base::out ),
		debugHandler( dbgHandler )
{
}

DbgBuf::~DbgBuf()
{
}

void DbgBuf::setExternalHandler( DbgHandler * dbgHandler )
{
	sync();
	debugHandler = dbgHandler;
}

int DbgBuf::sync()
{
	if( debugHandler ){
		std::string curStr = str();

		debugHandler->displayMessage(curStr, 0);
		str("");
	}
	return 1;
}

Dbg::Dbg(bool error_output, bool isEnabled):
			std::ostream(NULL), 
			error_out(error_output), 
			enabled(isEnabled), 
			external_out( false ), 
			dbgBuf( NULL ){
	updateBuf();
}

Dbg::~Dbg()
{
}

void Dbg::updateBuf()
{
	std::streambuf *buf = NULL;

	if( enabled ){
		if( error_out )
			buf = std::cerr.rdbuf();
		else if( external_out )
			buf = &dbgBuf;
		else
			buf = std::cout.rdbuf();
	}
	rdbuf( buf );
}

void Dbg::setEnabled(bool e){
	enabled = e;
	updateBuf();
}

bool Dbg::getEnabled(){
	return enabled;
}

void Dbg::setExternalHandler(DbgHandler * dbgHandler )
{
	dbgBuf.setExternalHandler( dbgHandler );
	external_out = dbgHandler != NULL;
	updateBuf();
}

std::ostream &end(std::ostream &os)
{
	return endl( os );
}

//#endif
