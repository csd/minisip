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




#include "../../../conf/ConferenceControl.h"
#include"MainWindow.h"
#include"CallWidget.h"
#include"ConferenceWidget.h"
#include"PhoneBook.h"
#include"SettingsDialog.h"
#include"CertificateDialog.h"
#include"DtmfWidget.h"
#ifndef WIN32
#include"TrayIcon.h"
#endif
#include"LogWidget.h"
#include"ImWidget.h"
#include"AccountsList.h"
#include"AccountsStatusWidget.h"
#include"../../../sip/SipSoftPhoneConfiguration.h"
#include"../../contactdb/ContactDb.h"
#include<libmsip/SipCommandString.h>

#include"../../../mediahandler/MediaCommandString.h"

#include<libmutil/trim.h>
#include<libmutil/itoa.h>

//#include<libmsip/SipSoftPhone.h>
//
#ifdef OLDLIBGLADEMM
#define SLOT(a,b) SigC::slot(a,b)
#define BIND SigC::bind 
#else
#define SLOT(a,b) sigc::mem_fun(a,b) 
#define BIND sigc::bind
#endif

MainWindow::MainWindow( int argc, char ** argv ):kit( argc, argv ){

	Gtk::Button * callButton;
	Gtk::Button * imButton;
	Gtk::MenuItem * prefMenu;
	Gtk::MenuItem * certMenu;
	Gtk::MenuItem * quitMenu;
	Gtk::MenuItem * addContactMenu;
	Gtk::MenuItem * addAddressContactMenu;
	Gtk::MenuItem * removeContactMenu;
	Gtk::MenuItem * editContactMenu;
	Gtk::MenuItem * callMenu;
	Gtk::MenuItem * conferenceMenu;
	Gtk::MenuItem * imMenu;
	Glib::RefPtr<Gnome::Glade::Xml>  refXml;
#ifndef OLDLIBGLADEMM
	Gtk::Expander * dtmfExpander;
#endif
	nextConfId=0;
	registerIcons();

	try{
		refXml = Gnome::Glade::Xml::create((string)MINISIP_DATADIR + "/minisip.glade");
	}
  
	catch(const Gnome::Glade::XmlError& ex){
		std::cerr << ex.what() << std::endl;
		exit( 1 );
	}

#ifndef HILDON_SUPPORT
	refXml->get_widget( "minisipMain", mainWindowWidget );
	refXml->get_widget( "mainTabWidget", mainTabWidget );
#endif
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
	refXml->get_widget( "conferenceMenu", conferenceMenu );
	refXml->get_widget( "imMenu", imMenu );

#ifndef OLDLIBGLADEMM
	DtmfWidget * dtmfWidget = manage( new DtmfWidget() );
	dtmfWidget->setHandler( this );
	refXml->get_widget( "dtmfExpander", dtmfExpander );
	dtmfExpander->add( *dtmfWidget );
#endif

#ifdef HILDON_SUPPORT
	/* Create the hildon app */
	mainWindowWidget = dynamic_cast<Gtk::Window *>( 
			manage( Glib::wrap( hildon_app_new() ) ) );
	
	/* Create a hildon app view */
	Gtk::Container *appview = dynamic_cast<Gtk::Container *>( 
		manage( Glib::wrap( hildon_appview_new( "Minisip" ) ) ) );

	hildon_app_set_appview( HILDON_APP( mainWindowWidget->gobj() ), 
			HILDON_APPVIEW( appview->gobj() ) );

	hildon_app_set_title( HILDON_APP( mainWindowWidget->gobj() ), 
			"Minisip" );

	Gtk::Widget * w;
	Gtk::HBox * mainHBox;

	mainHBox = manage( new Gtk::HBox( true, 6 ) );

	mainTabWidget = manage( new Gtk::Notebook() );
	
	refXml->get_widget( "dialVBox", w );

	w->reparent( *mainHBox );
	mainHBox->add( *mainTabWidget );
	//mainHBox->pack_end( *mainTabWidget );

	mainHBox->show_all();

	appview->add( *mainHBox );

	Gtk::Menu * mainMenu = dynamic_cast<Gtk::Menu *>( Glib::wrap( 
	  hildon_appview_get_menu( HILDON_APPVIEW( appview->gobj() ) ) ) );

	refXml->get_widget( "fileMenu", w );
	w->reparent( *mainMenu );
	refXml->get_widget( "viewMenu", w );
	w->reparent( *mainMenu );
	refXml->get_widget( "contactMenu", w );
	w->reparent( *mainMenu );
#endif



	treeSelection = phoneBookTreeView->get_selection();

//	treeSelection->set_select_function( SigC::slot( *this, 
//				&MainWindow::phoneSelect ) );
	
	treeSelection->signal_changed().connect(
		SLOT( *this, &MainWindow::phoneSelected ) );

	phoneBookTree = new PhoneBookTree;
	phoneBookModel = new PhoneBookModel( phoneBookTree );

	phoneBookTreeView->set_headers_visible( true );
	phoneBookTreeView->set_rules_hint( false );
	phoneBookTreeView->signal_button_press_event().connect_notify( 
		SLOT( *this, &MainWindow::phoneTreeClicked ), false );

	//phoneBookTreeView->set_hover_expand( true );

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

	callButton->signal_clicked().connect( SLOT( *this, &MainWindow::inviteClick ) );
	
	callMenu->signal_activate().connect( SLOT( *this, &MainWindow::inviteClick ) );

	//refXml->get_widget( "conferenceButton", conferenceButton );

	//conferenceButton->signal_clicked().connect( SLOT( *this, &MainWindow::conference ) );
	
	conferenceMenu->signal_activate().connect( SLOT( *this, &MainWindow::conference ) );

	phoneBookTreeView->signal_row_activated().connect( SLOT( *this, &MainWindow::inviteFromTreeview ) );

	refXml->get_widget( "imButton", imButton );

	imButton->signal_clicked().connect( SLOT( *this, &MainWindow::imClick ) );
	
	imMenu->signal_activate().connect( SLOT( *this, &MainWindow::imClick ) );
	
	certificateDialog = new CertificateDialog( refXml );
	settingsDialog = new SettingsDialog( refXml, certificateDialog );
	
	refXml->get_widget( "uriEntry", uriEntry );

	refXml->get_widget( "prefMenu", prefMenu );
	refXml->get_widget( "certMenu", certMenu );
	refXml->get_widget( "quitMenu", quitMenu );
	
	refXml->get_widget( "viewCallListMenu", viewCallListMenu );
	refXml->get_widget( "viewStatusMenu", viewStatusMenu );

	prefMenu->signal_activate().connect( SLOT( *settingsDialog, &SettingsDialog::show ) );
	certMenu->signal_activate().connect( SLOT( *this, &MainWindow::runCertificateSettings ) );
	
	//This two signals are for closing minisip ... see MainWindow::quit
	quitMenu->signal_activate().connect( SLOT( *this, &MainWindow::quit ) );
	mainWindowWidget->signal_delete_event().connect( SLOT( *this, &MainWindow::on_window_close ) );
	
	viewCallListMenu->signal_activate().connect( 
		BIND<uint8_t>(
			SLOT( *this, &MainWindow::viewToggle ),
			0 
			));
	
	viewStatusMenu->signal_activate().connect( 
		BIND<uint8_t>(
			SLOT( *this, &MainWindow::viewToggle ),
			1 
			));

	dispatcher.connect( SLOT( *this, &MainWindow::gotCommand ) );

	uriEntry->signal_activate().connect( SLOT( *this, &MainWindow::inviteClick ) );

#if not defined WIN32 && not defined HILDON_SUPPORT
	trayIcon = new MTrayIcon( this, refXml );

	mainWindowWidget->signal_hide().connect( SLOT( *this, &MainWindow::hideSlot ));
#endif

	logWidget = new LogWidget( this );

	//mainTabWidget->append_page( *logWidget, "Call list" );

	accountsList = AccountsList::create( new AccountsListColumns() );

	statusWidget = new AccountsStatusWidget( accountsList);
	//mainTabWidget->append_page( *statusWidget, "Accounts" );

	logDispatcher.connect( SLOT( *this, &MainWindow::gotLogEntry ) );


	mainTabWidget->signal_switch_page().connect(
		SLOT( *this, &MainWindow::onTabChange ) );

	mainWindowWidget->show_all();
//	statusWidget->hide();
	logWidget->hide();
	mainWindowWidget->set_sensitive( false );
}

MainWindow::~MainWindow(){
	factory->remove_default();
	delete settingsDialog;
	delete certificateDialog;
	delete statusWidget;
	delete logWidget;
	delete phoneMenu;
	if( trayIcon ){
		delete trayIcon;
	}
	delete phoneBookTree;
	delete mainWindowWidget;
}

bool MainWindow::on_window_close (GdkEventAny* event  ) {
	//enter quit mode ...
	quit();
	return true;
	
}

void MainWindow::run(){

	//add to the command dispatcher queue the startup commands
	list<string>::iterator iter;
	iter = getSipSoftPhoneConfiguration()->startupActions.begin();
	for( ; iter != getSipSoftPhoneConfiguration()->startupActions.end(); iter++ ) {
		int pos; 
		pos = (*iter).find(' ');
		string cmd = (*iter).substr( 0, pos );
		pos ++; //advance to the start of the params ...
		string params = (*iter).substr( pos, (*iter).size() - pos );
		handleCommand( CommandString( "", "startup_" + cmd, params ) ); 
	}

#ifndef WIN32
	if( trayIcon != NULL ){
		Gtk::Window * window = NULL;
		window = trayIcon->getWindow();
		kit.run( *window );
		if( window != NULL )	delete window;
	}
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
	list<ConferenceWidget *>::iterator j;

	commandsLock.lock();
	CommandString command = commands.pop_back();
	commandsLock.unlock();
#ifdef OUTPUT_DEBUG
// 	merr << "DEBUG: MainWindow::gotCmd :  " << command.getString() << end;
#endif
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
	for( j = conferenceWidgets.begin(); j != conferenceWidgets.end(); j++ ){
		if( (*j)->handleCommand( command ) ){
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
	if( command.getOp()=="conf_join_received" ){
		//string confid=itoa(rand());
		string confid="";
		string users=command.getParam3();
		int i=0;	
		while (users[i]!=';'&& users.length()!=0 &&!((uint)i>(users.length()-1))){
			confid=confid+users[i];
			i++;
		}
		users=trim(users.substr(i));
		
		addConference( confid, users,command.getParam(),command.getDestinationId(), true );
		return;
	}
	
	if( command.getOp() == SipCommandString::remote_presence_update){
		MRef<ContactEntry*> ce = contactDb->lookUp(command.getParam());
		if (ce){
			string state = command.getParam2();
// 			cerr << "State is: "<< state << endl;
			if (state =="online"){
// 				cerr << "Changed status to online "<< endl;
				ce->setOnlineStatus(CONTACT_STATUS_ONLINE);
			}else if (state=="offline")
				ce->setOnlineStatus(CONTACT_STATUS_OFFLINE);
			else
				ce->setOnlineStatus(CONTACT_STATUS_UNKNOWN);
			ce->setOnlineStatusDesc(command.getParam3());
			phoneBookTreeView->queue_draw();
			
		}else{
// 			cerr << "MainWindow::gotCommand: WARNING: did not find uri <"<< command.getParam()<< "> to change presence info"<< endl;
		}
		return;
	}
	
	if( command.getOp() == "startup_call"){ 
		string uri;
		string params;
		params = command.getParam();
		uri = params.substr( 0, params.find(' ') ); 
		invite( uri );
		return;
	} else if ( command.getOp() == "startup_im") {
		string uri;
		string params, mesg;
		int pos;
		params = command.getParam();
		pos = params.find( ' ' );
		uri = params.substr( 0, pos ); //get uri
		pos++;
		mesg = params.substr( pos, params.size() - pos ); //get message
		im( uri, mesg );
		return;
	} 
	
	if( command.getOp() == "set_active_tab" ) {
		int pageIdx;
		string param;
		param = command.getParam();
		if( param == "" ) 
			pageIdx = 0;
		else 	
			pageIdx = atoi( param.c_str() );
		mainTabWidget->set_current_page( pageIdx );
	}
	
	
#ifdef OUTPUT_DEBUG
	//merr << "MainWindow::gotCommand: Warning: did not handle command: "<< command.getOp()<< end;
#endif
}	

void MainWindow::gotPacket( int32_t /*i*/ ){
}

void MainWindow::displayMessage( string s, int style ){
	handleCommand( CommandString( "", "error_message", 
		Glib::locale_to_utf8( s ) ) );
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

MRef<SipSoftPhoneConfiguration *> MainWindow::getSipSoftPhoneConfiguration() {
	return this->config;
}
		
void MainWindow::updateConfig(){
	
	accountsList->loadFromConfig( config );
	settingsDialog->setAccounts( accountsList );
	settingsDialog->setConfig( config );
	certificateDialog->setCertChain( config->securityConfig.cert );
	certificateDialog->setRootCa( config->securityConfig.cert_db );

	const Glib::RefPtr<PhoneBookModel> modelPtr( phoneBookModel );

	list< MRef<PhoneBook *> > phonebooks = config->phonebooks;
	list< MRef<PhoneBook *> >::iterator i;

	for( i = phonebooks.begin(); i != phonebooks.end(); i++ ){
		phoneBookModel->setPhoneBook( *i );
	}
	
	Gtk::CellRendererText * renderer;
	phoneBookTreeView->set_model( modelPtr );
	if( phonebooks.size() > 0 ){
		renderer = manage( new Gtk::CellRendererText() );
		phoneBookTreeView->insert_column_with_data_func( 0, 
				"Contact", *renderer,
				SLOT( *phoneBookModel, &PhoneBookModel::setFont )
				);
	}
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

CallWidget * MainWindow::addCall( string callId, string remoteUri, bool incoming,
		          string securityStatus ){
	ContactEntry * entry;
	Gtk::Image * icon;
	Gtk::Label * label = manage( new Gtk::Label );
	Gtk::HBox * hbox = manage( new Gtk::HBox );
	Glib::ustring tabLabelText;

	CallWidget * callWidget = new CallWidget( callId, remoteUri, this, incoming, securityStatus );

	callWidgets.push_back( callWidget );

	entry = contactDb->lookUp( remoteUri );

	if( entry != NULL ){
		tabLabelText = Glib::locale_to_utf8( entry->getName() );
	}
	else{
		tabLabelText = remoteUri;
	}

	label->set_text( tabLabelText );
	
	if( incoming ){
		icon = manage( new Gtk::Image( Gtk::Stock::GO_BACK, Gtk::ICON_SIZE_SMALL_TOOLBAR ) );
	}
	else{
		icon = manage( new Gtk::Image( Gtk::Stock::GO_FORWARD, Gtk::ICON_SIZE_SMALL_TOOLBAR ) );
	}

	hbox->add( *icon );
	hbox->add( *label );
	hbox->show_all();
	
	mainTabWidget->append_page( *callWidget, *hbox ) ;
	callWidget->show();
	mainTabWidget->set_current_page( mainTabWidget->get_n_pages() - 1 );

	return callWidget;
}

void MainWindow::removeCall( string callId ){
	//do not increase the iterator automatically ... 
	//   we remove elements, thus we obtain the next element inside the loop
	for( list<CallWidget *>::iterator i = callWidgets.begin();
			i != callWidgets.end(); i++){
		if( (*i)->getMainCallId() == callId ){
			mainTabWidget->remove_page( *(*i) );
			callWidgets.erase( i );
			return;
		}
	}
}

void MainWindow::addConference( string confId, string users,string remoteUri,string callId, bool incoming ){
	//ContactEntry * entry; //not used
	Gtk::Image * icon;
	Gtk::Label * label = new Gtk::Label;
	Gtk::HBox * hbox = new Gtk::HBox;
	Glib::ustring tabLabelText;

	string from = config->inherited->sipIdentity->sipUsername + "@" + config->inherited->sipIdentity->sipDomain;
	ConferenceWidget * conferenceWidget = new ConferenceWidget(from, confId, users, remoteUri,callId, this, incoming);

	conferenceWidgets.push_back( conferenceWidget );

	tabLabelText = "Conference "+confId;
	

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

	
	mainTabWidget->append_page( *conferenceWidget, *hbox ) ;
	conferenceWidget->show();
	mainTabWidget->set_current_page( mainTabWidget->get_n_pages() - 1 );
}

void MainWindow::removeConference( string callId ){
	for( list<ConferenceWidget *>::iterator i = conferenceWidgets.begin();
			i != conferenceWidgets.end(); i++ ){
		if( (*i)->getMainConfId() == callId ){
			mainTabWidget->remove_page( *(*i) );
			conferenceWidgets.erase( i );
			return;
		}
	}
}

ImWidget * MainWindow::addIm( string uri ){
	string from = config->inherited->sipIdentity->sipUsername + "@" + config->inherited->sipIdentity->sipDomain;
	ImWidget * imWidget = new ImWidget( this, uri, from );

	imWidgets.push_back( imWidget );

	mainTabWidget->append_page( *imWidget, "Im " + uri ) ;
	imWidget->show();
	mainTabWidget->set_current_page( mainTabWidget->get_n_pages() - 1 );
	return imWidget;
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

void MainWindow::setActiveTabWidget( Gtk::Widget * widget ) {
	//mainTabWidget->set_focus_child( *widget );
	Gtk::Widget * currentWidget;
	currentWidget = mainTabWidget->get_nth_page( mainTabWidget->get_current_page() );
	for( int idx = 0; idx<mainTabWidget->get_n_pages(); idx++ ) {
		if( mainTabWidget->get_nth_page( idx ) == widget ) {
			handleCommand( CommandString( "", "set_active_tab", itoa(idx ) ) );
		}
	}
}

void MainWindow::setActiveTabWidget( int pageIdx ) {
	if( pageIdx >= 0 && pageIdx < mainTabWidget->get_n_pages() ) {
		handleCommand( CommandString( "", "set_active_tab", itoa( pageIdx ) ) );
	} 
#ifdef DEBUG_OUTPUT
	else {
		cerr << "MainWindow::setActiveTabWidget(int) - ignoring order (bad index)!" << endl;
	}
#endif
}

void MainWindow::inviteFromTreeview( const Gtk::TreeModel::Path&,
		                     Gtk::TreeViewColumn * ){
	phoneSelected();
	invite();
}

void MainWindow::inviteClick() {
	invite( uriEntry->get_text() );
}

void MainWindow::invite( string uri ){

	if( uri.length() > 0 ){
		string id = callback->guicb_doInvite( uri );

		if( id == "malformed" ){
			Gtk::MessageDialog dialog( "The SIP address you specified is not valid", Gtk::MESSAGE_WARNING );
			dialog.show();
		}
		
		addCall( id, uri, false );
	}
}

void MainWindow::conference(){
	string confid=itoa(rand());
	//callback->guicb_confDoInvite("ali");
	//string id = callback->guicb_doInvite( uri );
	addConference( confid, "","","",false );
}

void MainWindow::imClick( ) {
	im( uriEntry->get_text() );
}

void MainWindow::im( string uri, string message ){
	
	if( uri.length() > 0 ){
		bool found = false;
		ImWidget * imW;
		for( list<ImWidget *>::iterator i = imWidgets.begin();
				i != imWidgets.end(); i++ ){
			if( (*i)->getToUri() == uri ){
				mainTabWidget->set_current_page( 
					mainTabWidget->page_num( **i ) );
				imW = (*i);
				found = true;
				break;
			}
		}
		if( !found ) {
			imW = addIm( uri );
		}
		if( message != "" && imW ) {
			imW->send( message );
		}
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
        Gtk::TreeModel::Path path;
        Gtk::TreeViewColumn * column;
        int cellx,celly;
        bool gotPath;
        
        gotPath = phoneBookTreeView->get_path_at_pos( (int)event->x, (int)event->y, path, column, cellx, celly );

        /* Collapse or expand the phonebook entries */
        // FIXME: cellx >= 13 used to avoid the expander arrow, find
        // some proper way
        if( event->button == 1 && gotPath && path.get_depth() <= 1 && cellx >= 13){
                if( phoneBookTreeView->row_expanded( path ) ){
                        phoneBookTreeView->collapse_row( path );
                }
                else{

                        phoneBookTreeView->collapse_all();
                        phoneBookTreeView->expand_row( path, true );
                }
        }

        /* right click: pop up the menu */
        else if( event->button == 3 ){
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

void MainWindow::viewToggle( uint8_t w ){
	Gtk::Widget * widget;
	Glib::ustring title;
	switch( w ){
		case 0:
			widget = logWidget;
			title = "Call list";
			break;
		case 1:
			widget = statusWidget;
			title = "Accounts";
			break;
		default:
			return;
	}
	
	if( widget->is_visible() ){
		mainTabWidget->remove_page( *widget );
		widget->hide();
	}
	else{
		mainTabWidget->append_page( *widget, title );
		mainTabWidget->show_all();
	}
}

void MainWindow::setCallback( GuiCallback * callback ){
	statusWidget->setCallback( callback );
	settingsDialog->setCallback( callback );
	handleCommand( CommandString( "", "sip_ready" ) );
	Gui::setCallback( callback );
}

void MainWindow::runCertificateSettings(){
	certificateDialog->run();
	config->save();
}

void MainWindow::registerIcons(){
	factory = Gtk::IconFactory::create();
	
	Gtk::IconSet * iconSet = new Gtk::IconSet;
	Gtk::IconSource * iconSource = new Gtk::IconSource;

	iconSource->set_filename( (string)MINISIP_DATADIR + "/secure.png" );
	iconSource->set_size( Gtk::ICON_SIZE_DIALOG );
	iconSet->add_source( *iconSource );

	factory->add( Gtk::StockID( "minisip_secure" ), *iconSet );

	delete iconSource;
	delete iconSet;
	iconSource = new Gtk::IconSource;
	iconSet = new Gtk::IconSet;
	
	iconSource->set_filename( (string)MINISIP_DATADIR + "/insecure.png" );
	iconSource->set_size( Gtk::ICON_SIZE_DIALOG );
	iconSet->add_source( *iconSource );

	factory->add( Gtk::StockID( "minisip_insecure" ), *iconSet );
	
	delete iconSource;
	delete iconSet;
	iconSource = new Gtk::IconSource;
	iconSet = new Gtk::IconSet;
	
	iconSource->set_filename( (string)MINISIP_DATADIR + "/play.png" );
	iconSource->set_size( Gtk::ICON_SIZE_BUTTON );
	iconSet->add_source( *iconSource );

	factory->add( Gtk::StockID( "minisip_play" ), *iconSet );
	
	delete iconSource;
	delete iconSet;
	iconSource = new Gtk::IconSource;
	iconSet = new Gtk::IconSet;
	
	iconSource->set_filename( (string)MINISIP_DATADIR + "/noplay.png" );
	iconSource->set_size( Gtk::ICON_SIZE_BUTTON );
	iconSet->add_source( *iconSource );

	factory->add( Gtk::StockID( "minisip_noplay" ), *iconSet );
	
	delete iconSource;
	delete iconSet;
	iconSource = new Gtk::IconSource;
	iconSet = new Gtk::IconSet;
	
	iconSource->set_filename( (string)MINISIP_DATADIR + "/record.png" );
	iconSource->set_size( Gtk::ICON_SIZE_BUTTON );
	iconSet->add_source( *iconSource );

	factory->add( Gtk::StockID( "minisip_record" ), *iconSet );
	
	delete iconSource;
	delete iconSet;
	iconSource = new Gtk::IconSource;
	iconSet = new Gtk::IconSet;
	
	iconSource->set_filename( (string)MINISIP_DATADIR + "/norecord.png" );
	iconSource->set_size( Gtk::ICON_SIZE_BUTTON );
	iconSet->add_source( *iconSource );

	factory->add( Gtk::StockID( "minisip_norecord" ), *iconSet );

	factory->add_default();
	delete iconSet;
	delete iconSource;
}

void MainWindow::dtmfPressed( uint8_t symbol ){
	Glib::ustring uri = uriEntry->get_text();
	
	switch( symbol ){
		case 1:
			uri += "1";
			break;
		case 2:
			uri += "2";
			break;
		case 3:
			uri += "3";
			break;
		case 4:
			uri += "4";
			break;
		case 5:
			uri += "5";
			break;
		case 6:
			uri += "6";
			break;
		case 7:
			uri += "7";
			break;
		case 8:
			uri += "8";
			break;
		case 9:
			uri += "9";
			break;
		case 0:
			uri += "0";
			break;
	}
	uriEntry->set_text( uri );
}

void MainWindow::onTabChange( GtkNotebookPage * page, guint index ){
	Gtk::Widget * currentPage = mainTabWidget->get_nth_page( index );
	CallWidget * callWidget = NULL;
	ConferenceWidget *confWidget = NULL;
	ImWidget * imWidget = NULL;
	
	//cast the current page to the possible media widgets ...
	callWidget = dynamic_cast< CallWidget *>( currentPage );
	if( callWidget == NULL )
		confWidget = dynamic_cast< ConferenceWidget *>( currentPage );
	if( confWidget == NULL ) 
		imWidget = dynamic_cast< ImWidget *>( currentPage );
	
	//go through all the widgets, notifying them of the change
	list<CallWidget *>::iterator iterCall;
	for( iterCall = callWidgets.begin();
			iterCall != callWidgets.end();
			iterCall++ ) {
		bool equal = false;
		if( callWidget ) equal = (callWidget == (*iterCall) );
		(*iterCall)->activeWidgetChanged( equal, index );
	}

	list<ConferenceWidget *>::iterator iterConf;
	for( iterConf = conferenceWidgets.begin();
			iterConf != conferenceWidgets.end();
			iterConf++ ) {
		bool equal = false;
		if( confWidget ) equal = (confWidget == (*iterConf) );
		(*iterConf)->activeWidgetChanged( equal, index );
	}
	
	list<ImWidget *>::iterator iterIm;
	for( iterIm = imWidgets.begin();
			iterIm != imWidgets.end();
			iterIm++ ) {
		bool equal = false;
		if( imWidget ) equal = (imWidget == (*iterIm) );
		(*iterIm)->activeWidgetChanged( equal, index );
	}
	
}

