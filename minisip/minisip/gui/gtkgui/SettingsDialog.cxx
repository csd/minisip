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

/* Copyright (C) 2004-2007
 *
 * Authors: Erik Eliasson <eliasson@it.kth.se>
 *          Johan Bilien <jobi@via.ecp.fr>
 *          Mikael Magnusson <mikma@users.sourceforge.net>
*/

#include"SettingsDialog.h"
#include"AccountsList.h"
#include"CertificateDialog.h"
#include"TransportList.h"
#include<libminisip/gui/Gui.h>
#include<libminisip/signaling/sip/SipSoftPhoneConfiguration.h>
#include<libminisip/media/MediaCommandString.h>
#include<libminisip/media/soundcard/SoundDriverRegistry.h>

#include<libmnetutil/NetworkFunctions.h>
#include<libmcrypto/SipSimSoft.h>

#ifdef OLDLIBGLADEMM
#define SLOT(a,b) SigC::slot(a,b)
#define BIND SigC::bind
#else
#define SLOT(a,b) sigc::mem_fun(a,b)
#define BIND sigc::bind
#endif

using namespace std;

SettingsDialog::SettingsDialog( Glib::RefPtr<Gnome::Glade::Xml>  refXml,
				Glib::RefPtr<TransportList> transportList ){
	refXml->get_widget( "settingsDialog", dialogWindow );

	Gtk::Button * settingsOkButton;
	Gtk::Button * settingsCancelButton;
	
	/* Connect the Ok and cancel buttons */
	
	refXml->get_widget( "settingsOkButton", settingsOkButton );
	refXml->get_widget( "settingsCancelButton", settingsCancelButton );

	settingsOkButton->signal_clicked().connect( SLOT( *this, &SettingsDialog::accept ) );
	settingsCancelButton->signal_clicked().connect( SLOT( *this, &SettingsDialog::reject ) );


	generalSettings = new GeneralSettings( refXml );
	mediaSettings = new MediaSettings( refXml );
	deviceSettings = new DeviceSettings( refXml );
	advancedSettings = new AdvancedSettings( refXml, transportList );
	sipSettings = new SipSettings( refXml );


	dialogWindow->hide();
#ifdef IPAQ
//	dialogWindow->maximize();
	dialogWindow->set_type_hint( Gdk::WINDOW_TYPE_HINT_NORMAL );
#endif
}

SettingsDialog::~SettingsDialog(){
	delete generalSettings;
	delete mediaSettings;
	delete deviceSettings;
	delete advancedSettings;
	delete sipSettings;
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
	deviceSettings->setConfig( config );
	advancedSettings->setConfig( config );
	sipSettings->setConfig( config );
	
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
	warning += deviceSettings->apply();
	warning += advancedSettings->apply();
	warning += sipSettings->apply();
	
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


}

MediaSettings::~MediaSettings(){
	delete codecColumns;
}

void MediaSettings::setConfig( MRef<SipSoftPhoneConfiguration *> config ){
	list<string>::iterator iC;
	Gtk::TreeModel::iterator listIterator;
	this->config = config;

	codecList->clear();

	for( iC = config->audioCodecs.begin(); iC != config->audioCodecs.end();
			iC ++ ){
		listIterator = codecList->append();
		(*listIterator)[codecName] = 
			Glib::locale_to_utf8( *iC );
		(*listIterator)[codecEnabled] = true;
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
	config->audioCodecs.clear();

	for( iC = codecList->children().begin(); iC ; iC ++ ){
		config->audioCodecs.push_back( 
			Glib::locale_from_utf8( (*iC)[codecName] ) );
	}

	return "";	
}


DeviceSettings::DeviceSettings( Glib::RefPtr<Gnome::Glade::Xml>  refXml ){
	
	refXml->get_widget( "videoEntry", videoEntry );
	 refXml->get_widget ( "videoEntry2", videoEntry2 );

	 refXml->get_widget ( "displayFrameSize", displayFrameSize );
	 refXml->get_widget ( "displayFrameRate", displayFrameRate );
	
	
	refXml->get_widget( "soundInputEntry", soundInputEntry );
	refXml->get_widget( "soundOutputEntry", soundOutputEntry );

	refXml->get_widget( "soundInputList", soundInputView );
	refXml->get_widget( "soundOutputList", soundOutputView );

	refXml->get_widget( "videoLabel", videoLabel );
	refXml->get_widget( "videoDeviceLabel", videoDeviceLabel );
	


	refXml->get_widget( "spaudioCheck", spaudioCheck );

#ifndef VIDEO_SUPPORT
	videoEntry->hide();
	videoEntry2->hide();
	videoLabel->hide();
	videoDeviceLabel->hide();
#endif

	deviceColumns = new Gtk::TreeModelColumnRecord();
	deviceColumns->add( deviceName );
	deviceColumns->add( deviceDescription );

	soundInputList = Gtk::ListStore::create( *deviceColumns );
	soundOutputList = Gtk::ListStore::create( *deviceColumns );

	soundInputView->set_model( soundInputList );
	soundOutputView->set_model( soundOutputList );

	soundInputView->signal_changed().connect( SLOT( *this, &DeviceSettings::soundInputChange ) );
	soundOutputView->signal_changed().connect( SLOT( *this, &DeviceSettings::soundOutputChange ) );

	Gtk::CellRendererText* crt;
	crt = new Gtk::CellRendererText();
	soundInputView->pack_end(*manage(crt), true);
	soundInputView->add_attribute(crt->property_text(), deviceDescription);

	//crt = new Gtk::CellRendererText();
	soundOutputView->pack_end(*manage(crt), true);
	soundOutputView->add_attribute(crt->property_text(), deviceDescription);
	delete crt;
}

DeviceSettings::~DeviceSettings(){
	delete deviceColumns;
}

void DeviceSettings::setConfig( MRef<SipSoftPhoneConfiguration *> config ){
	list<string>::iterator iC;
	Gtk::TreeModel::iterator listIterator;
	this->config = config;
#ifdef VIDEO_SUPPORT

//cout <<"===========================================================================================> setting gui videoDevices :: "<< config->videoDevice << " , "<< config->videoDevice2 <<endl;  

	videoEntry->set_text( config->videoDevice );


	videoEntry2->set_text(config->videoDevice2);

	displayFrameSize->set_text(config->displayFrameSize);
	displayFrameRate->set_text(config->displayFrameRate);

#endif

	if( config->soundIOmixerType == "spatial" ){
		spaudioCheck->set_active( true );
	}


	MRef<SoundDriverRegistry*> registry = SoundDriverRegistry::getInstance();
	std::vector<SoundDeviceName> names = registry->getAllDeviceNames();

	vector<SoundDeviceName>::iterator iter;
	vector<SoundDeviceName>::iterator end = names.end();

	soundInputList->clear();
	soundOutputList->clear();

	listIterator = soundInputList->append();
        (*listIterator)[deviceName] = "manual:";
        (*listIterator)[deviceDescription] = "Manual entry...";
	soundInputView->set_active(*listIterator);
	soundInputEntry->set_text( config->soundDeviceIn );

	listIterator = soundOutputList->append();
        (*listIterator)[deviceName] = "manual:";
        (*listIterator)[deviceDescription] = "Manual entry...";
	soundOutputView->set_active(*listIterator);
	soundOutputEntry->set_text( config->soundDeviceOut );

	for( iter = names.begin(); iter != end; iter++ ){
		if( iter->getMaxInputChannels() > 0 ){
			listIterator = soundInputList->append();
			(*listIterator)[deviceName] = iter->getName();
			(*listIterator)[deviceDescription] = Glib::locale_to_utf8( iter->getDescription() );
			if( config->soundDeviceIn == iter->getName() )
				soundInputView->set_active(*listIterator);
		}

		if( iter->getMaxOutputChannels() > 0 ){
			listIterator = soundOutputList->append();
			(*listIterator)[deviceName] = iter->getName();
			(*listIterator)[deviceDescription] = Glib::locale_to_utf8( iter->getDescription() );
			if( config->soundDeviceOut == iter->getName() )
				soundOutputView->set_active(*listIterator);
		}
	}

}

void DeviceSettings::soundInputChange(){
	Gtk::TreeModel::iterator iter = soundInputView->get_active();
	if( !iter )
		return;

	Gtk::TreeModel::Row row = *iter;
	const string &name = row[deviceName];

	if( name == "manual:" ){
		soundInputEntry->set_sensitive( true );
	}
	else{
		soundInputEntry->set_sensitive( false );
		soundInputEntry->set_text( name );
	}
}

void DeviceSettings::soundOutputChange(){
	Gtk::TreeModel::iterator iter = soundOutputView->get_active();
	if( !iter )
		return;

	Gtk::TreeModel::Row row = *iter;
	const string &name = row[deviceName];

	if( name == "manual:" ){
		soundOutputEntry->set_sensitive( true );
	}
	else{
		soundOutputEntry->set_sensitive( false );
		soundOutputEntry->set_text( name );
	}
}

string DeviceSettings::apply(){
	Gtk::TreeModel::iterator iC;
	config->soundDeviceIn = soundInputEntry->get_text();
	config->soundDeviceOut = soundOutputEntry->get_text();

	if( spaudioCheck->get_active() ){
		config->soundIOmixerType = "spatial";
	}
	else{
		config->soundIOmixerType = "simple";
	}
	
#ifdef VIDEO_SUPPORT
	config->videoDevice = videoEntry->get_text();
	config->videoDevice2 = videoEntry2->get_text();

	config->displayFrameSize = displayFrameSize->get_text();
	config->displayFrameRate = displayFrameRate->get_text();

	cout << "========================================================================= "<< config->videoDevice2 ;
#endif
	return "";	
}


SecuritySettings::SecuritySettings( Glib::RefPtr<Gnome::Glade::Xml> refXml,
				    CertificateDialog * certDialog):
		certDialog( certDialog ),
		identity( NULL ){
	
	refXml->get_widget( "dhCheck", dhCheck );
	refXml->get_widget( "certCheck", certCheck );
	refXml->get_widget( "pskCheck", pskCheck );

	refXml->get_widget( "pskEntry", pskEntry );

	refXml->get_widget( "secureCheck", secureCheck );
	
//	refXml->get_widget( "secureTable", secureTable );
	
//	refXml->get_widget( "kaEntry", kaEntry );
//	refXml->get_widget( "kaCombo", kaCombo );

//	refXml->get_widget( "pskBox", pskBox );
	refXml->get_widget( "pskRadio", pskRadio );
	refXml->get_widget( "dhRadio", dhRadio );
	refXml->get_widget( "dhhmacRadio", dhhmacRadio );
	refXml->get_widget( "rsarRadio", rsarRadio );
	
	refXml->get_widget( "kaTypeLabel", kaTypeLabel );

	refXml->get_widget( "pskLabel", pskLabel );
	refXml->get_widget( "certificateButton", certificateButton );

	
	dhConn = dhCheck->signal_toggled().connect( SLOT( 
		*this, &SecuritySettings::kaChange ) );
	
	pskConn = pskCheck->signal_toggled().connect( SLOT( 
		*this, &SecuritySettings::kaChange ) );

	secureConn = secureCheck->signal_toggled().connect( SLOT( 
		*this, &SecuritySettings::secureChange ) );

	certificateConn = certificateButton->signal_clicked().connect( SLOT( *certDialog, &CertificateDialog::run ) );
	
	//kaCombo->set_value_in_list( true );
//	kaEntry->set_editable( false );
	
	reset();
}

SecuritySettings::~SecuritySettings(){
	// Need to disconnect the signals since the GTK dialog
	// will be reused by other instances of SecuritySettings.
	dhConn.disconnect();
	pskConn.disconnect();
	secureConn.disconnect();
	certificateConn.disconnect();
}

void SecuritySettings::reset(){
	identity = NULL;

	dhCheck->set_active( false );
	certCheck->set_active( false );
	pskCheck->set_active( false );

	pskEntry->set_text( "" );
	dhRadio->set_active( true );

	secureCheck->set_active( false );

	MRef<CertificateSet*> caDb = CertificateSet::create();
	MRef<CertificateChain*> certChain = CertificateChain::create();

	certDialog->setRootCa( caDb );
	certDialog->setCertChain( certChain );

	kaChange();
	secureChange();
}

void SecuritySettings::setConfig( MRef<SipIdentity *> theIdentity ){
	if( !theIdentity ){
		reset();
		return;
	}

	identity = theIdentity;

	dhCheck->set_active( identity->dhEnabled );
	certCheck->set_active( identity->checkCert );
	pskCheck->set_active( identity->pskEnabled );

//	string psk( (const char *)config->securityConfig.psk, config->securityConfig.psk_length );
	string psk=identity->getPsk();
	pskEntry->set_text( psk );


	if( identity->ka_type == KEY_MGMT_METHOD_MIKEY_DH ){
		dhRadio->set_active( true );
	}
	else if( identity->ka_type == KEY_MGMT_METHOD_MIKEY_PSK ){
		pskRadio->set_active( true );
	}
	else if( identity->ka_type == KEY_MGMT_METHOD_MIKEY_DHHMAC ){
		dhhmacRadio->set_active( true );
	}
	else if( identity->ka_type == KEY_MGMT_METHOD_MIKEY_RSA_R ){
		rsarRadio->set_active( true );
	}

	secureCheck->set_active( identity->securityEnabled );

	MRef<CertificateSet*> caDb =
		identity->getSim()->getCAs()->clone();

	MRef<CertificateChain*> certChain = 
		identity->getSim()->getCertificateChain()->clone();

	certDialog->setRootCa( caDb );
	certDialog->setCertChain( certChain );

	kaChange();
	secureChange();

}

void SecuritySettings::kaChange(){

	pskEntry->set_sensitive( pskCheck->get_active() );
	
	pskLabel->set_sensitive( pskCheck->get_active() );

	secureCheck->set_sensitive( pskCheck->get_active() 
			|| dhCheck->get_active() );

	if( !( pskCheck->get_active() || dhCheck->get_active() ) ){
		secureCheck->set_active( false );
	}

	pskRadio->set_sensitive( secureCheck->get_active() && 
			         pskCheck->get_active() );
	dhRadio->set_sensitive( secureCheck->get_active() && 
			        dhCheck->get_active() );
	dhhmacRadio->set_sensitive( secureCheck->get_active() && 
			        pskCheck->get_active() );
	rsarRadio->set_sensitive( secureCheck->get_active() && 
			        dhCheck->get_active() );

	if( dhCheck->get_active() && ! pskCheck->get_active() ){
		if( !rsarRadio->get_active() ){
			dhRadio->set_active( true );
		}
	}

	if( pskCheck->get_active() && ! dhCheck->get_active() ){
		if( !dhhmacRadio->get_active() ){
			pskRadio->set_active( true );
		}
	}

	certCheck->set_sensitive( dhCheck->get_active() );
}

void SecuritySettings::secureChange(){
	kaTypeLabel->set_sensitive( secureCheck->get_active() );
	pskRadio->set_sensitive( secureCheck->get_active() && 
			         pskCheck->get_active() );
	dhRadio->set_sensitive( secureCheck->get_active() && 
			        dhCheck->get_active() );
	dhhmacRadio->set_sensitive( secureCheck->get_active() && 
			        pskCheck->get_active() );
	rsarRadio->set_sensitive( secureCheck->get_active() && 
			        dhCheck->get_active() );

}


string SecuritySettings::apply(){
	string err;
	if( dhCheck->get_active() ){
		identity->getSim()->getCertificateChain()->lock();
		if( identity->getSim()->getCertificateChain()->isEmpty() ){
			err += "You have selected the Diffie-Hellman key agreement\n"
		       "but have not selected a certificate file.\n"
		       "The D-H key agreement has been disabled.";
			dhCheck->set_active( false );
		}
		
		else if( !identity->getSim()->getCertificateChain()->getFirst()->hasPk() ){
			err += "You have selected the Diffie-Hellman key agreement\n"
		       "but have not selected a private key file.\n"
		       "The D-H key agreement has been disabled.";
			dhCheck->set_active( false );
		}
		identity->getSim()->getCertificateChain()->unlock();
	}

	identity->dhEnabled = dhCheck->get_active();
	identity->pskEnabled = pskCheck->get_active();
	identity->checkCert = certCheck->get_active();


	string s = pskEntry->get_text();
        const char * psk = s.c_str();

#if 0	
        if( config->securityConfig.psk != NULL )
                delete [] config->securityConfig.psk;
        config->securityConfig.psk = new unsigned char[psk_length];
        memcpy( config->securityConfig.psk, psk, psk_length );
        config->securityConfig.psk_length = psk_length;
#endif	
	identity->setPsk(string(psk));


	 identity->securityEnabled = secureCheck->get_active();
	if( identity ){
		identity->securityEnabled = secureCheck->get_active();
	}

	if( identity->securityEnabled ){
		if( pskRadio->get_active() ){
			identity->ka_type = KEY_MGMT_METHOD_MIKEY_PSK;
		}
		else if( dhRadio->get_active() ){
			identity->ka_type = KEY_MGMT_METHOD_MIKEY_DH;
		}
		else if( dhhmacRadio->get_active() ){
			identity->ka_type = KEY_MGMT_METHOD_MIKEY_DHHMAC;
		}
		else if( rsarRadio->get_active() ){
			identity->ka_type = KEY_MGMT_METHOD_MIKEY_RSA_R;
		}
	}

	if( identity->getSim() ){
		identity->getSim()->setCAs( certDialog->getRootCa() );
		identity->getSim()->setCertificateChain( certDialog->getCertChain() );
	}
	else{
		MRef<SipSim*> sim =
			new SipSimSoft( certDialog->getCertChain(),
					certDialog->getRootCa() );
		identity->setSim( sim );
	}

	return err;

}

AdvancedSettings::AdvancedSettings( Glib::RefPtr<Gnome::Glade::Xml>  refXml,
				    Glib::RefPtr<TransportList> transportList ): transportList( transportList ){
	refXml->get_widget( "networkInterfacesCombo", networkInterfacesCombo );
	refXml->get_widget( "networkInterfacesEntry", networkInterfacesEntry );
		
	refXml->get_widget( "ipv4Radio", ipv4Radio );
	refXml->get_widget( "ipv46Radio", ipv46Radio );

	refXml->get_widget( "transportView", transportView );

	refXml->get_widget( "sipSpin", sipSpin );
	refXml->get_widget( "sipsSpin", sipsSpin );
	
	refXml->get_widget( "stunCheck", stunCheck );
	refXml->get_widget( "stunAutodetectCheck", stunAutodetectCheck );
	refXml->get_widget( "stunEntry", stunEntry );

	transportView->set_model( transportList );
	TransportListColumns *columns = transportList->getColumns();

	transportView->append_column_editable( "Enabled", columns->enabled );
	transportView->append_column( "Name", columns->name );
	transportView->append_column( "Scheme", columns->scheme );
	transportView->append_column( "Protocol", columns->protocol );
	transportView->append_column( "Description", columns->description );
	
	stunAutodetectCheck->signal_toggled().connect( SLOT( 
		*this, &AdvancedSettings::stunAutodetectChange ) );
	
}

void AdvancedSettings::setConfig( MRef<SipSoftPhoneConfiguration *> config ){ 
	this->config = config;

	transportList->loadFromConfig( config );

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

	if( config->useIpv6 ){
		ipv46Radio->set_active( true );
	}
	else{
		ipv4Radio->set_active( true );
	}
	
	sipSpin->set_value( config->sipStack->getStackConfig()->preferedLocalSipPort );
	sipsSpin->set_value( config->sipStack->getStackConfig()->preferedLocalSipsPort );

	transportChange();

	stunCheck->set_active( config->useSTUN );
	stunAutodetectCheck->set_active( !config->useUserDefinedStunServer );
	stunEntry->set_text( config->userDefinedStunServer );

}

void AdvancedSettings::transportChange(){

	stunAutodetectChange();
}

void AdvancedSettings::stunAutodetectChange(){
	stunEntry->set_sensitive( !stunAutodetectCheck->get_active() );
	if( stunAutodetectCheck->get_active() ){
		stunEntry->set_text( "" );
	}
}

string AdvancedSettings::apply(){
	string err;

	//config->networkInterfaceName = networkInterfacesCombo->
	string ipSelected = networkInterfacesEntry->get_text();
	string ifaceSel = NetworkFunctions::getInterfaceOf( ipSelected );
	#ifdef DEBUG_OUTPUT
	cout << "AdvancedSettings::apply - ip = " << ipSelected << "; iface = " << ifaceSel << endl;
	#endif
	if( ifaceSel != "" ) {
		config->networkInterfaceName = ifaceSel;
	}

	config->useIpv6 = ipv46Radio->get_active();

	config->sipStack->getStackConfig()->preferedLocalSipPort = sipSpin->get_value_as_int();
	config->sipStack->getStackConfig()->preferedLocalSipsPort = sipsSpin->get_value_as_int();

	config->useSTUN = stunCheck->get_active();
	config->useUserDefinedStunServer = !stunAutodetectCheck->get_active()
		&& stunEntry->get_text() != "";
	config->userDefinedStunServer = stunEntry->get_text();

	err += transportList->saveToConfig( config );
	return err;
}

// SipSettings
SipSettings::SipSettings( Glib::RefPtr<Gnome::Glade::Xml>  refXml ){
	refXml->get_widget( "anatCheck", anatCheck );
	refXml->get_widget( "100relCheck", use100RelCheck );
}

void SipSettings::setConfig( MRef<SipSoftPhoneConfiguration *> config ){ 
	this->config = config;

	MRef<SipStackConfig*> stackConfig = config->sipStack->getStackConfig();

	anatCheck->set_active( config->useAnat );
	use100RelCheck->set_active( stackConfig->use100Rel );
}

string SipSettings::apply(){
	MRef<SipStackConfig*> stackConfig = config->sipStack->getStackConfig();

	config->useAnat = anatCheck->get_active();
	stackConfig->use100Rel = use100RelCheck->get_active();

	return "";
}
