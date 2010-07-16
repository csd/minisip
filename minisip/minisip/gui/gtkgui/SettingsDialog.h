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

#ifndef SETTINGS_DIALOG_GTK_H
#define SETTINGS_DIALOG_GTK_H

#include<config.h>

#include<libglademm/xml.h>
#include<gtkmm.h>

#include<string>

#include<libmutil/MemObject.h>
#include<libmutil/MessageRouter.h>
#include"AccountsList.h"

class GeneralSettings;
class MediaSettings;
class DeviceSettings;
class SecuritySettings;
class TransportList;
class TransportListColumns;
class AdvancedSettings;
class SipSettings;
class SipSoftPhoneConfiguration;
class CertificateDialog;

class SettingsDialog 
#ifdef OLDLIBGLADEMM
: public SigC::Object 
#endif
{
	public:
		SettingsDialog( Glib::RefPtr<Gnome::Glade::Xml>  refXml,
				Glib::RefPtr<TransportList> transportList );
		~SettingsDialog();

		int run();
		void accept();
		void reject();

		void show();

		void setConfig( MRef<SipSoftPhoneConfiguration *> config );
		void setAccounts( Glib::RefPtr<AccountsList> list );
		void setCallback( MRef<CommandReceiver*> callback );
	
	private:
		MRef<CommandReceiver*> callback;
		//GuiCallback * callback;
		
		Gtk::Dialog * dialogWindow;
		GeneralSettings * generalSettings;
		MediaSettings * mediaSettings;
		DeviceSettings * deviceSettings;
		SecuritySettings * securitySettings;
		AdvancedSettings * advancedSettings;
		SipSettings * sipSettings;
		MRef<SipSoftPhoneConfiguration *> config;
};

class GeneralSettings 
#ifdef OLDLIBGLADEMM
: public SigC::Object
#endif
{

	public:
		GeneralSettings( Glib::RefPtr<Gnome::Glade::Xml>  refXml );

		 std::string apply();
		
		void setConfig( MRef<SipSoftPhoneConfiguration *> config );
		void setAccounts( Glib::RefPtr<AccountsList> list );

	private:
		void addAccount();
		void editAccount();
		void removeAccount();
		void setDefaultAccount();
		void setPstnAccount();

		Gtk::TreeView * accountsTreeView;
		//const Glib::RefPtr<AccountsList> accountsList;
		Glib::RefPtr<AccountsList> accountsList;
		Gtk::Button * accountsAddButton;
		Gtk::Button * accountsRemoveButton;
		Gtk::Button * accountsEditButton;
		Gtk::Button * defaultButton;
		Gtk::Button * pstnButton;

		MRef<SipSoftPhoneConfiguration *> config;

};

class MediaSettings
#ifdef OLDLIBGLADEMM
: public SigC::Object
#endif
{

	public:
		MediaSettings( Glib::RefPtr<Gnome::Glade::Xml>  refXml );
		~MediaSettings();

		 std::string apply();
		
		void setConfig( MRef<SipSoftPhoneConfiguration *> config );

	private:

		void moveCodec( int8_t upOrDown );

		Gtk::Button * codecUpButton;
		Gtk::Button * codecDownButton;

		Gtk::TreeView * codecTreeView;
		
		Glib::RefPtr<Gtk::ListStore> codecList;
		
		Gtk::TreeModelColumnRecord * codecColumns;
		Gtk::TreeModelColumn<bool> codecEnabled;	
		Gtk::TreeModelColumn<Glib::ustring> codecName;


		MRef<SipSoftPhoneConfiguration *> config;

};

class DeviceSettings
#ifdef OLDLIBGLADEMM
: public SigC::Object
#endif
{

	public:
		DeviceSettings( Glib::RefPtr<Gnome::Glade::Xml>  refXml );
		~DeviceSettings();

		 std::string apply();
		
		void setConfig( MRef<SipSoftPhoneConfiguration *> config );

	private:

		void soundInputChange();
		void soundOutputChange();

		Gtk::Entry * videoEntry;
	
		Gtk::Entry * videoEntry2;
		Gtk::Entry * displayFrameSize;		
		 Gtk::Entry * displayFrameRate;


		Gtk::Entry * soundInputEntry;
		Gtk::Entry * soundOutputEntry;

		Gtk::ComboBox * soundInputView;
		Gtk::ComboBox * soundOutputView;

		Gtk::TreeModelColumnRecord * deviceColumns;
		Gtk::TreeModelColumn<std::string> deviceName;
		Gtk::TreeModelColumn<Glib::ustring> deviceDescription;

		Glib::RefPtr<Gtk::ListStore> soundInputList;
		Glib::RefPtr<Gtk::ListStore> soundOutputList;

		Gtk::Label * videoLabel;
		Gtk::Label * videoDeviceLabel;

		Gtk::CheckButton * spaudioCheck;


		MRef<SipSoftPhoneConfiguration *> config;

};

class SecuritySettings
#ifdef OLDLIBGLADEMM
: public SigC::Object
#endif
{

	public:
		SecuritySettings( Glib::RefPtr<Gnome::Glade::Xml>  refXml,
				  CertificateDialog * certDialog );
		~SecuritySettings();

		 std::string apply();
		
		void setConfig( MRef<SipIdentity *> identity );

		void reset();

	private:

		void kaChange();
		void secureChange();

		CertificateDialog * certDialog;

		Gtk::CheckButton * dhCheck;
		Gtk::CheckButton * certCheck;
		Gtk::CheckButton * pskCheck;
		
		Gtk::Entry * pskEntry;
		
		Gtk::CheckButton * secureCheck;

		Gtk::Table * secureTable;

		Gtk::Label * kaTypeLabel;
		Gtk::Label * pskLabel;
		Gtk::RadioButton * pskRadio;
		Gtk::RadioButton * dhRadio;
		Gtk::RadioButton * dhhmacRadio;
		Gtk::RadioButton * rsarRadio;

		Gtk::Button * certificateButton;

		MRef<SipIdentity *> identity;

		sigc::connection dhConn;
		sigc::connection pskConn;
		sigc::connection secureConn;
		sigc::connection certificateConn;
};

class AdvancedSettings
#ifdef OLDLIBGLADEMM
: public SigC::Object
#endif
{

	public:
		AdvancedSettings( Glib::RefPtr<Gnome::Glade::Xml>  refXml,
				  Glib::RefPtr<TransportList> transportList );

		 std::string apply();
		
		void setConfig( MRef<SipSoftPhoneConfiguration *> config );

	private:

		void transportChange();
		void stunAutodetectChange();
		
		/** 
		Contains a drop-down list of the available network interfaces
		*/
		Gtk::Combo * networkInterfacesCombo;
		Gtk::Entry * networkInterfacesEntry;
		Gtk::RadioButton * ipv4Radio;
		Gtk::RadioButton * ipv46Radio;

		Gtk::TreeView * transportView;
		
		Gtk::SpinButton * sipSpin;
		Gtk::SpinButton * sipsSpin;

		Gtk::CheckButton * stunCheck;
		Gtk::CheckButton * stunAutodetectCheck;
		Gtk::Entry * stunEntry;

		MRef<SipSoftPhoneConfiguration *> config;
		Glib::RefPtr<TransportList> transportList;		
};

class SipSettings
#ifdef OLDLIBGLADEMM
: public SigC::Object
#endif
{

	public:
		SipSettings( Glib::RefPtr<Gnome::Glade::Xml>  refXml );

		std::string apply();
		
		void setConfig( MRef<SipSoftPhoneConfiguration *> config );

	private:
		Gtk::CheckButton * use100RelCheck;
		Gtk::CheckButton * anatCheck;

		MRef<SipSoftPhoneConfiguration *> config;

};

#endif
