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

#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H

#define DISABLE_PHONEBOOK

#include<config.h>

#include<qapplication.h>
#include<qwidget.h>
#include<qtabdialog.h>
#include<qlabel.h>
#include<qlineedit.h>
#include<qradiobutton.h>
#include<qcheckbox.h>
#include<qcombobox.h>
#include<qlayout.h>
#include<qbuttongroup.h>
#include<qvbuttongroup.h>
#include<qpushbutton.h>
#include<qlistview.h>
#include<qspinbox.h>
#include<qfiledialog.h>
#include<qscrollview.h>
#include"CertificateDialog.h"
//#include"../../../sip/state_machines/SipSoftPhone.h"
#include"../../../sip/SipSoftPhoneConfiguration.h"
#include<libmutil/XMLParser.h>

class SettingsDialog;

class GeneralTabWidget : public QWidget{
	Q_OBJECT;
	public:
		GeneralTabWidget(QWidget *parent=0, const char *name=0);
		~GeneralTabWidget();
		void setConfig(MRef<SipSoftPhoneConfiguration *> config);
		void backupSettings();
		void restoreSettings();

	public slots:
		void autodetectStateChanged(int state);
		string apply();
	private:
		MRef<SipSoftPhoneConfiguration *> config;
		void checkStates();
		QGridLayout layout;

		QGroupBox groupboxSip;

		QButtonGroup autodetectRadioGroup;

		QLabel labelUserURI;
		QLineEdit editUserURI;
		
		QLabel labelProxyUsername;
		QLineEdit editProxyUsername;
		
		QLabel labelProxyPassword;
		QLineEdit editProxyPassword;
		
		QRadioButton radioAutodetect;
		QLabel labelEmpty;
		
		QRadioButton radioManualProxy;
		QLineEdit editManualProxy;

		QCheckBox checkRegister;
		
		QGroupBox groupboxDevices;
		
		QLabel labelSoundDevice;
		QComboBox comboboxSoundDevice;
		
		QLabel labelNIC;
		QComboBox comboboxNIC;

		// backup 
		QString userUristring;
		QString proxyUsernamestring;
		QString proxyPasswordstring;
		bool autodetectProxy;
		QString manualProxystring;
		bool doRegister;
		int32_t soundDeviceIndex;
		int32_t nicDeviceIndex;

		SettingsDialog * tabDialog;
};

class SecurityTabWidget : public QWidget{
	Q_OBJECT;
	public:
		SecurityTabWidget(QWidget *parent=0, const char *name=0);
		~SecurityTabWidget();
		void setConfig(MRef<SipSoftPhoneConfiguration *> config);
		void backupSettings();
		void restoreSettings();
	public slots:
		//void autodetectStateChanged(int32_t state);
		string apply();
		void checkSecuredPressed(int state);
		void checkDHPressed(int state);
		void checkPSKPressed(int state);
		
	private:
		MRef<SipSoftPhoneConfiguration *>config;
		void checkStates();
		QGridLayout layout;
		QGroupBox groupboxDH;

		QGroupBox groupboxOutGoing;
		QCheckBox checkSecured;
		QLabel labelOGEmpty;
#ifndef OPIE
		QLabel labelOGEmpty2;
		QLabel labelKaType;
#endif
		QComboBox comboboxKaType;
		
		//QCheckBox checkPK;
		
		QCheckBox checkDH;
		//QGridLayout layoutDH;
		QLabel labelDHEmpty;
#ifndef OPIE
		QLabel labelDHEmpty2;
#endif

		QPushButton buttonCert;
		
		QGroupBox groupboxPSK;
		QCheckBox checkPSK;
		QLabel labelPSKEmpty;
#ifndef OPIE
		QLabel labelPSKEmpty2;
#endif
		QLabel labelPSK;
		QLineEdit editPSK;

		// backup
		bool doSecured;
		bool doDH;
		bool doPSK;
		bool doCA;
		QString castring;
		QString certificatestring;
		QString privateKeystring;
		QString pskstring;
		int kaTypeSelected;
		SettingsDialog * tabDialog;
};
		
#ifndef DISABLE_PHONEBOOK

class PhoneBooksWidget : public QWidget{
	EEQ_OBJECT;
	public:
		PhoneBooksWidget(QWidget *parent=0, const char *name=0);
		void setConfig(MRef<SipSoftPhoneConfiguration *> sipphone);
		void setSipSoftPhoneConfig(MRef<SipSoftPhoneConfiguration *>config);
		void backupSettings();
		void restoreSettings();
		
	EEpublic EEslots:                       //conditional compile conflicts with moc - removing this moc directive
		string apply(XMLParser *parser);	
		void addClicked();

	private:
		//SipSoftPhone *sipphone;
		MRef<SipSoftPhoneConfiguration *>config;
		QHBoxLayout hLayout;
		QVBoxLayout vLayout;

//		QLabel labelDesc;
//		QLineEdit editDesc;
		
		QLabel labelPath;
		QLineEdit editPath;

		QPushButton buttonAdd;

		QListView listPhoneBooks;
		SettingsDialog * tabDialog;
	
};

#endif


class StunTabWidget : public QWidget{
	Q_OBJECT;
	public:
		StunTabWidget(QWidget *parent=0, const char *name=0);
		~StunTabWidget();
		void setConfig(MRef<SipSoftPhoneConfiguration *> config);

		void backupSettings();
		void restoreSettings();
		string apply();

	public slots:
		void useStunPressed(int state);
//		void autodetectPressed(int state);
		void domainPressed(int state);
		void serverPressed(int state);
	private:
		void checkStates();
		MRef<SipSoftPhoneConfiguration *> config;
		QGridLayout layout;

		QGroupBox groupboxStun;

		QCheckBox checkUseStun;
		QLabel labelEmpty1;

		QCheckBox checkFromUri;
		QLabel labelEmpty2;
		
		QCheckBox checkFromDomain;
		QLineEdit editFromDomain;
		
		QCheckBox checkSpecifyServer;
		QLineEdit editServer;

		
		// Backup in case of cancel & check if value changed
		bool backupUseStun;
		bool backupFromUri;

		bool backupDoFromDomain;
		string backupDomain;
		bool backupDoSpecify;
		string backupServer; 
		SettingsDialog * tabDialog;
};





class AdvancedTabWidget : public QWidget{
	Q_OBJECT;
	public:
		AdvancedTabWidget(QWidget *parent=0, const char *name=0);
		~AdvancedTabWidget();
		void setConfig(MRef<SipSoftPhoneConfiguration *> config);

		void backupSettings();
		void restoreSettings();

	public slots:
		void usePstnPressed(int state);
		void checkTcpPressed(int state);
		void checkTlsPressed(int state);
		void checkGwIpPressed(int state);
		string apply();
	private:
		void checkStates();
		//SipSoftPhone *sipphone;
		MRef<SipSoftPhoneConfiguration *>config;
		QGridLayout layout;

		QGroupBox groupboxTransport;
		
		QLabel labelUdpPort;
		QSpinBox spinUdpPort;

		QCheckBox checkTcp;
		QSpinBox spinTcpPort;
		
		QCheckBox checkTls;
		QSpinBox spinTlsPort;

		QLabel emptyLabel;
		QPushButton buttonCert;

		QLabel labelTransport;
		QComboBox comboboxTransport;

#ifndef OPIE
//		QCheckBox checkGwIp;
//		QLineEdit editGwIp;
#endif
		

///		QLabel labelMediaPort;
		
///		QLineEdit editMediaPort;
		
		QGroupBox groupboxPstn;

		
		QCheckBox usePstnProxy;
		QLabel labelEmpty1;

		QLabel labelPSTNProxy;
		QLineEdit editPSTNProxy;
		
		QLabel labelPSTNNumber;
		QLineEdit editPSTNNumber;
		
		QLabel labelPSTNUsername;
		QLineEdit editPSTNUsername;

		QLabel labelPSTNPassword;
		QLineEdit editPSTNPassword;

	
		QCheckBox checkRegisterPSTN;

		
		// Backup in case of cancel & check if value changed
		bool doTcp;
		bool doTls;
		int32_t udpPort;
		int32_t tcpPort; 
		int32_t tlsPort; 
		int32_t transport;
		bool useDynamicSipPort;
		QString manualSipPortstring;
//		bool useDynamicMediaPort;
//		QString manualMediaPortstring;
		bool usePstnProxyBool;
		QString pstnProxystring;
		QString pstnProxyNumberstring;
		QString pstnProxyUsernamestring;
		QString pstnProxyPasswordstring;
		bool doRegisterPSTN;
		bool useGwIp;
		QString gwIpstring;
		SettingsDialog * tabDialog;
};



class SettingsDialog : public QTabDialog{
	Q_OBJECT;
	public:
		SettingsDialog(QWidget *parent=0, const char *name=0, bool destroy=false);
		~SettingsDialog();
		void setConfig(MRef<SipSoftPhoneConfiguration *> config);

		virtual void show();

	private slots:
		//void apply();
		//void cancel();
		virtual void accept();
		virtual void reject();
		void showCertDialog();
	private:
		CertificateDialog certDialog;

		
		GeneralTabWidget generalWidget;
		SecurityTabWidget securityWidget;
#ifndef DISABLE_PHONEBOOK
		PhoneBooksWidget phoneBooksWidget;
#endif
		StunTabWidget stunWidget;
		AdvancedTabWidget advancedWidget;
		MRef<SipSoftPhoneConfiguration *> config;
};


#endif
