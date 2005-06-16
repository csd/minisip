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
#include"../../../sip/SipSoftPhoneConfiguration.h"

#ifdef OLDLIBGLADEMM
#define SLOT(a,b) SigC::slot(a,b)
#define BIND SigC::bind
#else
#define SLOT(a,b) sigc::mem_fun(a,b)
#define BIND sigc::bind
#endif


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
	delete securitySettings;
	delete advancedSettings;
}

void SettingsDialog::setConfig( MRef<SipSoftPhoneConfiguration *> config ){
	this->config = config;
	generalSettings->setConfig( config );
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
	warning += securitySettings->apply();
	warning += advancedSettings->apply();
	
	config->save();

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

GeneralSettings::GeneralSettings( Glib::RefPtr<Gnome::Glade::Xml>  refXml ):
		accountsList( new AccountsList( new AccountsListColumns ) ){

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
	

	refXml->get_widget( "soundEntry", soundEntry );
	if( soundEntry == NULL ){
		exit( 1 );
	}

	accountsTreeView->set_headers_visible( true );
	accountsTreeView->set_rules_hint( true );
	accountsList->setTreeView( accountsTreeView );

	accountsAddButton->signal_clicked().connect( SLOT( *accountsList, &AccountsList::addAccount ) );
	accountsEditButton->signal_clicked().connect( 
			SLOT( *this, &GeneralSettings::editAccount ));
	accountsRemoveButton->signal_clicked().connect( 
			SLOT( *this, &GeneralSettings::removeAccount ));
	defaultButton->signal_clicked().connect( 
			SLOT( *this, &GeneralSettings::setDefaultAccount ));
	pstnButton->signal_clicked().connect( 
			SLOT( *this, &GeneralSettings::setPstnAccount ));


#ifdef IPAQ
	// Make the buttons smaller to fit the screen
	defaultButton->set_label( "Default" );
	pstnButton->set_label( "PSTN" );
#endif
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



void GeneralSettings::setConfig( MRef<SipSoftPhoneConfiguration *> config ){
	this->config = config;
	accountsList->loadFromConfig( config );
	soundEntry->set_text( config->soundDevice );
}

string GeneralSettings::apply(){
	string err;
	err += accountsList->saveToConfig( config );
	config->soundDevice = soundEntry->get_text();
	return err;

}

SecuritySettings::SecuritySettings( Glib::RefPtr<Gnome::Glade::Xml>  refXml ){
	
	refXml->get_widget( "dhCheck", dhCheck );
	refXml->get_widget( "pskCheck", pskCheck );

	refXml->get_widget( "pskEntry", pskEntry );

	refXml->get_widget( "secureCheck", secureCheck );
	
	refXml->get_widget( "secureTable", secureTable );
	
	refXml->get_widget( "kaEntry", kaEntry );
	refXml->get_widget( "kaCombo", kaCombo );

	refXml->get_widget( "pskBox", pskBox );

	
	dhCheck->signal_toggled().connect( SLOT( 
		*this, &SecuritySettings::kaChange ) );
	
	pskCheck->signal_toggled().connect( SLOT( 
		*this, &SecuritySettings::kaChange ) );

	secureCheck->signal_toggled().connect( SLOT( 
		*this, &SecuritySettings::secureChange ) );

	
	//kaCombo->set_value_in_list( true );
	kaEntry->set_editable( false );
	
}

void SecuritySettings::setConfig( MRef<SipSoftPhoneConfiguration *> config ){ 
	this->config = config;

	dhCheck->set_active( config->securityConfig.dh_enabled );
	pskCheck->set_active( config->securityConfig.psk_enabled );

	string psk( (const char *)config->securityConfig.psk, config->securityConfig.psk_length );
	pskEntry->set_text( psk );


	if( config->securityConfig.ka_type == KEY_MGMT_METHOD_MIKEY_DH ){
		kaEntry->set_text( "Diffie-Hellman" );
	}

	else if( config->securityConfig.ka_type == KEY_MGMT_METHOD_MIKEY_PSK ){
		kaEntry->set_text( "Pre-shared key" );
	}

	secureCheck->set_active( config->securityConfig.secured );
	
	kaChange();
	secureChange();

}

void SecuritySettings::kaChange(){

	pskBox->set_sensitive( pskCheck->get_active() );
	
	secureCheck->set_sensitive( pskCheck->get_active() 
			|| dhCheck->get_active() );

	if( !( pskCheck->get_active() || dhCheck->get_active() ) ){
		secureCheck->set_active( false );
	}

	kaCombo->set_sensitive( pskCheck->get_active()
			     || dhCheck->get_active() );
	
	std::list<string> list;

	if( dhCheck->get_active() ){
		list.push_back( "Diffie-Hellman" );
	}

	if( pskCheck->get_active() ){
		list.push_back( "Pre-shared key" );
	}

	if( list.size() > 0 ){
		kaCombo->set_popdown_strings( list );
	}


}

void SecuritySettings::secureChange(){
	secureTable->set_sensitive( secureCheck->get_active() );

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
		if( kaEntry->get_text() == "Pre-shared key" ){
			config->securityConfig.ka_type = KEY_MGMT_METHOD_MIKEY_PSK;
		}
		else if( kaEntry->get_text() == "Diffie-Hellman" ){
			config->securityConfig.ka_type = KEY_MGMT_METHOD_MIKEY_DH;
		}
	}

	return err;

}

AdvancedSettings::AdvancedSettings( Glib::RefPtr<Gnome::Glade::Xml>  refXml ){
	
	refXml->get_widget( "udpSpin", udpSpin );
	refXml->get_widget( "tcpSpin", tcpSpin );
	refXml->get_widget( "tlsSpin", tlsSpin );
	
	refXml->get_widget( "tcpCheck", tcpCheck );
	refXml->get_widget( "tlsCheck", tlsCheck );

	refXml->get_widget( "transportEntry", transportEntry );
	refXml->get_widget( "transportCombo", transportCombo );
	
	refXml->get_widget( "stunCheck", stunCheck );
	refXml->get_widget( "stunAutodetectCheck", stunAutodetectCheck );
	refXml->get_widget( "stunEntry", stunEntry );
	
	tcpCheck->signal_toggled().connect( SLOT( 
		*this, &AdvancedSettings::transportChange ) );
	tlsCheck->signal_toggled().connect( SLOT( 
		*this, &AdvancedSettings::transportChange ) );
	
	stunAutodetectCheck->signal_toggled().connect( SLOT( 
		*this, &AdvancedSettings::stunAutodetectChange ) );

	transportCombo->set_value_in_list( true );
	
}

void AdvancedSettings::setConfig( MRef<SipSoftPhoneConfiguration *> config ){ 
	this->config = config;

	udpSpin->set_value( config->inherited->localUdpPort );
	tcpSpin->set_value( config->inherited->localTcpPort );
	tlsSpin->set_value( config->inherited->localTlsPort );

	tcpCheck->set_active( config->tcp_server );
	tlsCheck->set_active( config->tls_server );

	transportChange();

	transportEntry->set_text( config->inherited->transport );

	stunCheck->set_active( config->useSTUN );
	stunAutodetectCheck->set_active( config->useUserDefinedStunServer );
	stunEntry->set_text( config->userDefinedStunServer );


}

void AdvancedSettings::transportChange(){

	tlsSpin->set_sensitive( tlsCheck->get_active() );
	tcpSpin->set_sensitive( tcpCheck->get_active() );
	
	std::list<string> list;

	list.push_back( "UDP" );

	if( tcpCheck->get_active() ){
		list.push_back( "TCP" );
	}

	if( tlsCheck->get_active() ){
		list.push_back( "TLS" );
	}

	transportCombo->set_popdown_strings( list );

	transportCombo->set_sensitive( list.size() > 1 );
	stunAutodetectChange();
}

void AdvancedSettings::stunAutodetectChange(){
	stunEntry->set_sensitive( !stunAutodetectCheck->get_active() );
	if( stunAutodetectCheck->get_active() ){
		stunEntry->set_text( "" );
	}
}

string AdvancedSettings::apply(){
	config->inherited->localUdpPort = udpSpin->get_value_as_int();
	config->inherited->localTcpPort = tcpSpin->get_value_as_int();
	config->inherited->localTlsPort = tlsSpin->get_value_as_int();

	config->tcp_server = tcpCheck->get_active();
	config->tls_server = tlsCheck->get_active();

	config->inherited->transport = transportEntry->get_text();

	config->useSTUN = stunCheck->get_active();
	config->useUserDefinedStunServer = stunAutodetectCheck->get_active()
		&& stunEntry->get_text() != "";
	config->userDefinedStunServer = stunEntry->get_text();
	

	return "";

}
