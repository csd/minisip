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

#ifndef ACCOUNTS_LIST_H
#define ACCOUNTS_LIST_H

#include<libglademm/xml.h>
#include<gtkmm.h>
#include<libmutil/MemObject.h>


class AccountsListColumns;
class CertificateDialog;
class SipSoftPhoneConfiguration;
class SipIdentity;
class TransportList;

class AccountsList : public MObject, public Gtk::ListStore{
	public:
		static Glib::RefPtr<AccountsList> create( Glib::RefPtr<Gnome::Glade::Xml>  refXml,
							  CertificateDialog * certDialog,
							  AccountsListColumns * columns,
							  Glib::RefPtr<TransportList> transportList );
		void loadFromConfig( MRef<SipSoftPhoneConfiguration *> config );
		std::string saveToConfig( MRef<SipSoftPhoneConfiguration *> config );

		void addAccount();
		void editAccount( Gtk::TreeModel::iterator iter );
		void setDefaultAccount( Gtk::TreeModel::iterator iter );
		void setPstnAccount( Gtk::TreeModel::iterator iter );

		AccountsListColumns * getColumns();

		virtual std::string getMemObjectType() const {return "AccountsList";};
		AccountsListColumns *columns;

	private:
		AccountsList( Glib::RefPtr<Gnome::Glade::Xml>  refXml,
			      CertificateDialog * certDialog,
			      AccountsListColumns * columns,
			      Glib::RefPtr<TransportList> transportList );

		Glib::RefPtr<Gnome::Glade::Xml> refXml;
		CertificateDialog * certDialog;
		Glib::RefPtr<TransportList> transportList;
};


class AccountsListColumns : public Gtk::TreeModel::ColumnRecord{
	public:
		AccountsListColumns();
		Gtk::TreeModelColumn< MRef<SipIdentity *> > identity;
		Gtk::TreeModelColumn<Glib::ustring> name;
		Gtk::TreeModelColumn<Glib::ustring> uri;
		Gtk::TreeModelColumn<bool> autodetectSettings;
		Gtk::TreeModelColumn<Glib::ustring> proxy;
		Gtk::TreeModelColumn<uint16_t> port;

		/** Name of SIP transport plugin, like SIP, UDP or TLS */
		Gtk::TreeModelColumn<Glib::ustring> transport;
		Gtk::TreeModelColumn<Glib::ustring> username;
		Gtk::TreeModelColumn<Glib::ustring> password;
		Gtk::TreeModelColumn<uint32_t> registerExpires;
		Gtk::TreeModelColumn<bool> doRegister;
		Gtk::TreeModelColumn<Glib::ustring> status;
		Gtk::TreeModelColumn<bool> defaultProxy;
		Gtk::TreeModelColumn<bool> pstnProxy;
};
#endif
