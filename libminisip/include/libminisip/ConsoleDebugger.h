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



#ifndef _CONSOLEDEBUGGER_H
#define _CONSOLEDEBUGGER_H

#ifdef _MSC_VER
#ifdef LIBMINISIP_EXPORTS
#define LIBMINISIP_API __declspec(dllexport)
#else
#define LIBMINISIP_API __declspec(dllimport)
#endif
#else
#define LIBMINISIP_API
#endif


#include<libmutil/MemObject.h>
#include<libminisip/SipSoftPhoneConfiguration.h>
#include<libmutil/Thread.h>
#include<string>

class LIBMINISIP_API ConsoleDebugger : public Runnable{
	public:
		ConsoleDebugger(MRef<SipSoftPhoneConfiguration *> conf):config(conf){};
		std::string getMemObjectType(){return "ConsoleDebugger";}
		void showMem();
		void showStat();
		void startThread();

		virtual void run();

	private:
		void showDialogInfo(MRef<SipDialog *> d, bool usesSM);
		MRef<SipSoftPhoneConfiguration *> config;
};

#endif
