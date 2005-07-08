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

/* Copyright (C) 2004, 2005
 *
 * Authors: Erik Eliasson <eliasson@it.kth.se>
 *          Johan Bilien <jobi@via.ecp.fr>
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
	
	refXml->get_widget( "certChainFrame", certChainFrame );
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

void CertificateDialog::run(){
	certDialog->run();
}

void CertificateDialog::chooseCert(){
	string result;
	MRef<certificate *> chosenCert;
	Gtk::FileSelection * dialog = new Gtk::FileSelection( 
			"Choose your certificate file" );

	int retVal = dialog->run();

	if( retVal == Gtk::RESPONSE_OK ){
		result = dialog->get_filename();

		try{
			chosenCert = new certificate( result );
		}
		catch( certificate_exception * exc ){
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
		certChain->add_certificate( chosenCert );
		certChain->unlock();
		
		/* Update the tree consequently */
		certTreeStore->clear();
		certTreeStore->addCertificate( chosenCert );

		/* And the reset the key field */
		pkeyLabel->set_text( "" );

		pkeyButton->set_sensitive( true );
		certChainFrame->show();
	}

	delete dialog;
	
}

void CertificateDialog::choosePKey(){
	string result;
	Gtk::FileSelection * dialog = new Gtk::FileSelection( 
			"Choose your private key file" );

	int retVal = dialog->run();

	if( retVal == Gtk::RESPONSE_OK ){
		result = dialog->get_filename();

		try{
			cert->set_pk( result );
		}
		catch( certificate_exception_pkey * exc ){
			Gtk::MessageDialog messageDialog( 
			"The private key file you selected does not. "
			"match the selected certificate ",
			MESSAGE_DIALOG_ARG );

			messageDialog.run();
			delete dialog;
			return;
		}
		catch( certificate_exception * exc ){
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
		certChainFrame->set_sensitive( true );
	}

	delete dialog;

}

void CertificateDialog::addCert(){
	string result;
	MRef<certificate *> chosenCert;

	Gtk::FileSelection * dialog = new Gtk::FileSelection( 
			"Choose a certificate file" );

	int retVal = dialog->run();

	if( retVal == Gtk::RESPONSE_OK ){
		result = dialog->get_filename();

		try{
			chosenCert = new certificate( result );

			certChain->lock();
			certChain->add_certificate( chosenCert );
			certChain->unlock();
		}
		catch( certificate_exception_chain * exc ){
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
		catch( certificate_exception * exc ){
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
	certChain->remove_last();
	certChain->unlock();

	/* update the GUI */
	if( certChain->is_empty() ){
		certLabel->set_text( "" );
		pkeyLabel->set_text( "" );
		pkeyButton->set_sensitive( false );
		//certChainFrame->set_sensitive( false );
		certChainFrame->hide();
	}
		
	certTreeStore->removeLast();

}

void CertificateDialog::addFileCa(){
	string result;
	MRef<certificate *> chosenCert;

	Gtk::FileSelection * dialog = new Gtk::FileSelection( 
			"Choose a CA file" );

	int retVal = dialog->run();

	if( retVal == Gtk::RESPONSE_OK ){
		result = dialog->get_filename();

		try{
			/* Update the internal DB */
			caDb->lock();
			caDb->add_file( result );
			caDb->unlock();
		}
		catch( certificate_exception * exc ){
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
		ca_db_item item;
		item.type = CERT_DB_ITEM_TYPE_FILE;
		item.item = result;
		caListStore->addCaItem( &item );
	}
	
	delete dialog;
}

void CertificateDialog::addDirCa(){
	string result;
	MRef<certificate *> chosenCert;

	Gtk::FileSelection * dialog = new Gtk::FileSelection( 
			"Choose a CA directory" );

	if( dialog->get_file_list() ){
		dialog->get_file_list()->get_parent()->hide();
	}
	if( dialog->get_selection_entry() ){
		dialog->get_selection_entry()->hide();
	}
	int retVal = dialog->run();

	if( retVal == Gtk::RESPONSE_OK ){
		//result = dialog->get_history_pulldown()->get_selection_entry();
		result = dialog->get_filename();

		/* Update CA internal db */
		caDb->lock();
		caDb->add_directory( result );
		caDb->unlock();

		/* Update the GUI */
		ca_db_item item;
		item.type = CERT_DB_ITEM_TYPE_DIR;
		item.item = result;
		caListStore->addCaItem( &item );

	}

	delete dialog;
}

void CertificateDialog::removeCa(){
	ca_db_item * removed;
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

void CertificateDialog::setCertChain( MRef<certificate_chain *> chain ){
	certChain = chain;
	MRef<certificate *> item;

	if( chain.isNull() ){
		return;
	}


	item = chain->get_first();
        if( !item.isNull() ){
                cert = item;
                addCertButton->set_sensitive( true );
                removeCertButton->set_sensitive( true );
                pkeyButton->set_sensitive( true );
                certLabel->set_text( cert->get_file() );
                pkeyLabel->set_text( cert->get_pk_file() );
        }
	else{
		certChainFrame->hide();
		certLabel->set_text( "Choose a certificate..." );
		pkeyLabel->set_text( "Choose a private key" );
		pkeyButton->set_sensitive( false );
		return;
	}


	chain->lock();

	item = chain->get_next();
        while( !item.isNull() ){

		certTreeStore->addCertificate( item );
		item = chain->get_next();
        }
        chain->unlock();

	certTreeView->expand_all();
	
}

                
void CertificateDialog::setRootCa( MRef<ca_db *> caDb ){


    	ca_db_item * item = NULL;

        this->caDb = caDb;
        if( caDb.isNull() ){
                return;
        }

        caDb->lock();
        caDb->init_index();
        item = caDb->get_next();

        while( item != NULL ){
		caListStore->addCaItem( item );
                item = caDb->get_next();
        }
        caDb->unlock();
}

CertTreeStore::CertTreeStore(){

	certColumns.add( commonNameColumn );
	certColumns.add( issuerColumn );
	treeStore = Gtk::TreeStore::create( certColumns );
}

void CertTreeStore::addCertificate( MRef<certificate *> cert ){
	if( isEmpty() ){
		lastElement = treeStore->append();
	}

	else{
		lastElement = treeStore->append( (*lastElement).children() );
	}

	(*lastElement)[ commonNameColumn ] = cert->get_cn();
	(*lastElement)[ issuerColumn ] = cert->get_issuer_cn();
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

void CaListStore::addCaItem( ca_db_item  * caItem ){
		
	Gtk::TreeModel::iterator iter = listStore->append();

	switch( caItem->type ){
		case CERT_DB_ITEM_TYPE_FILE:
			(*iter)[ typeColumn ] = "file";
			break;
		case CERT_DB_ITEM_TYPE_DIR:
			(*iter)[ typeColumn ] = "directory";
			break;
		default:
			/* Should not happen... */
			(*iter)[ typeColumn ] = "other";
	}

	(*iter)[ nameColumn ] = caItem->item;
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

ca_db_item * CaListStore::remove( Gtk::TreeModel::iterator selectedItem ){
	ca_db_item * ret = new ca_db_item;


	if( (*selectedItem)[typeColumn] == "file"  ){
                ret->type = CERT_DB_ITEM_TYPE_FILE;
        }
	else if( (*selectedItem)[typeColumn] == "directory"  ){
                ret->type = CERT_DB_ITEM_TYPE_DIR;
        }
        else{
                ret->type = CERT_DB_ITEM_TYPE_OTHER;
        }

	Glib::ustring toto = ((*selectedItem)[nameColumn]);
	ret->item = toto;

	listStore->erase( selectedItem );
	return ret;
}
