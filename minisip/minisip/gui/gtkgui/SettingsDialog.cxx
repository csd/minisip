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

#include"SettingsDialog.h"
#include"CertificateDialog.h"
#include"AccountsList.h"
#include<libminisip/gui/Gui.h>
#include<libminisip/sip/SipSoftPhoneConfiguration.h>
#include<libminisip/mediahandler/MediaCommandString.h>

#include<libmnetutil/NetworkFunctions.h>

#ifdef OLDLIBGLADEMM
#define SLOT(a,b) SigC::slot(a,b)
#define BIND SigC::bind
#else
#define SLOT(a,b) sigc::mem_fun(a,b)
#define BIND sigc::bind
#endif

using namespace std;

SettingsDialog::SettingsDialog( Glib::RefPtr<Gnome::Glade::Xml>  refXml,
		                CertificateDialog * certificateDialog){
	this->certificateDialog = certificateDialog;
	refXml->get_widget( "settingsDialog", dialogWindow );

	Gtk::Button * settingsOkButton;
	Gtk::Button * settingsCancelButton;
	
	/* Connect the Ok and cancel buttons */
	
	refXml->get_widget( "settingsOkButton", settingsOkButton );
	refXml->get_widget( "settingsCancelButton", settingsCancelButton );

	settingsOkButton->signal_clicked().connect( SLOT( *this, &SettingsDialog::accept ) );
	settingsCancelButton->signal_clicked().connect( SLOT( *this, &SettingsDialog::reject ) );


	refXml->get_widget( "certificateButton", certificateButton );

	certificateButton->signal_clicked().connect( SLOT( *certificateDialog, &CertificateDialog::run ) );
	
	generalSettings = new GeneralSettings( refXml );
	mediaSettings = new MediaSettings( refXml );
	securitySettings = new SecuritySettings( refXml );
	advancedSettings = new AdvancedSettings( refXml );


	dialogWindow->hide();
#ifdef IPAQ
//	dialogWindow->maximize();
	dialogWindow->set_type_hint( Gdk::WINDOW_TYPE_HINT_NORMAL );
#endif
}

SettingsDialog::~SettingsDialog(){
	delete generalSettings;
	delete mediaSettings;
	delete securitySettings;
	delete advancedSettings;
	delete dialogWindow;
}

void SettingsDialog::setCallback( MRef<CommandReceiver*> callback ){
	this->callback = callback;
}

void SettingsDialog::setAccounts( Glib::RefPtr<AccountsList> list ){
	generalSettings->setAccounts( list );
}

void SettingsDialog::setConfig( MRef<SipSoftPhoneConfiguration *> config ){
	this->config = config;
	generalSettings->setConfig( config );
	mediaSettings->setConfig( config );
	securitySettings->setConfig( config );
	advancedSettings->setConfig( config );
	
}

int SettingsDialog::run(){
	int ret = dialogWindow->run();
	dialogWindow->hide();

	return ret;

}

void SettingsDialog::show(){
	dialogWindow->show();
//	dialogWindow->hide();
}

void SettingsDialog::accept(){
	string warning( "" );

	warning += generalSettings->apply();
	warning += mediaSettings->apply();
	warning += securitySettings->apply();
	warning += advancedSettings->apply();
	
	config->save();
	// FIXME: only reload the mediahandler when something actually
	// changed in the media properties
	CommandString cmdstr = CommandString( "", MediaCommandString::reload );
	callback->handleCommand("media", cmdstr );

	if( warning != "" ){
#ifdef OLDLIBGLADEMM
		Gtk::MessageDialog messageDialog( warning, 
				Gtk::MESSAGE_WARNING, Gtk::BUTTONS_OK, 
				/* use markup */false,
				/* Modal */true );
#else
		Gtk::MessageDialog messageDialog( warning,
				/* use markup */false, 
				Gtk::MESSAGE_WARNING, Gtk::BUTTONS_OK,
				/* Modal */ true );
#endif
		messageDialog.run();
	}
	
	dialogWindow->response( Gtk::RESPONSE_OK );
	dialogWindow->hide();
}

void SettingsDialog::reject(){
	dialogWindow->response( Gtk::RESPONSE_CANCEL );
	dialogWindow->hide();
}

GeneralSettings::GeneralSettings( Glib::RefPtr<Gnome::Glade::Xml>  refXml ){

	refXml->get_widget( "accountsTreeView", accountsTreeView );
	if( accountsTreeView == NULL ){
		exit( 1 );
	}
	
	refXml->get_widget( "accountsAddButton", accountsAddButton );
	if( accountsAddButton == NULL ){
		exit( 1 );
	}

	refXml->get_widget( "accountsRemoveButton", accountsRemoveButton );
	if( accountsRemoveButton == NULL ){
		exit( 1 );
	}
	
	refXml->get_widget( "accountsEditButton", accountsEditButton );
	if( accountsEditButton == NULL ){
		exit( 1 );
	}
	
	refXml->get_widget( "defaultButton", defaultButton );
	if( defaultButton == NULL ){
		exit( 1 );
	}
	
	refXml->get_widget( "pstnButton", pstnButton );
	if( pstnButton == NULL ){
		exit( 1 );
	}

	accountsTreeView->set_headers_visible( true );
	accountsTreeView->set_rules_hint( true );

	accountsAddButton->signal_clicked().connect( 
			SLOT( *this, &GeneralSettings::addAccount ) );
	accountsEditButton->signal_clicked().connect( 
			SLOT( *this, &GeneralSettings::editAccount ) );
	accountsRemoveButton->signal_clicked().connect( 
			SLOT( *this, &GeneralSettings::removeAccount ) );
	defaultButton->signal_clicked().connect( 
			SLOT( *this, &GeneralSettings::setDefaultAccount ) );
	pstnButton->signal_clicked().connect( 
			SLOT( *this, &GeneralSettings::setPstnAccount ) );


#ifdef IPAQ
	// Make the buttons smaller to fit the screen
	defaultButton->set_label( "Default" );
	pstnButton->set_label( "PSTN" );
#endif
}

void GeneralSettings::addAccount(){
	if( accountsList ){
		accountsList->addAccount();
	}
}

void GeneralSettings::editAccount(){
	if( accountsTreeView->get_selection()->get_selected() ){
		accountsList->editAccount( accountsTreeView->get_selection()->get_selected() );
	}
}

void GeneralSettings::removeAccount(){
	if( accountsTreeView->get_selection()->get_selected() ){
#ifdef OLDLIBGLADEMM
		Gtk::MessageDialog dialog( "Are you sure you want to erase "
			"this account?", 
			Gtk::MESSAGE_QUESTION, 
			Gtk::BUTTONS_YES_NO, true, false );
#else
		Gtk::MessageDialog dialog( "Are you sure you want to erase "
			"this account?", 
			/* use markup*/false,
			Gtk::MESSAGE_QUESTION, 
			Gtk::BUTTONS_YES_NO, 
			/* Modal */true );
#endif
		if( dialog.run() == Gtk::RESPONSE_YES ){
			accountsList->erase( 
			accountsTreeView->get_selection()->get_selected() );
		}
	}
}

void GeneralSettings::setDefaultAccount(){
	if( accountsTreeView->get_selection()->get_selected() ){
		accountsList->setDefaultAccount(  accountsTreeView->get_selection()->get_selected() );
	}
}

void GeneralSettings::setPstnAccount(){
	if( accountsTreeView->get_selection()->get_selected() ){
		accountsList->setPstnAccount(  accountsTreeView->get_selection()->get_selected() );
	}
}

void GeneralSettings::setAccounts( Glib::RefPtr<AccountsList> list ){
	AccountsListColumns * columns = list->getColumns();
	accountsTreeView->set_model( list );

#ifndef IPAQ
        accountsTreeView->append_column_editable( "Register", columns->doRegister ); 
#else
        accountsTreeView->append_column_editable( "R", columns->doRegister );
#endif
        accountsTreeView->append_column( "Account", columns->name );
#ifndef IPAQ
        accountsTreeView->append_column( "Default", columns->defaultProxy );         accountsTreeView->append_column( "PSTN", columns->pstnProxy );
#else
        accountsTreeView->append_column( "D", columns->defaultProxy );
        accountsTreeView->append_column( "P", columns->pstnProxy );
#endif
        accountsTreeView->columns_autosize();
	accountsList = list;
}


void GeneralSettings::setConfig( MRef<SipSoftPhoneConfiguration *> config ){
	this->config = config;
}

string GeneralSettings::apply(){
	string err;
	err += accountsList->saveToConfig( config );
	return err;

}

MediaSettings::MediaSettings( Glib::RefPtr<Gnome::Glade::Xml>  refXml ){

	refXml->get_widget( "codecUpButton", codecUpButton );
	refXml->get_widget( "codecDownButton", codecDownButton );

	refXml->get_widget( "codecTreeView", codecTreeView );
	
	refXml->get_widget( "soundEntry", soundEntry );
	refXml->get_widget( "videoEntry", videoEntry );
	
	refXml->get_widget( "videoLabel", videoLabel );
	refXml->get_widget( "videoDeviceLabel", videoDeviceLabel );
	
	refXml->get_widget( "spaudioCheck", spaudioCheck );

	/* Build the ListStore */
	codecColumns = new Gtk::TreeModelColumnRecord();
	codecColumns->add( codecEnabled );
	codecColumns->add( codecName );

	codecList = Gtk::ListStore::create( *codecColumns );

	codecTreeView->set_model( codecList );
//	codecTreeView->append_column( "Enabled", codecEnabled );
	codecTreeView->append_column( "CODEC", codecName );

	codecUpButton->signal_clicked().connect( BIND<int8_t>(
		SLOT( *this, &MediaSettings::moveCodec ),
		-1 ) );
	
	codecDownButton->signal_clicked().connect( BIND<int8_t>(
		SLOT( *this, &MediaSettings::moveCodec ),
		1 ) );
#ifndef VIDEO_SUPPORT
	videoEntry->hide();
	videoLabel->hide();
	videoDeviceLabel->hide();
#endif


}

MediaSettings::~MediaSettings(){
	delete codecColumns;
}

void MediaSettings::setConfig( MRef<SipSoftPhoneConfiguration *> config ){
	list<string>::iterator iC;
	Gtk::TreeModel::iterator listIterator;
	this->config = config;
	soundEntry->set_text( config->soundDevice );
#ifdef VIDEO_SUPPORT
	videoEntry->set_text( config->videoDevice );
#endif

	codecList->clear();

	for( iC = config->audioCodecs.begin(); iC != config->audioCodecs.end();
			iC ++ ){
		listIterator = codecList->append();
		(*listIterator)[codecName] = 
			Glib::locale_to_utf8( *iC );
		(*listIterator)[codecEnabled] = true;
	}

	if( config->soundIOmixerType == "spatial" ){
		spaudioCheck->set_active( true );
	}

}

void MediaSettings::moveCodec( int8_t upOrDown ){
	Glib::RefPtr<Gtk::TreeSelection> treeSelection = 
		codecTreeView->get_selection();

	Gtk::TreeModel::iterator iter = treeSelection->get_selected();
	Gtk::TreeModel::iterator iter2 = treeSelection->get_selected();
#ifdef OLDLIBGLADEMM
	Gtk::TreeModel::iterator i, savedIter;
#endif

	if( iter ){
		if( upOrDown > 0 ){
			iter2 ++;
		}
		else{
			if( iter2 == codecList->children().begin() ){
				// already on the top of the list
				return;
			}
#ifndef OLDLIBGLADEMM
			iter2 --;
#else
			savedIter = i = codecList->children().begin();
			i++;
			for( ; i!= codecList->children().end(); i++ ){
				if( *i == *iter2 ){
					iter2 = savedIter;
					break;
				}
				savedIter = i;
			}

#endif
		}

		if( iter2 ){
			codecList->iter_swap( iter, iter2 );
		}
	}
}

string MediaSettings::apply(){
	Gtk::TreeModel::iterator iC;
	config->soundDevice = soundEntry->get_text();
	config->audioCodecs.clear();

	for( iC = codecList->children().begin(); iC ; iC ++ ){
		config->audioCodecs.push_back( 
			Glib::locale_from_utf8( (*iC)[codecName] ) );
	}

	if( spaudioCheck->get_active() ){
		config->soundIOmixerType = "spatial";
	}
	else{
		config->soundIOmixerType = "simple";
	}
	
#ifdef VIDEO_SUPPORT
	config->videoDevice = videoEntry->get_text();
#endif
	return "";	
}


SecuritySettings::SecuritySettings( Glib::RefPtr<Gnome::Glade::Xml>  refXml ){
	
	refXml->get_widget( "dhCheck", dhCheck );
	refXml->get_widget( "pskCheck", pskCheck );

	refXml->get_widget( "pskEntry", pskEntry );

	refXml->get_widget( "secureCheck", secureCheck );
	
//	refXml->get_widget( "secureTable", secureTable );
	
//	refXml->get_widget( "kaEntry", kaEntry );
//	refXml->get_widget( "kaCombo", kaCombo );

//	refXml->get_widget( "pskBox", pskBox );
	refXml->get_widget( "pskRadio", pskRadio );
	refXml->get_widget( "dhRadio", dhRadio );
	
	refXml->get_widget( "kaTypeLabel", kaTypeLabel );

	
	dhCheck->signal_toggled().connect( SLOT( 
		*this, &SecuritySettings::kaChange ) );
	
	pskCheck->signal_toggled().connect( SLOT( 
		*this, &SecuritySettings::kaChange ) );

	secureCheck->signal_toggled().connect( SLOT( 
		*this, &SecuritySettings::secureChange ) );

	
	//kaCombo->set_value_in_list( true );
//	kaEntry->set_editable( false );
	
}

void SecuritySettings::setConfig( MRef<SipSoftPhoneConfiguration *> config ){ 
	this->config = config;

	dhCheck->set_active( config->securityConfig.dh_enabled );
	pskCheck->set_active( config->securityConfig.psk_enabled );

	string psk( (const char *)config->securityConfig.psk, config->securityConfig.psk_length );
	pskEntry->set_text( psk );


	if( config->securityConfig.ka_type == KEY_MGMT_METHOD_MIKEY_DH ){
		dhRadio->set_active( true );
	}

	else if( config->securityConfig.ka_type == KEY_MGMT_METHOD_MIKEY_PSK ){
		pskRadio->set_active( true );
	}

	secureCheck->set_active( config->securityConfig.secured );
	
	kaChange();
	secureChange();

}

void SecuritySettings::kaChange(){

	pskEntry->set_sensitive( pskCheck->get_active() );
	
	secureCheck->set_sensitive( pskCheck->get_active() 
			|| dhCheck->get_active() );

	if( !( pskCheck->get_active() || dhCheck->get_active() ) ){
		secureCheck->set_active( false );
	}

	pskRadio->set_sensitive( secureCheck->get_active() && 
			         pskCheck->get_active() );
	dhRadio->set_sensitive( secureCheck->get_active() && 
			        dhCheck->get_active() );

	if( dhCheck->get_active() && ! pskCheck->get_active() ){
		dhRadio->set_active( true );
	}

	if( pskCheck->get_active() && ! dhCheck->get_active() ){
		pskRadio->set_active( true );
	}
}

void SecuritySettings::secureChange(){
//	secureTable->set_sensitive( secureCheck->get_active() );
	kaTypeLabel->set_sensitive( secureCheck->get_active() );
	pskRadio->set_sensitive( secureCheck->get_active() && 
			         pskCheck->get_active() );
	dhRadio->set_sensitive( secureCheck->get_active() && 
			        dhCheck->get_active() );

}


string SecuritySettings::apply(){
	string err;
	if( dhCheck->get_active() ){
		config->securityConfig.cert->lock();
		if( config->securityConfig.cert->is_empty() ){
			err += "You have selected the Diffie-Hellman key agreement\n"
		       "but have not selected a certificate file.\n"
		       "The D-H key agreement has been disabled.";
			dhCheck->set_active( false );
		}
		
		else if( !config->securityConfig.cert->get_first()->get_openssl_private_key() ){
			err += "You have selected the Diffie-Hellman key agreement\n"
		       "but have not selected a private key file.\n"
		       "The D-H key agreement has been disabled.";
			dhCheck->set_active( false );
		}
		config->securityConfig.cert->unlock();
	}

	config->securityConfig.dh_enabled = dhCheck->get_active();
	config->securityConfig.psk_enabled = pskCheck->get_active();


	string s = pskEntry->get_text();
        const unsigned char * psk = (const unsigned char *)s.c_str();
        unsigned int psk_length = s.size();
        if( config->securityConfig.psk != NULL )
                delete [] config->securityConfig.psk;
        config->securityConfig.psk = new unsigned char[psk_length];
        memcpy( config->securityConfig.psk, psk, psk_length );
        config->securityConfig.psk_length = psk_length;

	config->securityConfig.secured = secureCheck->get_active();
	if( config->inherited->sipIdentity ){
		config->inherited->sipIdentity->securitySupport = secureCheck->get_active();
	}

	if( config->securityConfig.secured ){
		if( pskRadio->get_active() ){
			config->securityConfig.ka_type = KEY_MGMT_METHOD_MIKEY_PSK;
		}
		else if( dhRadio->get_active() ){
			config->securityConfig.ka_type = KEY_MGMT_METHOD_MIKEY_DH;
		}
	}

	return err;

}

AdvancedSettings::AdvancedSettings( Glib::RefPtr<Gnome::Glade::Xml>  refXml ){
	refXml->get_widget( "networkInterfacesCombo", networkInterfacesCombo );
	refXml->get_widget( "networkInterfacesEntry", networkInterfacesEntry );
		
	refXml->get_widget( "udpSpin", udpSpin );
	refXml->get_widget( "tcpSpin", tcpSpin );
	refXml->get_widget( "tlsSpin", tlsSpin );
	
	refXml->get_widget( "tcpCheck", tcpCheck );
	refXml->get_widget( "tlsCheck", tlsCheck );
	
	refXml->get_widget( "stunCheck", stunCheck );
	refXml->get_widget( "stunAutodetectCheck", stunAutodetectCheck );
	refXml->get_widget( "stunEntry", stunEntry );
	
	tcpCheck->signal_toggled().connect( SLOT( 
		*this, &AdvancedSettings::transportChange ) );
	tlsCheck->signal_toggled().connect( SLOT( 
		*this, &AdvancedSettings::transportChange ) );
	
	stunAutodetectCheck->signal_toggled().connect( SLOT( 
		*this, &AdvancedSettings::stunAutodetectChange ) );
	
}

void AdvancedSettings::setConfig( MRef<SipSoftPhoneConfiguration *> config ){ 
	this->config = config;

	//Set the choosen network interface ...
	vector<string> ifaces = NetworkFunctions::getAllInterfaces();
	list<string> ifaceIP;
	for(unsigned int i=0; i<ifaces.size(); i++ ){
		string ip = NetworkFunctions::getInterfaceIPStr(ifaces[i]);
		#ifdef DEBUG_OUTPUT
		cout << "AdvancedSettings::setConfig - Network Interface: name = " << ifaces[i] << "; IP=" << ip << endl;
		#endif
		ifaceIP.push_back( ip );
		
	}	
	networkInterfacesCombo->set_popdown_strings( ifaceIP );
	networkInterfacesCombo->set_sensitive( ifaceIP.size() > 1 );
	//set the preferred's IP as selected ...
	if( config ) {
		string preferredIfaceIP = NetworkFunctions::getInterfaceIPStr( config->networkInterfaceName );
		networkInterfacesEntry->set_text( preferredIfaceIP );
	}
	
	udpSpin->set_value( config->inherited->localUdpPort );
	tcpSpin->set_value( config->inherited->localTcpPort );
	tlsSpin->set_value( config->inherited->localTlsPort );

	tcpCheck->set_active( config->tcp_server );
	tlsCheck->set_active( config->tls_server );

	transportChange();

	stunCheck->set_active( config->useSTUN );
	stunAutodetectCheck->set_active( config->useUserDefinedStunServer );
	stunEntry->set_text( config->userDefinedStunServer );

}

void AdvancedSettings::transportChange(){

	tlsSpin->set_sensitive( tlsCheck->get_active() );
	tcpSpin->set_sensitive( tcpCheck->get_active() );
	
	stunAutodetectChange();
}

void AdvancedSettings::stunAutodetectChange(){
	stunEntry->set_sensitive( !stunAutodetectCheck->get_active() );
	if( stunAutodetectCheck->get_active() ){
		stunEntry->set_text( "" );
	}
}

string AdvancedSettings::apply(){
	//config->networkInterfaceName = networkInterfacesCombo->
	string ipSelected = networkInterfacesEntry->get_text();
	string ifaceSel = NetworkFunctions::getInterfaceOf( ipSelected );
	#ifdef DEBUG_OUTPUT
	cout << "AdvancedSettings::apply - ip = " << ipSelected << "; iface = " << ifaceSel << endl;
	#endif
	if( ifaceSel != "" ) {
		config->networkInterfaceName = ifaceSel;
	}

	config->inherited->localUdpPort = udpSpin->get_value_as_int();
	config->inherited->localTcpPort = tcpSpin->get_value_as_int();
	config->inherited->localTlsPort = tlsSpin->get_value_as_int();

	config->tcp_server = tcpCheck->get_active();
	config->tls_server = tlsCheck->get_active();

	config->useSTUN = stunCheck->get_active();
	config->useUserDefinedStunServer = stunAutodetectCheck->get_active()
		&& stunEntry->get_text() != "";
	config->userDefinedStunServer = stunEntry->get_text();

	return "";

}
