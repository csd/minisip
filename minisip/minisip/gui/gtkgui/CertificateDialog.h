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

#ifndef CERTIFICATEDIALOG_H
#define CERTIFICATEDIALOG_H

#include<config.h>

#include<libglademm/xml.h>
#include<gtkmm.h>

#include<libmcrypto/cert.h>

class CertTreeStore;
class CaListStore;

class CertificateDialog
#ifdef OLDLIBGLADEMM
: public SigC::Object
#endif
{
	public:
		CertificateDialog( Glib::RefPtr<Gnome::Glade::Xml>  refXml );
		~CertificateDialog();

		MRef<CertificateChain*> getCertChain() const;
		MRef<CertificateSet*> getRootCa() const;

		void setCertChain( MRef<CertificateChain *> chain );
		void setRootCa( MRef<CertificateSet *> caDb );

		void run();

	private:

	//	Glib::RefPtr<Gnome::Glade::Xml>  refXml;

		MRef<CertificateChain *> certChain;
		MRef<CertificateSet *> caDb;
		MRef<Certificate *> cert;

		void addDirCa();
                void addFileCa();
                void removeCa();
                void addCert();
                void removeCert();
                void chooseCert();
                void choosePKey();

		//Gtk::TreeModel::ColumnRecord certColumns;
		//Gtk::TreeModelColumn<Glib::ustring> commonNameColumn;
		//Gtk::TreeModelColumn<Glib::ustring> issuerColumn;
		//Glib::RefPtr<Gtk::TreeStore> certTreeStore;
		MRef<CertTreeStore *> certTreeStore;
		MRef<CaListStore *> caListStore;
		
		Gtk::Label * certLabel;
		Gtk::Label * pkeyLabel;

		Gtk::Button * certButton;
		Gtk::Button * pkeyButton;

		Gtk::TreeView * certTreeView;

		Gtk::Button * addCertButton;
		Gtk::Button * removeCertButton;

		Gtk::TreeView * caTreeView;

		Gtk::Button * addDirCaButton;
		Gtk::Button * addFileCaButton;
		Gtk::Button * removeCaButton;

		Gtk::Dialog * certDialog;
		
		Gtk::Button * closeButton;
};

class CertTreeStore : public MObject{
	public:
		CertTreeStore();

		virtual std::string getMemObjectType() const {return "CertTreeStore";}
		void addCertificate( MRef<Certificate *> );
		MRef<CertificateChain *> getCertChain();
		void associateTreeView( Gtk::TreeView * certTreeView );
		bool isEmpty();
		void clear();
		void removeLast();

	private:
		Gtk::TreeModelColumn<Glib::ustring> commonNameColumn;
		Gtk::TreeModelColumn<Glib::ustring> issuerColumn;
		Gtk::TreeModel::ColumnRecord certColumns;
		Glib::RefPtr<Gtk::TreeStore> treeStore;
		Gtk::TreeModel::iterator lastElement;
	
};

class CaListStore : public MObject{
	public:
		CaListStore();

		void addCaItem( MRef<CertificateSetItem*> caItem );
		virtual std::string getMemObjectType() const {return "CaListStore";}
		//MRef<CertificateChain *> getCertChain();
		
		void associateTreeView( Gtk::TreeView * caTreeView );
		bool isEmpty();
		MRef<CertificateSetItem*> remove( Gtk::TreeModel::iterator );
		void clear();

	private:
		Gtk::TreeModelColumn<Glib::ustring> typeColumn;
		Gtk::TreeModelColumn<Glib::ustring> nameColumn;
		
		Gtk::TreeModel::ColumnRecord caColumns;
		Glib::RefPtr<Gtk::ListStore> listStore;
	
};




#endif
