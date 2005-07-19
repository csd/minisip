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

#ifndef SETTINGS_DIALOG_GTK_H
#define SETTINGS_DIALOG_GTK_H

#include<config.h>

#include<libglademm/xml.h>
#include<gtkmm.h>

#include<libmutil/MemObject.h>
#include"AccountsList.h"

class GeneralSettings;
class SecuritySettings;
class AdvancedSettings;
class SipSoftPhoneConfiguration;
class CertificateDialog;

class SettingsDialog 
#ifdef OLDLIBGLADEMM
: public SigC::Object 
#endif
{
	public:
		SettingsDialog( Glib::RefPtr<Gnome::Glade::Xml>  refXml, 
				CertificateDialog * certDialog );
		~SettingsDialog();

		int run();
		void accept();
		void reject();

		void show();

		void setConfig( MRef<SipSoftPhoneConfiguration *> config );
		void setAccounts( Glib::RefPtr<AccountsList> list );
	
	private:
	
		CertificateDialog * certificateDialog;
		Gtk::Button * certificateButton;
		Gtk::Dialog * dialogWindow;
		GeneralSettings * generalSettings;
		SecuritySettings * securitySettings;
		AdvancedSettings * advancedSettings;
		MRef<SipSoftPhoneConfiguration *> config;
};

class GeneralSettings 
#ifdef OLDLIBGLADEMM
: public SigC::Object
#endif
{

	public:
		GeneralSettings( Glib::RefPtr<Gnome::Glade::Xml>  refXml );

		string apply();
		
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

		Gtk::Entry * soundEntry;

		MRef<SipSoftPhoneConfiguration *> config;

};

class SecuritySettings
#ifdef OLDLIBGLADEMM
: public SigC::Object
#endif
{

	public:
		SecuritySettings( Glib::RefPtr<Gnome::Glade::Xml>  refXml );

		string apply();
		
		void setConfig( MRef<SipSoftPhoneConfiguration *> config );

	private:

		void kaChange();
		void secureChange();

		Gtk::CheckButton * dhCheck;
		Gtk::CheckButton * pskCheck;
		
		Gtk::HBox * pskBox;
		
		
		Gtk::Entry * pskEntry;
		
		Gtk::CheckButton * secureCheck;

		Gtk::Table * secureTable;

		Gtk::Entry * kaEntry;
		Gtk::Combo * kaCombo;


		MRef<SipSoftPhoneConfiguration *> config;

};

class AdvancedSettings
#ifdef OLDLIBGLADEMM
: public SigC::Object
#endif
{

	public:
		AdvancedSettings( Glib::RefPtr<Gnome::Glade::Xml>  refXml );

		string apply();
		
		void setConfig( MRef<SipSoftPhoneConfiguration *> config );

	private:

		void transportChange();
		void stunAutodetectChange();
		
		Gtk::SpinButton * udpSpin;
		Gtk::SpinButton * tcpSpin;
		Gtk::SpinButton * tlsSpin;

		Gtk::CheckButton * tcpCheck;
		Gtk::CheckButton * tlsCheck;

		Gtk::Entry * transportEntry;
		Gtk::Combo * transportCombo;
		
		Gtk::CheckButton * stunCheck;
		Gtk::CheckButton * stunAutodetectCheck;
		Gtk::Entry * stunEntry;

		MRef<SipSoftPhoneConfiguration *> config;

};


#endif
