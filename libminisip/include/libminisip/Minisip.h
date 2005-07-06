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




#ifndef MINISIP_H
#define MINISIP_H

#ifdef _MSC_VER
#ifdef LIBMINISIP_EXPORTS
#define LIBMINISIP_API __declspec(dllexport)
#else
#define LIBMINISIP_API __declspec(dllimport)
#endif
#else
#define LIBMINISIP_API
#endif


#include<config.h>
#include<libminisip/ConferenceControl.h>
#include<libmutil/MemObject.h>
#include<libmutil/Thread.h>
#include<libmutil/StateMachine.h>

class MediaHandler;
class Gui;
class SipSoftPhoneConfiguration;
class Sip;
class SipSMCommand;
class ConsoleDebugger;
class ConferenceControl;


class LIBMINISIP_API Minisip : public Runnable{
	public:
		Minisip(MRef<Gui*> gui, int argc, char**argv );
		virtual ~Minisip();
		
		std::string getMemObjectType(){return "Minisip";}

		void exit();
		void startSip();
		void runGui();

		//void setGui(MRef<Gui *> gui);
	private:
		
		virtual void run();
		
		void initParseConfig();

		std::string conffile;
		MRef<MediaHandler *> mediaHandler;
		MRef<Gui *> gui;
		MRef<SipSoftPhoneConfiguration *> phoneConf;
		MRef<Sip *> sip;
#ifdef GTK_GUI
#ifdef DEBUG_OUTPUT
		MRef<ConsoleDebugger *> consoleDbg;
#endif
#endif
};

#endif
