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






#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H


#include"../Gui.h"
#include<libmutil/Mutex.h>
#include<libmutil/minilist.h>
#include<libmutil/MemObject.h>
#include <libglademm/xml.h>
#include <gtkmm.h>
#include <iostream>


class CallWidget;
class PhoneBookModel;
class PhoneBookTree;
class SettingsDialog;
class CertificateDialog;
class MTrayIcon;
class LogWidget;
class LogEntry;
class ImWidget;
class ContactDb;

class MainWindow : public Gui, public LogEntryHandler, public DbgHandler
#ifdef OLDLIBGLADEMM
		   ,public SigC::Object
#endif
{
	public:
		MainWindow( int argc, char ** argv );
		~MainWindow();

		bool isVisible();
		void hide();
		void show();

		virtual void run();
		virtual void quit();
		virtual void log( int type, string msg );

		virtual void handleCommand( CommandString command );
		virtual void gotPacket( int32_t i );
		virtual void displayMessage( string s, int style=-1 );
		virtual void setSipSoftPhoneConfiguration( 
				MRef<SipSoftPhoneConfiguration *> config );
		MRef<SipSoftPhoneConfiguration *>  getSipSoftPhoneConfiguration();
		virtual bool configDialog( 
				MRef<SipSoftPhoneConfiguration *> conf );

		virtual void setContactDb( MRef<ContactDb *> contactDb );

		virtual void setCallback( GuiCallback *callback );

		
		/* used to select whether a node can or cannot be selected in
		 * the tree */
		bool phoneSelect( const Glib::RefPtr<Gtk::TreeModel>& model,
                                  const Gtk::TreeModel::Path& path, bool );

		void phoneSelected();
		void runPref();
		void viewCallListToggle();

		virtual void handle( MRef<LogEntry *> );
		void removeCall( string callId );
		void removeIm( string uri );

	private:

		void hideSlot();
		void phoneTreeClicked( GdkEventButton * event );
		void im();
		void invite();
		void inviteFromTreeview( const Gtk::TreeModel::Path&,
				         Gtk::TreeViewColumn * );
		void gotCommand();
		void gotLogEntry();
		void addCall( string callId, string remoteUri, bool incoming,
			      string securityStatus="unprotected" );
		void addIm( string uri );
		void updateConfig();
		void doDisplayErrorMessage( string s );
		void runCertificateSettings();

		MRef<ContactDb *> contactDb;


		//Glib::RefPtr<Gnome::Glade::Xml>  refXml;
		Gtk::Window * mainWindowWidget;
		Gtk::Notebook * mainTabWidget;
		Gtk::TreeView * phoneBookTreeView;
		Gtk::Menu * phoneMenu;
		Gtk::MenuItem * phoneAddMenu;
		Gtk::MenuItem * phoneAddAddressMenu;
		Gtk::MenuItem * phoneRemoveMenu;
		Gtk::MenuItem * phoneEditMenu;
		Gtk::Entry * uriEntry;
		PhoneBookModel * phoneBookModel;
		PhoneBookTree * phoneBookTree;
		Glib::RefPtr<Gtk::TreeSelection> treeSelection;
		Gtk::Main kit;

		Gtk::CheckMenuItem * viewCallListMenu;

		SettingsDialog * settingsDialog;
		CertificateDialog * certificateDialog;
		MTrayIcon * trayIcon;
		LogWidget * logWidget;

		list<CallWidget *> callWidgets;
		list<ImWidget *> imWidgets;

		MRef<SipSoftPhoneConfiguration *> config;

		minilist<CommandString> commands;
		Mutex commandsLock;

		Glib::Dispatcher dispatcher;
		Glib::Dispatcher logDispatcher;
		
		minilist< MRef<LogEntry *> > logEntries;
		Mutex logEntriesLock;
};

#endif	
