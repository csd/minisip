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

#ifndef _MINISIPOPENGLGUI_H
#define _MINISIPOPENGLGUI_H

#include<libmutil/CommandString.h>
#include<libminisip/signaling/sip/SipSoftPhoneConfiguration.h>
#include<libminisip/media/video/display/VideoDisplay.h>
#include<libminisip/gui/Gui.h>
#include<libmutil/Semaphore.h>

#include<string>

/**
 * A text user interface. 
 * The interface interacts with the user via the terminal, and with the <code>Sip</code> class
 * with <code>SipSMCommands</code> via the <code>MessageRouter</code>.
 */
class OpenGlGui : public Gui {
	public:
		OpenGlGui(bool fullscreen);
	
		std::string getMemObjectType() const {return "OpenGlGui";}
		
		virtual void handleCommand(const CommandString&);
		virtual CommandString handleCommandResp(std::string subsystem, const CommandString&);
		virtual void setSipSoftPhoneConfiguration(MRef<SipSoftPhoneConfiguration *>sipphoneconfig);
		virtual void setContactDb(MRef<ContactDb *>){};
		virtual bool configDialog( MRef<SipSoftPhoneConfiguration *> conf );
	
		virtual void displayErrorMessage(std::string msg);
	
		void start();
		virtual void run();

		/**
		 * Suspends calling thread until gui has stopped running
 		 */
		void waitQuit();
	
		virtual void setCallback(MRef<CommandReceiver*> callback);
	
//		virtual void keyPressed(int key);
//		virtual void guiExecute(std::string cmd);
//		virtual void guiExecute(const MRef<QuestionDialog*> &d);
//
		void join();
	
	private:
		bool startFullscreen;
		
//		void showMem();
		
//		std::string currentconfname;
		std::string currentcaller;
//		std::string input;
//		std::string callId;
//		std::string state;
		MRef<SipSoftPhoneConfiguration *> config;
//		bool autoanswer;
		MRef<Semaphore *> semSipReady;
		Thread *thread;
		MRef<Semaphore *> quitSem;
		MRef<VideoDisplay*> display;
		
		
		///indicates that the user is in a call and cannot answer any other incoming calls
//		bool inCall;

		///indicates that the TextUI is in the P2T Mode
//		bool p2tmode;
};

#endif
