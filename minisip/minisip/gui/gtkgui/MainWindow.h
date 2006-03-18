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


#include"GtkMainUI.h"
#include<libminisip/minisip/gui/Gui.h>
//#include"../../LogEntry.h"
#include<libmutil/MemObject.h>
#include"DtmfWidget.h"

//#include "../../../conf/ConferenceControl.h"
//#include"ConferenceWidget.h"
#include<libmutil/Mutex.h>
#include<libmutil/minilist.h>
#include <libglademm/xml.h>
#include <gtkmm.h>
#include <iostream>

class CallWidget;
class ConferenceWidget;
class PhoneBookModel;
class PhoneBookTree;
class SettingsDialog;
class CertificateDialog;

#ifndef WIN32
	class MTrayIcon;
#endif

class LogWidget;
class LogEntry;
class ImWidget;
class ContactDb;
class AccountsList;
class AccountsStatusWidget;


class MainWindow : public GtkMainUI, 
			public DtmfHandler
		#ifdef OLDLIBGLADEMM
			,public SigC::Object
		#endif
{
	public:
		MainWindow( Gtk::Main *main, std::string programDir );
		virtual ~MainWindow();

		bool isVisible();
		void hide();
		void show();

		virtual void run();
		
		/**
		Last function to execute before quitting the GUI.
		Do any clean up deemed necessary here.
		*/
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
		void viewToggle( uint8_t );

		virtual void handle( MRef<LogEntry *> );
		void removeCall( string callId );
		void removeConference( string callId );
		void removeIm( string uri );

		/**
		Use this functions to set the active tab in the notebook from
		outside the main gtk loop. This functions send a command to the 
		dispatcher, the command (inside the main loop) sets the active
		tab. All this because otherwise the Gtk::Notebook object won't 
		visually refresh.
		*/
		void setActiveTabWidget( Gtk::Widget * widget );
		void setActiveTabWidget( int pageIdx );
		
		virtual void dtmfPressed( uint8_t symbol );

		virtual std::string getMemObjectType(){return "MainWindow";};

		std::string getDataFileName( std::string baseName );
		
	private:

		void registerIcons();
		void hideSlot();
		void phoneTreeClicked( GdkEventButton * event );
		
		void inviteClick();
		void invite( string uri="" );
		
		void conference();
		
		void imClick( );
		void im( string uri="", string message="" );
		
		void inviteFromTreeview( const Gtk::TreeModel::Path&,
				         Gtk::TreeViewColumn * );
		void gotCommand();
		void gotLogEntry();
		
		CallWidget * addCall( string callId, 
					string remoteUri, 
					bool incoming,
			      		string securityStatus="unprotected" );
		ImWidget * addIm( string uri );
		void addConference( string confId, 
					string users, 
					string remoteUri,
					string callId, 
					bool incoming );
		
		void updateConfig();
		void doDisplayErrorMessage( string s );
		void runCertificateSettings();
		
		/**
		Event handler for the notebook on_switch_page.
		We notify all the widgets of the page change (via their
		activeWidgetChanged( bool, int ), telling them whether
		they just have become active/inactive, and the tab index
		of the currently active page.
		*/
		void onTabChange( GtkNotebookPage*, guint );

		MRef<ContactDb *> contactDb;

		int nextConfId;

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
		Gtk::Main *kit;

		Gtk::CheckMenuItem * viewCallListMenu;
		Gtk::CheckMenuItem * viewStatusMenu;

		Glib::RefPtr<AccountsList> accountsList;
		Glib::RefPtr<Gtk::IconFactory> factory;

		/**
		* This function is connected to the window close icon
		* (the cross on the top-right corner). 
		* It hides the main window and enters quit mode.
		*/
		bool on_window_close (GdkEventAny* event  );
		
		SettingsDialog * settingsDialog;
		CertificateDialog * certificateDialog;
		
		#ifndef WIN32
			MTrayIcon * trayIcon;
		#endif
		
		LogWidget * logWidget;
		AccountsStatusWidget * statusWidget;

		list<CallWidget *> callWidgets;
		list<ConferenceWidget *> conferenceWidgets;
		list<ImWidget *> imWidgets;

		MRef<SipSoftPhoneConfiguration *> config;

		minilist<CommandString> commands;
		Mutex commandsLock;

		Glib::Dispatcher dispatcher;
		Glib::Dispatcher logDispatcher;
		
		minilist< MRef<LogEntry *> > logEntries;
		Mutex logEntriesLock;

		std::string programDir;
		std::string lastDataDir;
};


#endif	
