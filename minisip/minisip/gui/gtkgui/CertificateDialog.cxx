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

#include"CertificateDialog.h"

#ifdef OLDLIBGLADEMM
#define SLOT(a,b) SigC::slot(a,b)
#define BIND SigC::bind
#define MESSAGE_DIALOG_ARG Gtk::MESSAGE_WARNING,Gtk::BUTTONS_OK,false,true
#else
#define SLOT(a,b) sigc::mem_fun(a,b)
#define BIND sigc::bind
#define MESSAGE_DIALOG_ARG false,Gtk::MESSAGE_WARNING,Gtk::BUTTONS_OK,true
#endif

using namespace std;

CertificateDialog::CertificateDialog( Glib::RefPtr<Gnome::Glade::Xml>  refXml ){
//	this->refXml = refXml;

	refXml->get_widget( "certLabel", certLabel );
	refXml->get_widget( "pkeyLabel", pkeyLabel );

	refXml->get_widget( "certButton", certButton );
	refXml->get_widget( "pkeyButton", pkeyButton );

	refXml->get_widget( "certTreeView", certTreeView );

	refXml->get_widget( "addCertButton", addCertButton );
	refXml->get_widget( "removeCertButton", removeCertButton );

	refXml->get_widget( "caTreeView", caTreeView );

	refXml->get_widget( "addFileCaButton", addFileCaButton );
	refXml->get_widget( "addDirCaButton", addDirCaButton );
	refXml->get_widget( "removeCaButton", removeCaButton );

	refXml->get_widget( "certDialog", certDialog );

	refXml->get_widget( "closeButton", closeButton );

	certButton->signal_clicked().connect( SLOT( *this,
				&CertificateDialog::chooseCert ));
	pkeyButton->signal_clicked().connect( SLOT( *this,
				&CertificateDialog::choosePKey ));

	addCertButton->signal_clicked().connect(  SLOT( *this,
				&CertificateDialog::addCert ));
	removeCertButton->signal_clicked().connect(  SLOT( *this,
				&CertificateDialog::removeCert ));

	addFileCaButton->signal_clicked().connect( SLOT( *this,
				&CertificateDialog::addFileCa ));
	addDirCaButton->signal_clicked().connect( SLOT( *this,
				&CertificateDialog::addDirCa ));
	removeCaButton->signal_clicked().connect( SLOT( *this,
				&CertificateDialog::removeCa ));

	closeButton->signal_clicked().connect( SLOT( *certDialog,
				&Gtk::Dialog::hide ));

	certTreeStore = new CertTreeStore();
	certTreeStore->associateTreeView( certTreeView );

	caListStore = new CaListStore();
	caListStore->associateTreeView( caTreeView );

	certDialog->hide();

}

CertificateDialog::~CertificateDialog(){
	delete certDialog;
}

void CertificateDialog::run(){
	certDialog->run();
}

void CertificateDialog::chooseCert(){
	string result;
	MRef<Certificate *> chosenCert;
#ifdef OLDLIBGLADEMM
	Gtk::FileSelection * dialog = new Gtk::FileSelection(
			"Choose your certificate file" );
#else
	Gtk::FileChooserDialog * dialog = new Gtk::FileChooserDialog(
			"Choose your certificate file" );
	dialog->add_button( Gtk::Stock::CANCEL, Gtk::RESPONSE_CANCEL );
	dialog->add_button( Gtk::Stock::OPEN, Gtk::RESPONSE_OK );
#endif

	int retVal = dialog->run();

	if( retVal == Gtk::RESPONSE_OK ){
		result = dialog->get_filename();

		try{
			chosenCert = Certificate::load( result );
		}
		catch( CertificateException & exc ){
			Gtk::MessageDialog messageDialog(
			"Minisip could not open that certificate file. "
			"Please check that the file is a correct "
                        "PEM-encoded certificate.", MESSAGE_DIALOG_ARG );

			messageDialog.run();
			delete dialog;
			return;
		}

		cert = chosenCert;
		certLabel->set_text( result );

		/* Set this certificate as root of the cert chain */

		certChain->lock();
		certChain->clear();
		certChain->addCertificate( chosenCert );
		certChain->unlock();

		/* Update the tree consequently */
		certTreeStore->clear();
		certTreeStore->addCertificate( chosenCert );

		/* And the reset the key field */
		pkeyLabel->set_text( "" );

		pkeyButton->set_sensitive( true );
		certTreeView->set_sensitive( true );
		addCertButton->set_sensitive( true );
		removeCertButton->set_sensitive( true );
	}

	delete dialog;

}

void CertificateDialog::choosePKey(){
	string result;
#ifdef OLDLIBGLADEMM
	Gtk::FileSelection * dialog = new Gtk::FileSelection(
			"Choose your private key file" );
#else
	Gtk::FileChooserDialog * dialog = new Gtk::FileChooserDialog(
			"Choose your private key file" );
	dialog->add_button( Gtk::Stock::CANCEL, Gtk::RESPONSE_CANCEL );
	dialog->add_button( Gtk::Stock::OPEN, Gtk::RESPONSE_OK );
#endif

	int retVal = dialog->run();

	if( retVal == Gtk::RESPONSE_OK ){
		result = dialog->get_filename();

		try{
			cert->setPk( result );
		}
		catch( CertificateExceptionPkey & exc ){
			Gtk::MessageDialog messageDialog(
			"The private key file you selected does not. "
			"match the selected certificate ",
			MESSAGE_DIALOG_ARG );

			messageDialog.run();
			delete dialog;
			return;
		}
		catch( CertificateException & exc ){
			Gtk::MessageDialog messageDialog(
			"Minisip could not open that file. "
			"Please check that the file is a correct "
                        "PEM-encoded private key.",
			MESSAGE_DIALOG_ARG );

			messageDialog.run();
			delete dialog;
			return;
		}

		pkeyLabel->set_text( result );
		certTreeView->set_sensitive( true );
		addCertButton->set_sensitive( true );
		removeCertButton->set_sensitive( true );
	}

	delete dialog;

}

void CertificateDialog::addCert(){
	string result;
	MRef<Certificate *> chosenCert;

#ifdef OLDLIBGLADEMM
	Gtk::FileSelection * dialog = new Gtk::FileSelection(
			"Choose a certificate file" );
#else
	Gtk::FileChooserDialog * dialog = new Gtk::FileChooserDialog(
			"Choose your certificate file" );
	dialog->add_button( Gtk::Stock::CANCEL, Gtk::RESPONSE_CANCEL );
	dialog->add_button( Gtk::Stock::OPEN, Gtk::RESPONSE_OK );
#endif

	int retVal = dialog->run();

	if( retVal == Gtk::RESPONSE_OK ){
		result = dialog->get_filename();

		try{
			chosenCert = Certificate::load( result );

			certChain->lock();
			certChain->addCertificate( chosenCert );
			certChain->unlock();
		}
		catch( CertificateExceptionChain & exc ){
			Gtk::MessageDialog messageDialog(
			"The selected certificate is not "
			"assigned to the issuer of the previous "
			"one.",
			MESSAGE_DIALOG_ARG );

			certChain->unlock();
			messageDialog.run();
			delete dialog;
			return;
		}
		catch( CertificateException & exc ){
			Gtk::MessageDialog messageDialog(
			"Minisip could not open that file. "
			"Please check that the file is a correct "
                        "PEM-encoded certificate.",
			MESSAGE_DIALOG_ARG );

			certChain->unlock();
			messageDialog.run();
			delete dialog;
			return;
		}

		certTreeStore->addCertificate( chosenCert );

	}

	delete dialog;
}

void CertificateDialog::removeCert(){

	/* update the internal chain */
	certChain->lock();
	certChain->removeLast();
	certChain->unlock();

	/* update the GUI */
	if( certChain->isEmpty() ){
		pkeyButton->set_sensitive( false );
		certTreeView->set_sensitive( false );
		addCertButton->set_sensitive( false );
		removeCertButton->set_sensitive( false );
		certLabel->set_text( "Choose a certificate..." );
		pkeyLabel->set_text( "Choose a private key" );
	}

	certTreeStore->removeLast();

}

void CertificateDialog::addFileCa(){
	string result;
	MRef<Certificate *> chosenCert;

#ifdef OLDLIBGLADEMM
	Gtk::FileSelection * dialog = new Gtk::FileSelection(
			"Choose a CA file" );
#else
	Gtk::FileChooserDialog * dialog = new Gtk::FileChooserDialog(
			"Choose a CA file" );
	dialog->add_button( Gtk::Stock::CANCEL, Gtk::RESPONSE_CANCEL );
	dialog->add_button( Gtk::Stock::OPEN, Gtk::RESPONSE_OK );
#endif

	int retVal = dialog->run();

	if( retVal == Gtk::RESPONSE_OK ){
		result = dialog->get_filename();

		try{
			/* Update the internal DB */
			caDb->lock();
			caDb->addFile( result );
			caDb->unlock();
		}
		catch( CertificateException & exc ){
			caDb->unlock();
			Gtk::MessageDialog messageDialog(
			"Minisip could not open that file. "
			"Please check that the file is a correct "
                        "PEM-encoded certificate.",
			MESSAGE_DIALOG_ARG );

			messageDialog.run();
			delete dialog;
			return;
		}

		/* Update the GUI */
		MRef<CertificateSetItem*> item = new CertificateSetItem();
		item->setImportMethod(CertificateSetItem::IMPORTMETHOD_FILE);
		item->setImportParameter(result);
		caListStore->addCaItem( item );
	}

	delete dialog;
}

void CertificateDialog::addDirCa(){
	string result;
	MRef<Certificate *> chosenCert;

#ifdef OLDLIBGLADEMM
	Gtk::FileSelection * dialog = new Gtk::FileSelection(
			"Choose a CA directory" );
	if( dialog->get_file_list() ){
		dialog->get_file_list()->get_parent()->hide();
	}
	if( dialog->get_selection_entry() ){
		dialog->get_selection_entry()->hide();
	}
#else
	Gtk::FileChooserDialog * dialog = new Gtk::FileChooserDialog(
			"Choose your CA directory", Gtk::FILE_CHOOSER_ACTION_SELECT_FOLDER );
	dialog->add_button( Gtk::Stock::CANCEL, Gtk::RESPONSE_CANCEL );
	dialog->add_button( Gtk::Stock::OPEN, Gtk::RESPONSE_OK );
#endif

	int retVal = dialog->run();

	if( retVal == Gtk::RESPONSE_OK ){
		//result = dialog->get_history_pulldown()->get_selection_entry();
		result = dialog->get_filename();

		/* Update CA internal db */
		caDb->lock();
		caDb->addDirectory( result );
		caDb->unlock();

		/* Update the GUI */
		MRef<CertificateSetItem*> item = new CertificateSetItem();
		item->setImportMethod(CertificateSetItem::IMPORTMETHOD_DIRECTORY);
		item->setImportParameter(result);
		caListStore->addCaItem(item);
	}

	delete dialog;
}

void CertificateDialog::removeCa(){
	MRef<CertificateSetItem*> removed;
	 Glib::RefPtr<Gtk::TreeSelection> selection;

	selection = caTreeView->get_selection();

	if( selection->count_selected_rows () == 0 ){
		return;
	}

	Gtk::TreeModel::iterator selectedItem = selection->get_selected();
	removed = caListStore->remove( selectedItem );

	caDb->lock();
	caDb->remove( removed );
	caDb->unlock();

}

void CertificateDialog::setCertChain( MRef<CertificateChain *> chain ){
	certChain = chain;
	certTreeStore->clear();
	MRef<Certificate *> item;

	if( chain.isNull() ){
		return;
	}


	item = chain->getFirst();
        if( !item.isNull() ){
                cert = item;
		certTreeView->set_sensitive( true );
                addCertButton->set_sensitive( true );
                removeCertButton->set_sensitive( true );
                pkeyButton->set_sensitive( true );
                certLabel->set_text( cert->getFile() );
                pkeyLabel->set_text( cert->getPkFile() );
        }
	else{
		certTreeView->set_sensitive( false );
		addCertButton->set_sensitive( false );
		removeCertButton->set_sensitive( false );
		certLabel->set_text( "Choose a certificate..." );
		pkeyLabel->set_text( "Choose a private key" );
		pkeyButton->set_sensitive( false );
		return;
	}


	chain->lock();

	item = chain->getNext();
        while( !item.isNull() ){

		certTreeStore->addCertificate( item );
		item = chain->getNext();
        }
        chain->unlock();

	certTreeView->expand_all();

}


void CertificateDialog::setRootCa( MRef<CertificateSet *> caDb ){


	MRef<CertificateSetItem*> item = NULL;

        this->caDb = caDb;
	caListStore->clear();
        if( caDb.isNull() ){
                return;
        }

        caDb->lock();
        caDb->initIndex();
        item = caDb->getNext();

        while( item ){
		caListStore->addCaItem( item );
                item = caDb->getNext();
        }
        caDb->unlock();
}

MRef<CertificateChain*> CertificateDialog::getCertChain() const{
	return certChain;
}

MRef<CertificateSet*> CertificateDialog::getRootCa() const{
	return caDb;
}

CertTreeStore::CertTreeStore(){

	certColumns.add( commonNameColumn );
	certColumns.add( issuerColumn );
	treeStore = Gtk::TreeStore::create( certColumns );
}

void CertTreeStore::addCertificate( MRef<Certificate *> cert ){
	if( isEmpty() ){
		lastElement = treeStore->append();
	}

	else{
		lastElement = treeStore->append( (*lastElement).children() );
	}

	(*lastElement)[ commonNameColumn ] = cert->getCn();
	(*lastElement)[ issuerColumn ] = cert->getIssuerCn();
}

void CertTreeStore::associateTreeView( Gtk::TreeView * treeView ){
//	const Glib::RefPtr<CertTreeStore> modelPtr( this );
	treeView->set_model( treeStore );
	treeView->append_column( "Common name", commonNameColumn );
	treeView->append_column( "Issuer", issuerColumn );
}

bool CertTreeStore::isEmpty(){
	return treeStore->children().empty();
}

void CertTreeStore::removeLast(){
	Gtk::TreeModel::iterator tmp = (*lastElement).parent();


	treeStore->erase( lastElement );
	lastElement = tmp;
}


void CertTreeStore::clear(){
	treeStore->clear();
}

CaListStore::CaListStore(){

	caColumns.add( typeColumn );
	caColumns.add( nameColumn );
	listStore = Gtk::ListStore::create( caColumns );
}

void CaListStore::addCaItem( MRef<CertificateSetItem*> caItem ){

	Gtk::TreeModel::iterator iter = listStore->append();

	switch( caItem->getImportMethod() ){
		case CertificateSetItem::IMPORTMETHOD_FILE:
			(*iter)[ typeColumn ] = "file";
			break;
		case CertificateSetItem::IMPORTMETHOD_DIRECTORY:
			(*iter)[ typeColumn ] = "directory";
			break;
		default:
			/* Should not happen... */
			(*iter)[ typeColumn ] = "other";
	}

	(*iter)[ nameColumn ] = caItem->getImportParameter();
}

void CaListStore::associateTreeView( Gtk::TreeView * treeView ){
//	const Glib::RefPtr<CertTreeStore> modelPtr( this );
	treeView->set_model( listStore );
	treeView->append_column( "Type", typeColumn );
	treeView->append_column( "Name", nameColumn );
}

bool CaListStore::isEmpty(){
	return listStore->children().empty();
}

MRef<CertificateSetItem*> CaListStore::remove( Gtk::TreeModel::iterator selectedItem ){
	CertificateSetItem * ret = new CertificateSetItem;


	if( (*selectedItem)[typeColumn] == "file"  ){
		ret->setImportMethod(CertificateSetItem::IMPORTMETHOD_FILE);
        } else if( (*selectedItem)[typeColumn] == "directory"  ){
		ret->setImportMethod(CertificateSetItem::IMPORTMETHOD_DIRECTORY);
        } else {
		ret->setImportMethod(CertificateSetItem::IMPORTMETHOD_OTHER);
        }

	Glib::ustring toto = ((*selectedItem)[nameColumn]);
	ret->setImportParameter(toto);

	listStore->erase( selectedItem );
	return ret;
}

void CaListStore::clear(){
	listStore->clear();
}
