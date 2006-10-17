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

#include"MinisipMainWindowWidget.h"
#include"SettingsDialog.h"
#include<qpushbutton.h>
#include<qmessagebox.h>
#include<libmsip/SipKeyAgreement.h>
#include<libmsip/SipMessageTransport.h>
#include<libmsip/SipCommandString.h>
#include"../../../sip/DefaultDialogHandler.h"
#include<libmutil/stringutils.h>
#include<libmutil/XMLParser.h>
#include<libmutil/cert.h>
#include<libmnetutil/IP4Address.h>
#ifdef HAVE_LIBASOUND
#include"../../../soundcard/AlsaCard.h"
#include"../../../soundcard/SoundIO.h"
#endif

SettingsDialog::SettingsDialog(QWidget *parent, const char *name, bool destroy) : 
		QTabDialog(parent,name, true, destroy?Qt::WDestructiveClose:0),
		certDialog( this ),
		generalWidget(this, name), 
		securityWidget(this, name), 
#ifndef DISABLE_PHONEBOOK
		phoneBooksWidget( this, name), 
#endif
		advancedWidget(this, name)
{
	addTab( &generalWidget, "General");
	addTab( &securityWidget, "Security");
#ifndef DISABLE_PHONEBOOK
	addTab( &phoneBooksWidget, "Phone books");
#endif
	addTab( &stunWidget, "STUN");
	addTab( &advancedWidget, "Advanced");

	setCancelButton();
}

SettingsDialog::~SettingsDialog(){
}

void SettingsDialog::setConfig(MRef<SipSoftPhoneConfiguration *> config){
	this->config=config;
	generalWidget.setConfig(config);
	securityWidget.setConfig(config);
#ifndef DISABLE_PHONEBOOK
	phoneBooksWidget.setConfig(config);
#endif
	stunWidget.setConfig(config);
	advancedWidget.setConfig(config);
	certDialog.setCertChain(config->inherited.cert);
	certDialog.setRootCa(config->inherited.cert_db);
	fprintf( stderr, "Exiting setConfig\n");
}

void SettingsDialog::show(){
	generalWidget.backupSettings();
	stunWidget.backupSettings();
	advancedWidget.backupSettings();
	securityWidget.backupSettings();
	QTabDialog::show();	
}

void SettingsDialog::accept(){
	string errReport = advancedWidget.apply() + generalWidget.apply() + securityWidget.apply() + stunWidget.apply(); 

#ifndef DISABLE_PHONEBOOK
	errReport = errReport + phoneBooksWidget.apply(parser);
#endif
	if (errReport.length()>0){
		
		QMessageBox msg("Apply warnings!", ("The warnings were reported when applying changes:\n"+errReport).c_str(),QMessageBox::Warning,QMessageBox::Ok,QMessageBox::NoButton,QMessageBox::NoButton,this);
		msg.exec();
	}
	config->save();
	QDialog::accept();
}

void SettingsDialog::reject(){
	generalWidget.restoreSettings();
	stunWidget.restoreSettings();
	advancedWidget.restoreSettings();
	securityWidget.restoreSettings();
//	hide();
	QDialog::reject();
	//TODO: add restore for phonebook settings
	
}

void SettingsDialog::showCertDialog(){
	certDialog.show();
}


GeneralTabWidget::GeneralTabWidget(QWidget *parent, const char *name): 
		QWidget(parent, name), 
		layout( this, 2, 1),

		groupboxSip(2,Qt::Horizontal,"SIP",this),
		
		autodetectRadioGroup(),

		labelUserURI( "User's SIP address:", &groupboxSip, "sipuri"), 
		editUserURI(&groupboxSip),
	
		labelProxyUsername( "User's proxy username:", &groupboxSip), 
		editProxyUsername(&groupboxSip),
		
		labelProxyPassword( "User's proxy password:", &groupboxSip), 
		editProxyPassword(&groupboxSip),
		
		radioAutodetect("Autodetect proxy",&groupboxSip),
		labelEmpty( "", &groupboxSip),
		
		radioManualProxy("Manual proxy:",&groupboxSip),
		editManualProxy(&groupboxSip),

		checkRegister("Register to proxy", &groupboxSip),
	
		groupboxDevices(2,Qt::Horizontal,"Devices", this),
		
		labelSoundDevice("Sound I/O device:",&groupboxDevices),
		comboboxSoundDevice(&groupboxDevices),
	
		labelNIC("Network interface:",&groupboxDevices),
		comboboxNIC(&groupboxDevices)
{
	layout.addWidget(&groupboxSip, 0, 0);
	layout.addWidget(&groupboxDevices, 1, 0);
			
#ifndef OPIE
	editUserURI.setMinimumWidth(250);
#endif

	tabDialog = (SettingsDialog *)parent;
	
}

GeneralTabWidget::~GeneralTabWidget(){
}

void GeneralTabWidget::setConfig(MRef<SipSoftPhoneConfiguration *> config){
	this->config = config;
	editUserURI.setText((config->inherited.userUri).c_str());

	editProxyUsername.setText((config->inherited.proxyUsername).c_str());

	editProxyPassword.setEchoMode(QLineEdit::Password);
	editProxyPassword.setText((config->inherited.proxyPassword).c_str());

	autodetectRadioGroup.insert(&radioAutodetect,1);
	radioAutodetect.setChecked(config->autodetectProxy);
	connect(&radioAutodetect, SIGNAL(stateChanged(int)), this, SLOT(autodetectStateChanged(int)) );
	
	autodetectRadioGroup.insert(&radioManualProxy,2);
	autodetectRadioGroup.setRadioButtonExclusive(true);
	if(config->inherited.proxyAddr!=NULL)
		editManualProxy.setText(config->inherited.proxyAddr->getString().c_str());
	
	checkRegister.setChecked(config->doRegister);
	
#ifdef HAVE_LIBASOUND
	list<AlsaCard *> cards = AlsaCard::getCardList();
	list<AlsaCard *>::iterator i;
	int j = 0;
	
	comboboxSoundDevice.clear();
	
	for( i = cards.begin(); i != cards.end(); i++ ){
		comboboxSoundDevice.insertItem((*i)->getCardName().c_str());
		if( config->soundcard != NULL && 
		    config->soundcard->getDevice() == (string("hw:")+itoa(j)) ){
			comboboxSoundDevice.setCurrentItem(j);
		}
		j++;
	}
	
#endif
	checkStates();

}

void GeneralTabWidget::backupSettings(){
//	cerr << "Saving GeneralTabWidget settings"<< endl;
	userUristring = editUserURI.text();
	proxyUsernamestring = editProxyUsername.text();
	proxyPasswordstring = editProxyPassword.text();
	autodetectProxy = radioAutodetect.isChecked();
	manualProxystring = editManualProxy.text();
	doRegister = checkRegister.isChecked();
	soundDeviceIndex=comboboxSoundDevice.currentItem();
	nicDeviceIndex= comboboxNIC.currentItem();
}

void GeneralTabWidget::restoreSettings(){
//	cerr << "Restoring GeneralTabWidget settings"<< endl;
	editUserURI.setText(userUristring);
	editProxyUsername.setText(proxyUsernamestring);
	editProxyPassword.setText(proxyPasswordstring);
	radioAutodetect.setChecked(autodetectProxy);
	editManualProxy.setText(manualProxystring);
	checkRegister.setChecked(doRegister);
	
	comboboxSoundDevice.setCurrentItem(soundDeviceIndex);
	comboboxNIC.setCurrentItem(nicDeviceIndex);
}

string GeneralTabWidget::apply(){
	string ret = "";

	if (editUserURI.text() != userUristring){
		//sipphone->setUserURI(editUserURI.text().ascii());
		config->inherited.userUri = editUserURI.text().ascii();
		//cerr << "Changing user uri setting to "<<  editUserURI.text();
	}
	if (editProxyUsername.text() != proxyUsernamestring){
		//sipphone->setProxyUsername(editProxyUsername.text().ascii());
		config->inherited.proxyUsername = editProxyUsername.text().ascii();
//		cerr << "Changing proxy username setting to "<< editProxyUsername.text();
	}
	if (editProxyPassword.text()!= proxyPasswordstring){
		//sipphone->setProxyPassword(editProxyPassword.text().ascii());
		config->inherited.proxyPassword = editProxyPassword.text().ascii();
//		cerr << "Changing proxy password setting to "<< editProxyPassword.text();
	}
	
	if (radioAutodetect.isChecked() != autodetectProxy){
		config->autodetectProxy= radioAutodetect.isChecked();
	}
	
	if (editManualProxy.text() != manualProxystring){
		try{
			IPAddress *ip;
			ip = new IP4Address(string(editManualProxy.text().ascii()));
			if( config->inherited.proxyAddr != NULL )
				delete config->inherited.proxyAddr;
			config->inherited.proxyAddr = ip;
		} catch (IPAddressHostNotFoundException & exc){
			cerr << "Could not resolve proxy address:";
			cerr << exc->what() << endl;
		}

	}
	
	if (checkRegister.isChecked()!= doRegister){
//		sipphone->setDoRegisterToProxy(checkRegister.isChecked());
		config->doRegister = checkRegister.isChecked();
		if (config->doRegister){
			CommandString reg("",SipCommandString::proxy_register);
//			if (!config->dialogContainer.isNull()){
			if (!config->sip.isNull()){
                                SipSMCommand cmd(reg, SipSMCommand::remote, SipSMCommand::TU);
				//config->dialogContainer->handleCommand(cmd);
				config->sip->handleCommand(cmd);
                        }
		}
	}

		
	//	cerr << "Register setting changed to "<< checkRegister.isChecked() << endl;
	if (comboboxSoundDevice.currentItem()!= soundDeviceIndex){
#ifndef HAVE_LIBASOUND
		cerr << "WARNING: UNHANDLED: Sound device changed to "<<comboboxSoundDevice.currentText()<< endl;
	}
#else
		ret += "WARNING: Changing the sound device requires to restart the application\n";
	}
#endif
	
	if (comboboxNIC.currentItem()!=nicDeviceIndex)
		cerr << "WARNING: UNHANDLED: NIC device changed to "<< comboboxNIC.currentText()<< endl;
	
	return ret; //"Error from GTW\n";	
}

void GeneralTabWidget::checkStates(){
	if (!radioManualProxy.isChecked()){
		radioAutodetect.setChecked(true);
		editManualProxy.setEnabled(false);
	}
}

void GeneralTabWidget::autodetectStateChanged(int state){
	int32_t toState=false;
	switch(state){
		case QButton::On:
			toState=false;
			break;
		case QButton::Off:
			toState=true;
			break;
		case QButton::NoChange:
			return;
	}
	
	editManualProxy.setEnabled(toState);
	radioManualProxy.setChecked(toState);
}

SecurityTabWidget::SecurityTabWidget(QWidget *parent, const char *name): QWidget(parent, name), 
		layout( this, 3, 1),
		
#ifndef OPIE
		groupboxOutGoing(3,Qt::Horizontal,"Outgoing Calls", this),
#else
		groupboxOutGoing(2,Qt::Horizontal,"Outgoing Calls", this),
#endif
		checkSecured("Use secured outgoing calls if possible", &groupboxOutGoing),
		labelOGEmpty("", &groupboxOutGoing ),
#ifndef OPIE
		labelOGEmpty2("", &groupboxOutGoing ),
		labelKaType("Key agreement for outgoing calls",&groupboxOutGoing),
#endif
		comboboxKaType(false,&groupboxOutGoing),

		//layoutDH(&groupboxDH,5,4), 
#ifndef OPIE
		groupboxDH(3,Qt::Horizontal,"Diffie-Hellman", this),
#else
		groupboxDH(2,Qt::Horizontal,"Diffie-Hellman", this),
#endif
#ifndef OPIE
		checkDH("Enable Diffie-Hellman key agreement", &groupboxDH),
#else
		checkDH("Diffie-Hellman", &groupboxDH),
#endif
		labelDHEmpty("",&groupboxDH),
#ifndef OPIE
		labelDHEmpty2("",&groupboxDH),
#endif
		buttonCert( "Certificate settings...", &groupboxDH ),
#ifndef OPIE
		groupboxPSK(3,Qt::Horizontal,"Pre-Shared Key", this),
#else
		groupboxPSK(2,Qt::Horizontal,"Pre-Shared Key", this),
#endif
#ifndef OPIE
		checkPSK("Enable Pre-Shared Key key agreement", &groupboxPSK),
#else
		checkPSK("Pre-Shared Key", &groupboxPSK),
#endif
		labelPSKEmpty("", &groupboxPSK ),
#ifndef OPIE
		labelPSKEmpty2("", &groupboxPSK ),
#endif
		labelPSK("Pre-Shared Key", &groupboxPSK),
		editPSK(&groupboxPSK)
	
	
{
	// Outgoing

	layout.addWidget(&groupboxOutGoing, 0, 0);
	connect(&checkSecured, SIGNAL(stateChanged(int)), this, SLOT(checkSecuredPressed(int)) );
	//layout.addWidget(&labelKaType, 1,0);
	//layout.addWidget(&comboboxKaType, 1,1);
	//

	tabDialog = (SettingsDialog *)parent;
}

SecurityTabWidget::~SecurityTabWidget(){
}



void SecurityTabWidget::setConfig(MRef<SipSoftPhoneConfiguration *> config){
	this->config = config;
	checkSecured.setChecked(config->inherited.secured);

	// Diffie-Hellman
	layout.addWidget(&groupboxDH, 1,0);
	connect(&checkDH, SIGNAL(stateChanged(int)), this, SLOT(checkDHPressed(int)) );
	checkDH.setChecked(config->inherited.dh_enabled);

	connect(&buttonCert, SIGNAL(clicked()), tabDialog, 
		SLOT( showCertDialog() ) );

	// Pre-Shared Key
	layout.addWidget(&groupboxPSK, 2, 0);
	connect(&checkPSK, SIGNAL(stateChanged(int)), this, SLOT(checkPSKPressed(int)) );
	
	checkPSK.setChecked(config->inherited.psk_enabled);
	
	string psk( (const char *)config->inherited.psk, config->inherited.psk_length );	
	editPSK.setText(psk.c_str());
	
	checkStates();
#if (QT_VERSION > 0x030000)
	if( config->inherited.ka_type == KEY_MGMT_METHOD_MIKEY_DH )
		comboboxKaType.setCurrentText( "Diffie-Hellman" );
	else if( config->inherited.ka_type == KEY_MGMT_METHOD_MIKEY_PSK )
		comboboxKaType.setCurrentText( "Pre-Shared Keys" );
#else
	if( config->inherited.psk_enabled && config->inherited.dh_enabled ){
		if( config->inherited.ka_type == KEY_MGMT_METHOD_MIKEY_DH )
			comboboxKaType.setCurrentItem( 1 );
		else if( config->inherited.ka_type == KEY_MGMT_METHOD_MIKEY_PSK )
			comboboxKaType.setCurrentItem( 0 );
	}
#endif
}

void SecurityTabWidget::backupSettings(){
	doSecured = checkSecured.isChecked();
	doDH = checkDH.isChecked();
	doPSK = checkPSK.isChecked();
	pskstring = editPSK.text();
	kaTypeSelected = comboboxKaType.currentItem();
}

void SecurityTabWidget::restoreSettings(){
	checkSecured.setChecked(doSecured);
	checkDH.setChecked(doDH);
	checkPSK.setChecked(doPSK);
	editPSK.setText(pskstring);
	comboboxKaType.setCurrentItem(kaTypeSelected);
}

void SecurityTabWidget::checkStates(){
	bool secured_checked = checkSecured.isChecked();
	bool dh_checked = checkDH.isChecked();
	bool psk_checked = checkPSK.isChecked();
	


	comboboxKaType.clear();
	if( psk_checked )
		comboboxKaType.insertItem("Pre-Shared Keys");
	if( dh_checked )
		comboboxKaType.insertItem("Diffie-Hellman");


	labelPSK.setEnabled(psk_checked);
	editPSK.setEnabled(psk_checked);

	checkSecured.setEnabled( dh_checked || psk_checked );

//	if( !(dh_checked || psk_checked) ){
//		checkSecured.setChecked(false);
//	}

#ifndef OPIE
	labelKaType.setEnabled( dh_checked || psk_checked );
#endif
	comboboxKaType.setEnabled( (dh_checked || psk_checked) && secured_checked );
}

void SecurityTabWidget::checkSecuredPressed(int state){
	checkStates();
}

void SecurityTabWidget::checkDHPressed(int state){
	checkStates();
}

void SecurityTabWidget::checkPSKPressed(int state){
	checkStates();
}

string SecurityTabWidget::apply(){
	string error = "";
	
	if (checkSecured.isChecked()!= doSecured){
		config->inherited.secured = (checkDH.isChecked() || checkPSK.isChecked()) && checkSecured.isChecked();
	}
	
	if (checkPSK.isChecked()!= doPSK){
		config->inherited.psk_enabled = checkPSK.isChecked();
	}

	if (checkDH.isChecked()!= doDH){
		config->inherited.dh_enabled = checkDH.isChecked();
	}
	
	if (editPSK.text() != pskstring){
		string s = editPSK.text().ascii();
		unsigned char * psk = (unsigned char *)s.c_str();
		unsigned int psk_length = s.size();
		if( config->inherited.psk != NULL )
			delete [] config->inherited.psk;
		config->inherited.psk = new unsigned char[psk_length];
		memcpy( config->inherited.psk, psk, psk_length );
		config->inherited.psk_length = psk_length;
	}

	if (checkSecured.isEnabled() && checkSecured.isChecked()){
		if (comboboxKaType.currentText() == "Pre-Shared Keys"){
			config->inherited.ka_type = KEY_MGMT_METHOD_MIKEY_PSK;
		}
		else if (comboboxKaType.currentText() == "Diffie-Hellman"){
			config->inherited.ka_type = KEY_MGMT_METHOD_MIKEY_DH;
		}
	}
	


	return error; //"Error from GTW\n";	
}

#ifndef DISABLE_PHONEBOOK

PhoneBooksWidget::PhoneBooksWidget(QWidget *parent, const char *name): 
		QWidget(parent,name),
		hLayout(),
		vLayout(this),
//		labelDesc("Description:",this),
//		editDesc(this),
		labelPath("URL (file:// or http://):",this),
		editPath(this),
		buttonAdd("Add", this),
		listPhoneBooks(this)
{
	listPhoneBooks.setAllColumnsShowFocus(true);
//	listPhoneBooks.addColumn("Description");
	listPhoneBooks.addColumn("URL");
#if (QT_VERSION > 0x030000)
        listPhoneBooks.setResizeMode(QListView::AllColumns);
#endif

//	hLayout.addWidget(&labelDesc);
//	hLayout.addWidget(&editDesc);
	hLayout.addWidget(&labelPath);
	hLayout.addWidget(&editPath);
	hLayout.addWidget(&buttonAdd);
	
	vLayout.addLayout(&hLayout);
	vLayout.addWidget(&listPhoneBooks);
	tabDialog = (SettingsDialog *)parent;
	
}

void PhoneBooksWidget::setConfig(MRef<SipSoftPhoneConfiguration *> config){
	this->config = config;
	
	list<string> phonebooks = config->phonebooks;
	for (list<string>::iterator i=phonebooks.begin(); i!=phonebooks.end(); i++)
		listPhoneBooks.insertItem(new QListViewItem(&listPhoneBooks, (*i).c_str()));
	
	connect(&buttonAdd, SIGNAL(clicked()), this, SLOT(addClicked()));
}

void PhoneBooksWidget::addClicked(){
	listPhoneBooks.insertItem(new QListViewItem(&listPhoneBooks/*, editDesc.text()*/, editPath.text()));
//	editDesc.setText("");
	editPath.setText("");
}

string PhoneBooksWidget::apply(){
	QListViewItem *cur = listPhoneBooks.firstChild();

	list<string> phonebooks = config->phonebooks;

	while (cur!=NULL){
		//cerr << cur->text(0)<< endl;
		string s = cur->text(0).ascii();
		list<string>::iterator i;
		bool found=false;
		for (i=phonebooks.begin(); i!=phonebooks.end(); i++){
			if (*i==s)
				found=true;
		}
		if (!found){
			parser->addValue("phonebook",s);
		}
		
		cur = cur->nextSibling();
	}
	
//	return "PhoneBookWidget::apply(): unimplemented\n";
//	return "(phone book settings are not applied - unimplemented\n";
	return "";
}

#endif

/////////////////////////////
//



StunTabWidget::StunTabWidget(QWidget *parent, const char *name):QWidget(parent, name), 
		layout( this, 2, 1),

		groupboxStun(2,Qt::Horizontal,"STUN", this),
		
		checkUseStun("Use STUN to detect NAT", &groupboxStun),
		labelEmpty1("", &groupboxStun),

		checkFromUri("Autodetect STUN server from SIP address", &groupboxStun),
		labelEmpty2("", &groupboxStun),
		
		checkFromDomain("Detect STUN server for domain:",&groupboxStun),
		editFromDomain(&groupboxStun),

		checkSpecifyServer("Use STUN server:",&groupboxStun),
		editServer(&groupboxStun)
{
	layout.addWidget(&groupboxStun, 0, 0);
	tabDialog = (SettingsDialog *)parent;
}

StunTabWidget::~StunTabWidget(){
}


void StunTabWidget::setConfig(MRef<SipSoftPhoneConfiguration *> config){
	this->config = config;
	
	checkUseStun.setChecked(config->useSTUN);

	checkFromUri.setChecked(config->findStunServerFromSipUri);

	checkFromDomain.setChecked(config->findStunServerFromDomain);
	editFromDomain.setText(config->stunDomain.c_str());

	checkSpecifyServer.setChecked(config->useUserDefinedStunServer);
	editServer.setText(config->userDefinedStunServer.c_str());

	connect(&checkUseStun, SIGNAL(stateChanged(int)), this, SLOT(useStunPressed(int)) );
	connect(&checkFromDomain, SIGNAL(stateChanged(int)), this, SLOT(domainPressed(int)) );
	connect(&checkSpecifyServer, SIGNAL(stateChanged(int)), this, SLOT(serverPressed(int)) );
	
	checkStates();
}

void StunTabWidget::backupSettings(){

	backupUseStun = checkUseStun.isChecked();
	backupFromUri = checkFromUri.isChecked();
	backupDoFromDomain = checkFromDomain.isChecked();
	backupDomain = editFromDomain.text().ascii();
	backupDoSpecify = checkSpecifyServer.isChecked();
	backupServer = editServer.text().ascii();
}

void StunTabWidget::restoreSettings(){
	checkUseStun.setChecked(backupUseStun);
	checkFromUri.setChecked(backupFromUri);
	checkFromDomain.setChecked(backupDoFromDomain);
	editFromDomain.setText(backupDomain.c_str());
	checkSpecifyServer.setChecked(backupDoSpecify);
	editServer.setText(backupServer.c_str());
}

string StunTabWidget::apply(){
	string ret="";
	
	if (checkUseStun.isChecked() != backupUseStun){
		config->useSTUN = checkUseStun.isChecked();
		ret = ret + "WARNING: Changing STUN settings requires restarting the application for them to have any effect.\n";
	}
	
	if (checkFromUri.isChecked() != backupFromUri){
		config->findStunServerFromSipUri = checkFromUri.isChecked();
		ret = ret + "WARNING: Enabling and disabling STUN autodetect from SIP address requires restarting minisip\n";
	}
	
	if (checkFromDomain.isChecked() != backupDoFromDomain){
		config->stunDomain = editFromDomain.text().ascii();
		config->findStunServerFromDomain = config->stunDomain!="";
		ret = ret + "WARNING: Enabling and disabling STUN server detect from domain requires restarting minisip\n";
	}
	
	if (checkSpecifyServer.isChecked() != backupDoSpecify){
		config->useUserDefinedStunServer = checkSpecifyServer.isChecked();
		if (checkSpecifyServer.isChecked()){
			config->userDefinedStunServer = editServer.text().ascii();
		}else{
			config->userDefinedStunServer = "";
		}
		ret = ret + "WARNING: Enabling and disabling STUN manual server requires restarting minisip\n";
	}
	return ret;
}

void StunTabWidget::useStunPressed(int state){
	checkStates();
}

void StunTabWidget::domainPressed(int state){
	checkStates();
}

void StunTabWidget::serverPressed(int state){
	checkStates();
}

void StunTabWidget::checkStates(){
	bool useStun = checkUseStun.isChecked();

	checkFromUri.setEnabled(useStun);
	
	checkFromDomain.setEnabled(useStun);
	editFromDomain.setEnabled(useStun && checkFromDomain.isChecked());
	
	checkSpecifyServer.setEnabled(useStun);
	editServer.setEnabled(useStun && checkSpecifyServer.isChecked());
}


////////////////////////////



AdvancedTabWidget::AdvancedTabWidget(QWidget *parent, const char *name):
		QWidget(parent, name), 
		layout( this, 2, 1),

		groupboxTransport(2,Qt::Horizontal,"SIP Transport", this),
		
		labelUdpPort("Local UDP port (0 for any)", &groupboxTransport),
		spinUdpPort(0,65535,1,&groupboxTransport),
		
		checkTcp("Enable TCP on local port:",&groupboxTransport),
		spinTcpPort(1024,65535,1,&groupboxTransport),

		checkTls("Enable TLS on local port:",&groupboxTransport),
		spinTlsPort(1024,65535,1,&groupboxTransport),
		buttonCert( "Certificate settings...", &groupboxTransport ),
		emptyLabel( &groupboxTransport, "" ),

		labelTransport("Preferred transport protocol:",&groupboxTransport),
		comboboxTransport(&groupboxTransport),
		
#ifndef OPIE
//		checkGwIp("Use this public IP (NAT):",&groupboxTransport),
//		editGwIp(&groupboxTransport),
#endif
///		labelMediaPort("Local (S)RTP port (0 for any)", this),
		
///		editMediaPort(this),

		groupboxPstn(2,Qt::Horizontal,"PSTN calls", this),
		usePstnProxy("Proxy for PSTN calls", &groupboxPstn),
		labelEmpty1("", &groupboxPstn),
	
		labelPSTNProxy("    PSTN proxy:",&groupboxPstn),
		editPSTNProxy(&groupboxPstn),
		
		labelPSTNNumber("    PSTN proxy number:",&groupboxPstn),
		editPSTNNumber(&groupboxPstn),
		
		labelPSTNUsername("    PSTN proxy username:",&groupboxPstn),
		editPSTNUsername(&groupboxPstn),

		labelPSTNPassword("    PSTN proxy password:",&groupboxPstn),
		editPSTNPassword(&groupboxPstn),
		
		checkRegisterPSTN("Register",&groupboxPstn)
{
	layout.addWidget(&groupboxTransport, 0, 0);
	tabDialog = (SettingsDialog *)parent;
}

AdvancedTabWidget::~AdvancedTabWidget(){
}


void AdvancedTabWidget::setConfig(MRef<SipSoftPhoneConfiguration *> config){
	this->config = config;
	
	spinUdpPort.setValue(config->inherited.localUdpPort);
	
	checkTcp.setChecked(config->tcp_server);
	connect(&checkTcp, SIGNAL(stateChanged(int)), this, SLOT(checkTcpPressed(int)) );
	spinTcpPort.setValue(config->inherited.localTcpPort);
	
	checkTls.setChecked(config->tls_server);
	connect(&checkTls, SIGNAL(stateChanged(int)), this, SLOT(checkTlsPressed(int)) );
	spinTlsPort.setValue(config->inherited.localTlsPort);
	
	connect(&buttonCert, SIGNAL(clicked()), tabDialog, 
		SLOT( showCertDialog() ) );

#ifndef OPIE
//	checkGwIp.setChecked(config->use_gw_ip);
//	connect(&checkGwIp, SIGNAL(stateChanged(int)), this, SLOT(checkGwIpPressed(int)) );
#endif
	
	layout.addWidget(&groupboxPstn, 1, 0);
	usePstnProxy.setChecked(config->usePSTNProxy);
	connect(&usePstnProxy, SIGNAL(stateChanged(int)), this, SLOT(usePstnPressed(int)) );
#ifndef OPIE
	editPSTNProxy.setMinimumWidth(250);
#endif
	
	if(config->pstnProxy != NULL)
		editPSTNProxy.setText(config->pstnProxy->getString().c_str());

	editPSTNNumber.setText(config->pstnNumber.c_str());
	
	editPSTNUsername.setText(config->pstnProxyUsername.c_str());

	editPSTNPassword.setEchoMode(QLineEdit::Password);
	editPSTNPassword.setText(config->pstnProxyPassword.c_str());
	
	checkRegisterPSTN.setChecked(config->doRegisterPSTN);
	
	checkStates();
#if (QT_VERSION > 0x030000)
	comboboxTransport.setCurrentText(QString(config->inherited.transport.c_str()));
#endif

}


void AdvancedTabWidget::backupSettings(){

	usePstnProxyBool = usePstnProxy.isChecked();

	pstnProxystring = editPSTNProxy.text();
	pstnProxyNumberstring = editPSTNNumber.text();
	pstnProxyUsernamestring = editPSTNUsername.text();
	pstnProxyPasswordstring = editPSTNPassword.text();

	doRegisterPSTN = checkRegisterPSTN.isChecked();

	doTcp = checkTcp.isChecked();
	doTls = checkTls.isChecked();
	udpPort = spinUdpPort.value();
	tcpPort = spinTcpPort.value();
	tlsPort = spinTlsPort.value();
	transport = comboboxTransport.currentItem();
#ifndef OPIE
//	useGwIp = checkGwIp.isChecked();
//	gwIpstring = editGwIp.text();
#endif
	
}

void AdvancedTabWidget::restoreSettings(){

	usePstnProxy.setChecked(usePstnProxyBool);

	editPSTNProxy.setText(pstnProxystring);
	editPSTNNumber.setText(pstnProxyNumberstring);
	editPSTNUsername.setText(pstnProxyUsernamestring);
	editPSTNPassword.setText(pstnProxyPasswordstring);
	
	checkRegisterPSTN.setChecked(doRegisterPSTN);

	checkTcp.setChecked(doTcp);
	checkTls.setChecked(doTls);
	spinUdpPort.setValue(udpPort);
	spinTcpPort.setValue(tcpPort);
	spinTlsPort.setValue(tlsPort);
	comboboxTransport.setCurrentItem(transport);
#ifndef OPIE
//	checkGwIp.setChecked(useGwIp);
//	editGwIp.setText(gwIpstring);
#endif
}

string AdvancedTabWidget::apply(){
	string ret;
	
	if (spinUdpPort.value() != udpPort){
		config->inherited.localUdpPort = spinUdpPort.value();
		ret = ret + "WARNING: Changing local UDP port requires restarting the application.\n";
	}
	
	if (checkTcp.isChecked()){
		if (spinTcpPort.value() != tcpPort){
			config->inherited.localTcpPort = spinTcpPort.value();
			ret = ret + "WARNING: Changing local TCP port requires restarting the application.\n";
		}
		if (!doTcp){
			config->tcp_server = true;
			ret = ret + "WARNING: Enabling TCP requires restarting the application.\n";
		}
	}
	else{
		if (doTcp){
			config->tcp_server = false;
			ret = ret + "WARNING: Disabling TCP requires restarting the application.\n";
		}
	}

	if (checkTls.isChecked()){
		if (spinTlsPort.value() != tlsPort){
			config->inherited.localTlsPort = spinTlsPort.value();
			ret = ret + "WARNING: Changing local TLS port requires restarting the application.\n";
		}
		if (!doTls){
			config->tls_server = true;
			ret = ret + "WARNING: Enabling TLS requires restarting the application.\n";
		}
	}
	else{
		if (doTls){
			config->tls_server = false;
			ret = ret + "WARNING: Disabling TLS requires restarting the application.\n";
		}
	}

	
	if (spinTlsPort.value() != tlsPort){
//		if (!dynamicSipPort.isChecked())
		config->inherited.localTlsPort = spinTlsPort.value();
		ret = ret + "WARNING: Changing local TLS port requires restarting the application.\n";
	}
	
	if( comboboxTransport.currentText().ascii() != config->inherited.transport ){
		config->inherited.transport =  comboboxTransport.currentText().ascii();
		ret += "WARNING: Changing the preferred transport requires restarting the application.\n";
	}

#ifndef OPIE
//	if (checkGwIp.isChecked() != useGwIp){
//		config->use_gw_ip = checkGwIp.isChecked();
//		if (checkGwIp.isChecked()){
//			if (editGwIp.text() != gwIpstring){
//				config->inherited.localIpString = editGwIp.text().ascii();
//			}
//		}
//	}
#endif
		
	if (usePstnProxy.isChecked() != usePstnProxyBool){
		config->usePSTNProxy = usePstnProxy.isChecked();
	}
	
	if (editPSTNProxy.text() != pstnProxystring){
		config->pstnProxyString = editPSTNProxy.text().ascii();
	}

	if (editPSTNNumber.text() != pstnProxyNumberstring){
		config->pstnNumber = editPSTNNumber.text().ascii();
		
	}

	if (editPSTNUsername.text() != pstnProxyUsernamestring){
		config->pstnProxyUsername= editPSTNUsername.text().ascii();
	}

	if (editPSTNPassword.text() != pstnProxyPasswordstring){
		config->pstnProxyPassword = editPSTNPassword.text().ascii();
	}
	
	if (checkRegisterPSTN.isChecked()!= doRegisterPSTN){
		config->doRegisterPSTN = checkRegisterPSTN.isChecked();
		if (config->doRegisterPSTN){
			CommandString reg("",SipCommandString::proxy_register);
			//if (!config->dialogContainer.isNull()){
			if (!config->sip.isNull()){
                                SipSMCommand cmd(reg, SipSMCommand::remote, SipSMCommand::TU);
				//config->dialogContainer->handleCommand(cmd);
				config->sip->handleCommand(cmd);
			}
		}
	}


	
	return ret;	
}

void AdvancedTabWidget::usePstnPressed(int state){
	int32_t toState=false;
	switch(state){
		case QButton::On:
			toState=true;
			break;
		case QButton::Off:
			toState=false;
			break;
		case QButton::NoChange:
		default:
			return;
	}
	
	labelPSTNProxy.setEnabled(toState);
	editPSTNProxy.setEnabled(toState);
	
	labelPSTNNumber.setEnabled(toState);
	editPSTNNumber.setEnabled(toState);
	
	labelPSTNUsername.setEnabled(toState);
	editPSTNUsername.setEnabled(toState);

	labelPSTNPassword.setEnabled(toState);
	editPSTNPassword.setEnabled(toState);

	checkRegisterPSTN.setEnabled(toState);
}

void AdvancedTabWidget::checkTcpPressed(int state){
	checkStates();
}

void AdvancedTabWidget::checkTlsPressed(int state){
	checkStates();
}

void AdvancedTabWidget::checkGwIpPressed(int state){
	checkStates();
}

void AdvancedTabWidget::checkStates(){
	
	bool pstnproxy = usePstnProxy.isChecked();
	bool tcp = checkTcp.isChecked();
	bool tls = checkTls.isChecked();
#ifndef OPIE
//	bool gw = checkGwIp.isChecked();
#endif

	spinTcpPort.setEnabled(tcp);
	spinTlsPort.setEnabled(tls);
	
	comboboxTransport.clear();
	comboboxTransport.insertItem("UDP");
	if( tcp )
		comboboxTransport.insertItem("TCP");
	if( tls )
		comboboxTransport.insertItem("TLS");

#ifndef OPIE
//	editGwIp.setEnabled(gw);
#endif

	comboboxTransport.setEnabled( tcp || tls );
	labelTransport.setEnabled( tcp || tls );
	
	labelPSTNProxy.setEnabled(pstnproxy);
	editPSTNProxy.setEnabled(pstnproxy);
	
	labelPSTNNumber.setEnabled(pstnproxy);
	editPSTNNumber.setEnabled(pstnproxy);
	
	labelPSTNUsername.setEnabled(pstnproxy);
	editPSTNUsername.setEnabled(pstnproxy);

	labelPSTNPassword.setEnabled(pstnproxy);
	editPSTNPassword.setEnabled(pstnproxy);

	checkRegisterPSTN.setEnabled(pstnproxy);
}


