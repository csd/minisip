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





#include"MainWindow.h"
#include"CallWidget.h"
#include"PhoneBook.h"
#include"SettingsDialog.h"
#include"CertificateDialog.h"
#ifndef WIN32
#include"TrayIcon.h"
#endif
#include"LogWidget.h"
#include"ImWidget.h"
#include"../../../sip/SipSoftPhoneConfiguration.h"
#include"../../contactdb/ContactDb.h"
#include<libmsip/SipCommandString.h>
//#include<libmsip/SipSoftPhone.h>
//
#ifdef OLDLIBGLADEMM
#define SLOT(a,b) SigC::slot(a,b)
#define BIND SigC::bind 
#else
#define SLOT(a,b) sigc::mem_fun(a,b) 
#define BIND sigc::bind
#endif

MainWindow::MainWindow( int32_t argc, char ** argv ):kit( argc, argv ){

	Gtk::Button * callButton;
	Gtk::Button * imButton;
	Gtk::MenuItem * prefMenu;
	Gtk::MenuItem * certMenu;
	Gtk::MenuItem * quitMenu;
	Gtk::MenuBar  * minisipMenubar;
	Gtk::MenuItem * addContactMenu;
	Gtk::MenuItem * addAddressContactMenu;
	Gtk::MenuItem * removeContactMenu;
	Gtk::MenuItem * editContactMenu;
	Gtk::MenuItem * callMenu;
	Gtk::MenuItem * imMenu;
	Glib::RefPtr<Gnome::Glade::Xml>  refXml;

  	try{
    		refXml = Gnome::Glade::Xml::create((string)MINISIP_DATADIR + "/minisip.glade");
  	}
  
	catch(const Gnome::Glade::XmlError& ex){
    		std::cerr << ex.what() << std::endl;
    		exit( 1 );
  	}

  	//Get the Glade-instantiated Dialog:
  	//Gtk::Widget* mainWindow = 0;
  	refXml->get_widget( "minisipMain", mainWindowWidget );

	refXml->get_widget( "mainTabWidget", mainTabWidget );

	refXml->get_widget( "phoneBookTree", phoneBookTreeView );
	
	refXml->get_widget( "phoneMenu", phoneMenu );
	refXml->get_widget( "phoneAddMenu", phoneAddMenu );
	refXml->get_widget( "phoneAddAddressMenu", phoneAddAddressMenu );
	refXml->get_widget( "phoneRemoveMenu", phoneRemoveMenu );
	refXml->get_widget( "phoneEditMenu", phoneEditMenu );
	
	refXml->get_widget( "addContactMenu", addContactMenu );
	refXml->get_widget( "addAddressContactMenu", addAddressContactMenu );
	refXml->get_widget( "removeContactMenu", removeContactMenu );
	refXml->get_widget( "editContactMenu", editContactMenu );

	refXml->get_widget( "callMenu", callMenu );
	refXml->get_widget( "imMenu", imMenu );

	treeSelection = phoneBookTreeView->get_selection();

//	treeSelection->set_select_function( SigC::slot( *this, 
//				&MainWindow::phoneSelect ) );
	
	treeSelection->signal_changed().connect(
		SLOT( *this, &MainWindow::phoneSelected ) );

	phoneBookTree = new PhoneBookTree;
	phoneBookModel = new PhoneBookModel( phoneBookTree );

	phoneBookTreeView->set_headers_visible( false );
	phoneBookTreeView->set_rules_hint( false );
	phoneBookTreeView->signal_button_press_event().connect_notify( 
		SLOT( *this, &MainWindow::phoneTreeClicked ), false );

	/* Context menu */
	phoneAddAddressMenu->signal_activate().connect(
		BIND<Glib::RefPtr<Gtk::TreeSelection>, bool >(
		SLOT( *phoneBookModel, &PhoneBookModel::addContact ),
		treeSelection, true ));
	
	phoneAddMenu->signal_activate().connect(
		BIND<Glib::RefPtr<Gtk::TreeSelection>, bool>(
		SLOT( *phoneBookModel, &PhoneBookModel::addContact ),
		treeSelection, false ));
	
	phoneRemoveMenu->signal_activate().connect(
		BIND<Glib::RefPtr<Gtk::TreeSelection> >(
		SLOT( *phoneBookModel, &PhoneBookModel::removeContact ),
		treeSelection ));
	
	phoneEditMenu->signal_activate().connect(
		BIND<Glib::RefPtr<Gtk::TreeSelection> >(
		SLOT( *phoneBookModel, &PhoneBookModel::editContact ),
		treeSelection ));
	
	/* Contact menu from the menubar */
	addAddressContactMenu->signal_activate().connect(
		BIND<Glib::RefPtr<Gtk::TreeSelection>, bool >(
		SLOT( *phoneBookModel, &PhoneBookModel::addContact ),
		treeSelection, true ));
	
	addContactMenu->signal_activate().connect(
		BIND<Glib::RefPtr<Gtk::TreeSelection>, bool>(
		SLOT( *phoneBookModel, &PhoneBookModel::addContact ),
		treeSelection, false ));
	
	removeContactMenu->signal_activate().connect(
		BIND<Glib::RefPtr<Gtk::TreeSelection> >(
		SLOT( *phoneBookModel, &PhoneBookModel::removeContact ),
		treeSelection ));
	
	editContactMenu->signal_activate().connect(
		BIND<Glib::RefPtr<Gtk::TreeSelection> >(
		SLOT( *phoneBookModel, &PhoneBookModel::editContact ),
		treeSelection ));
	
	refXml->get_widget( "callButton", callButton );

	callButton->signal_clicked().connect( SLOT( *this, &MainWindow::invite ) );
	
	callMenu->signal_activate().connect( SLOT( *this, &MainWindow::invite ) );

	phoneBookTreeView->signal_row_activated().connect( SLOT( *this, &MainWindow::inviteFromTreeview ) );

	refXml->get_widget( "imButton", imButton );

	imButton->signal_clicked().connect( SLOT( *this, &MainWindow::im ) );
	
	imMenu->signal_activate().connect( SLOT( *this, &MainWindow::im ) );
	
	certificateDialog = new CertificateDialog( refXml );
	settingsDialog = new SettingsDialog( refXml, certificateDialog );
	
	refXml->get_widget( "uriEntry", uriEntry );

	refXml->get_widget( "prefMenu", prefMenu );
	refXml->get_widget( "certMenu", certMenu );
	refXml->get_widget( "quitMenu", quitMenu );
	
	refXml->get_widget( "viewCallListMenu", viewCallListMenu );

	prefMenu->signal_activate().connect( SLOT( *settingsDialog, &SettingsDialog::show ) );
	certMenu->signal_activate().connect( SLOT( *this, &MainWindow::runCertificateSettings ) );
	quitMenu->signal_activate().connect( SLOT( *this, &MainWindow::quit ) );
	
	viewCallListMenu->signal_activate().connect( SLOT( *this, &MainWindow::viewCallListToggle ) );

	dispatcher.connect( SLOT( *this, &MainWindow::gotCommand ) );

#ifndef WIN32
	trayIcon = new MTrayIcon( this, refXml );

	mainWindowWidget->signal_hide().connect( SLOT( *this, &MainWindow::hideSlot ));
#endif
	logWidget = new LogWidget( this );

	mainTabWidget->append_page( *logWidget, "Call list" );

	logDispatcher.connect( SLOT( *this, &MainWindow::gotLogEntry ) );

	mainWindowWidget->set_sensitive( false );

}

MainWindow::~MainWindow(){
}

void MainWindow::run(){
#ifndef WIN32
	if( trayIcon != NULL )
		kit.run( *(trayIcon->getWindow()) );
	else
#endif
		kit.run( *mainWindowWidget );
}

void MainWindow::quit(){
	kit.quit();
}

bool MainWindow::isVisible(){
	return mainWindowWidget->is_visible();
}

void MainWindow::hide(){
	mainWindowWidget->hide();
}

void MainWindow::runPref(){
	settingsDialog->run();
}

void MainWindow::show(){
	mainWindowWidget->show();
}

void MainWindow::hideSlot(){
}

void MainWindow::handleCommand( CommandString command ){

	commandsLock.lock();
	commands.push_front( command );
	commandsLock.unlock();

	dispatcher.emit();

}

void MainWindow::gotCommand(){

	list<CallWidget *>::iterator i;

	commandsLock.lock();
	CommandString command = commands.pop_back();
	commandsLock.unlock();

	if( command.getOp() == "sip_ready" ){
		mainWindowWidget->set_sensitive( true );
		return;
	}
	
	if( command.getOp() == "config_updated" ){
		updateConfig();
		return;
	}

	if( command.getOp() == "error_message" ){
		doDisplayErrorMessage( command.getParam() );
		return;
	}
	
	for( i = callWidgets.begin(); i != callWidgets.end(); i++ ){
		if( (*i)->handleCommand( command ) ){
			return;
		}
	}

	if (command.getOp() == SipCommandString::incoming_im){

		list<ImWidget *>::iterator i;
		for (i = imWidgets.begin(); i != imWidgets.end(); i++ ){
			if( (*i)->handleIm( command.getParam(), command.getParam2()/*, command.getParam3()*/ ) ){
				return;
			}

		}
		addIm(command.getParam2());
		for (i = imWidgets.begin(); i != imWidgets.end(); i++ ){
			if( (*i)->handleIm( command.getParam(), command.getParam2()/*, command.getParam3()*/ ) ){
				return;
			}

		}
		
		return;
	}

	if( command.getOp() == SipCommandString::incoming_available ){
		addCall( command.getDestinationId(), command.getParam(), true,
			 command.getParam2() );
		return;
	}
	
	if( command.getOp() == SipCommandString::remote_presence_update){
		MRef<ContactEntry*> ce = contactDb->lookUp(command.getParam());
		if (ce){
			string state = command.getParam2();
			cerr << "State is: "<< state << endl;
			if (state =="online"){
				cerr << "Changed status to online "<< endl;
				ce->setOnlineStatus(CONTACT_STATUS_ONLINE);
			}else if (state=="offline")
				ce->setOnlineStatus(CONTACT_STATUS_OFFLINE);
			else
				ce->setOnlineStatus(CONTACT_STATUS_UNKNOWN);
			ce->setOnlineStatusDesc(command.getParam3());
			phoneBookTreeView->queue_draw();
			
		}else{
			cerr << "MainWindow::gotCommand: WARNING: did not find uri <"<< command.getParam()<< "> to change presence info"<< endl;
		}
		return;
	}
	
	if( command.getOp() == SipCommandString::register_sent 
			|| command.getOp() == SipCommandString::register_ok ){
		//ignore these commands
		return;
	}

	merr << "MainWindow::gotCommand: Warning: did not handle command: "<< command.getOp()<< end;
}	

void MainWindow::gotPacket( int32_t /*i*/ ){
}

void MainWindow::displayErrorMessage( string s ){
	handleCommand( CommandString( "", "error_message", s ) );
}

void MainWindow::doDisplayErrorMessage( string s ){
	Gtk::MessageDialog dialog( s, Gtk::MESSAGE_ERROR );
	dialog.run();
}

void MainWindow::setSipSoftPhoneConfiguration( 
		MRef<SipSoftPhoneConfiguration *> config ){
	
	this->config = config;

	handleCommand( CommandString( "", "config_updated" ) );
}

void MainWindow::updateConfig(){
	
	settingsDialog->setConfig( config );
	certificateDialog->setCertChain( config->securityConfig.cert );
	certificateDialog->setRootCa( config->securityConfig.cert_db );

	const Glib::RefPtr<PhoneBookModel> modelPtr( phoneBookModel );

	list< MRef<PhoneBook *> > phonebooks = config->phonebooks;
	list< MRef<PhoneBook *> >::iterator i;

	for( i = phonebooks.begin(); i != phonebooks.end(); i++ ){

        	phoneBookModel->setPhoneBook( *i );
        }
	
	Gtk::CellRendererText * renderer = new Gtk::CellRendererText();
	phoneBookTreeView->set_model( modelPtr );
	if( phonebooks.size() > 0 ){
//		  phoneBookTreeView->append_column( "Contact", phoneBookTree->name );
		  phoneBookTreeView->insert_column_with_data_func( 0, 
			"Contact", *renderer,
			SLOT( *phoneBookModel, &PhoneBookModel::setFont )
			);
	}

//	Gtk::TreeViewColumn * column = phoneBookTreeView->get_column( 0 );

	phoneBookTreeView->expand_all();
}

void MainWindow::setContactDb( MRef<ContactDb *> contactDb ){
	this->contactDb = contactDb;
	logWidget->setContactDb( contactDb );
}
	
bool MainWindow::configDialog( MRef<SipSoftPhoneConfiguration *> conf ){
	int ret;
	settingsDialog->setConfig( conf );

	ret = settingsDialog->run();
	return ( ret == Gtk::RESPONSE_OK );
}

void MainWindow::log( int /*type*/, string /*msg*/ ){
}

void MainWindow::addCall( string callId, string remoteUri, bool incoming,
		          string securityStatus ){
	ContactEntry * entry;
	Gtk::Image * icon;
	Gtk::Label * label = new Gtk::Label;
	Gtk::HBox * hbox = new Gtk::HBox;
	Glib::ustring tabLabelText;


	CallWidget * callWidget = new CallWidget( callId, remoteUri, this, incoming, securityStatus );

	callWidgets.push_back( callWidget );

	entry = contactDb->lookUp( remoteUri );

	if( entry != NULL ){
		tabLabelText = Glib::locale_to_utf8( entry->getName() );
	}
	else{
		tabLabelText = "Call";
	}

	label->set_text( tabLabelText );
	
	if( incoming ){
		icon = new Gtk::Image( Gtk::Stock::GO_BACK, Gtk::ICON_SIZE_SMALL_TOOLBAR );
	}
	else{
		icon = new Gtk::Image( Gtk::Stock::GO_FORWARD, Gtk::ICON_SIZE_SMALL_TOOLBAR );
	}


	hbox->add( *icon );
	hbox->add( *label );
	hbox->show_all();

	
	mainTabWidget->append_page( *callWidget, *hbox ) ;
	callWidget->show();
	mainTabWidget->set_current_page( mainTabWidget->get_n_pages() - 1 );

#ifdef IPAQ
	callWidget->update();
#endif

	
}

void MainWindow::removeCall( string callId ){
	for( list<CallWidget *>::iterator i = callWidgets.begin();
			i != callWidgets.end(); i++ ){
		if( (*i)->getCallId() == callId ){
			mainTabWidget->remove_page( *(*i) );
			callWidgets.erase( i );
			return;
		}
	}
}

void MainWindow::addIm( string uri ){
	string from = config->inherited.sipIdentity->sipUsername + "@" + config->inherited.sipIdentity->sipDomain;
	ImWidget * imWidget = new ImWidget( this, uri, from );

	imWidgets.push_back( imWidget );


	mainTabWidget->append_page( *imWidget, "Im" ) ;
	imWidget->show();
	mainTabWidget->set_current_page( mainTabWidget->get_n_pages() - 1 );
}

void MainWindow::removeIm( string uri ){
	for( list<ImWidget *>::iterator i = imWidgets.begin();
			i != imWidgets.end(); i++ ){
		if( (*i)->getToUri() == uri ){
			mainTabWidget->remove_page( *(*i) );
			imWidgets.erase( i );
			return;
		}
	}
}

void MainWindow::inviteFromTreeview( const Gtk::TreeModel::Path&,
		                     Gtk::TreeViewColumn * ){
	invite();
}

void MainWindow::invite(){
	string uri;

	uri = uriEntry->get_text();

	if( uri.length() > 0 ){
		string id = callback->guicb_doInvite( uri );

		if( id == "malformed" ){
			Gtk::MessageDialog dialog( "The SIP address you specified is not valid", Gtk::MESSAGE_WARNING );
			dialog.show();
		}
		
		addCall( id, uri, false );
	}
}

void MainWindow::im(){
	string uri;

	uri = uriEntry->get_text();

	if( uri.length() > 0 ){
		for( list<ImWidget *>::iterator i = imWidgets.begin();
				i != imWidgets.end(); i++ ){
			if( (*i)->getToUri() == uri ){
				mainTabWidget->set_current_page( 
					mainTabWidget->page_num( **i ) );
				return;
			}
		}
		addIm( uri );
	}
}


	

bool MainWindow::phoneSelect( const Glib::RefPtr<Gtk::TreeModel>& model,
                  const Gtk::TreeModel::Path& path, bool ){
	
	const Gtk::TreeModel::iterator iter = model->get_iter( path );
	/* Only the leaves can be selected */
	return iter->children().empty();
}

void MainWindow::phoneSelected(){
	Glib::RefPtr<Gtk::TreeSelection> treeSelection2 = 
		phoneBookTreeView->get_selection();
	
	Gtk::TreeModel::iterator iter = treeSelection2->get_selected();

	if( iter ){
		if( uriEntry ){
			uriEntry->set_text( (*iter)[phoneBookModel->tree->uri] );
		}

	}
}

void MainWindow::phoneTreeClicked( GdkEventButton * event ){
/*
	if( event->type==GDK_2BUTTON_PRESS || event->type==GDK_3BUTTON_PRESS ){
		phoneSelected();
		invite();
		return;
	}
*/	
	if( event->button == 3 ){
		Glib::RefPtr<Gtk::TreeSelection> treeSelection2 = 
			phoneBookTreeView->get_selection();
		bool noSelect = (treeSelection2->count_selected_rows() == 0 );
		phoneAddAddressMenu->set_sensitive( !noSelect );
		phoneRemoveMenu->set_sensitive( !noSelect );
		phoneEditMenu->set_sensitive( !noSelect );
		phoneMenu->popup( event->button, gtk_get_current_event_time() );
	}
}

void MainWindow::handle( MRef<LogEntry *> logEntry ){
	logEntriesLock.lock();
	logEntries.push_front( logEntry );
	logEntriesLock.unlock();

	logDispatcher.emit();
}

void MainWindow::gotLogEntry(){

	list<CallWidget *>::iterator i;

	logEntriesLock.lock();
	MRef< LogEntry * >  logEntry = logEntries.pop_back();
	logEntriesLock.unlock();

	logWidget->addLogEntry( logEntry );

}	

void MainWindow::viewCallListToggle(){
	if( logWidget->is_visible() ){
		logWidget->hide();
	}
	else{
		logWidget->show();
	}
}

void MainWindow::setCallback( GuiCallback * callback ){
	handleCommand( CommandString( "", "sip_ready" ) );
	Gui::setCallback( callback );
}

void MainWindow::runCertificateSettings(){
	certificateDialog->run();
	config->save();
}
