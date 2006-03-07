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


#ifndef _DBG_H
#define _DBG_H

#include <libmutil/libmutil_config.h>

#include<string>
#include<iostream>
#include<sstream>

#define DBG_INFO 0
#define DBG_ERROR 1

//==================================================================
//if not win ce, pocket pc ... inherit from iostream and stringbuf
//otherwise, use an old version of the debug class (release 1923)
#ifndef _WIN32_WCE

	class LIBMUTIL_API DbgHandler {
		public:
			virtual ~DbgHandler(){}
		private:
			virtual void displayMessage(std::string output,int style=-1)=0;
			friend class Dbg;
			friend class DbgBuf;
	};

	class LIBMUTIL_API DbgBuf : public std::stringbuf {
		public:
			DbgBuf( DbgHandler * dbgHandler );
			virtual ~DbgBuf();
			virtual void setExternalHandler(DbgHandler * dbgHandler);
		protected:
			virtual int sync();
		private:
			DbgHandler * debugHandler;
	};

	class LIBMUTIL_API Dbg: public std::ostream {
		public:
		Dbg(bool error_output=false, bool enabled=true);
		virtual ~Dbg();
		void setEnabled(bool enabled);
		bool getEnabled();
		void setExternalHandler(DbgHandler * dbgHandler);

		protected:
		void updateBuf();

		private:
		bool error_out;
		bool enabled;
		bool external_out;
		DbgBuf dbgBuf;
	};


	extern LIBMUTIL_API Dbg mout;
	extern LIBMUTIL_API Dbg merr;
	extern LIBMUTIL_API Dbg mdbg;

	extern LIBMUTIL_API bool outputStateMachineDebug;
	
	extern LIBMUTIL_API std::ostream &end(std::ostream &os);

//==================================================================
//if wince, use the old interface ... ======================
#else

	class LIBMUTIL_API DbgEndl{
		public:
			DbgEndl(){}
		private:
			int i;
	};

	class LIBMUTIL_API DbgHandler{
		public:
			virtual ~DbgHandler(){}
		private:
			virtual void displayMessage(std::string output,int style=-1)=0;
			friend class Dbg;
	};

	class LIBMUTIL_API Dbg{
		public:
			static const DbgEndl endl;
			Dbg(bool error_output=false, bool enabled=true);

			Dbg &operator<<(std::string);
			Dbg &operator<<(int);
			Dbg &operator<<(char);
			Dbg &operator<<(DbgEndl &endl);

			void setEnabled(bool enabled);
			bool getEnabled();
			void setExternalHandler(DbgHandler * dbgHandler);

		private:
			bool error_out;
			bool enabled;
			std::string str;
			DbgHandler * debugHandler;
	};

	extern LIBMUTIL_API Dbg mout;
	extern LIBMUTIL_API Dbg merr;
	extern LIBMUTIL_API Dbg mdbg;
	extern LIBMUTIL_API DbgEndl end;

	extern LIBMUTIL_API bool outputStateMachineDebug;

#endif

#endif

