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

#include"CertificateDialog.h"

#include<iostream>
using namespace std;


CertificateDialog::CertificateDialog( QWidget * parent/*, certificate_chain * cc */):
		QTabDialog( parent ),
		persTab( this ),
		rootCaBox( 0, Qt::Vertical, "Root certificates database", this ),
		certChainBox( 0, Qt::Vertical,
				"Personal certificates chain", &persTab ),
		pkBox( 2, Qt::Horizontal,
				"Personal information", &persTab ),
		layout( &persTab, 3 ),

		rootCaLayout( rootCaBox.layout() ),
		certChainLayout( certChainBox.layout() ),
		rootCaList( &rootCaBox ),
		certChainList( &certChainBox ),
		addCaDirButton(  "Add a directory", &rootCaBox ),
		addCaFileButton( "Add a file", &rootCaBox ),
		removeCaButton( "Remove", &rootCaBox ),

		addCertChainButton( "Add", &certChainBox ),
		removeCertChainButton( "Remove", &certChainBox ),
		certLabel( &pkBox ),
		certBrowse( "Certificate", &pkBox ),
		pkLabel( &pkBox ),
		pkBrowse( "Private key", &pkBox ),
		lastChain( NULL ),
		certDb( NULL ),
		certChain( NULL ),
		cert( NULL )
		{

	setCaption( "Certificates management" );

	insertTab( &persTab, "Personal settings" );
	insertTab( &rootCaBox, "CA database" );
	layout.addWidget( &pkBox );
	layout.addWidget( &certChainBox );
	//layout.addWidget( &rootCaBox );

	rootCaLayout.addMultiCellWidget( &rootCaList, 0, 0, 0, 2 );
	rootCaLayout.addWidget( &addCaDirButton, 1, 0 );
	rootCaLayout.addWidget( &addCaFileButton, 1, 1 );
	rootCaLayout.addWidget( &removeCaButton, 1, 2 );

	certChainLayout.addMultiCellWidget( &certChainList, 0, 0, 0, 1 );
	certChainLayout.addWidget( &addCertChainButton, 1, 0 );
	certChainLayout.addWidget( &removeCertChainButton, 1, 1 );

	rootCaList.addColumn( "Type" );
	rootCaList.addColumn( "Item" );

	certChainList.addColumn( "Common name" );
	certChainList.addColumn( "Issuer" );

	connect( &addCaDirButton,  SIGNAL(clicked()), this, SLOT(addCaDir()));
	connect( &addCaFileButton, SIGNAL(clicked()), this, SLOT(addCaFile()));
	connect( &addCertChainButton, SIGNAL(clicked()), this,
			SLOT( addCertChain() ));
	connect( &removeCaButton, SIGNAL(clicked()), this,
			SLOT( removeCa() ));

	connect( &removeCertChainButton, SIGNAL(clicked()), this,
			SLOT( removeCertChain() ));

	connect( &certBrowse, SIGNAL(clicked()), this,
			SLOT( chooseCert() ));

	connect( &pkBrowse, SIGNAL(clicked()), this,
			SLOT( choosePk() ));

	certChainList.setAllColumnsShowFocus( true );
	rootCaList.setAllColumnsShowFocus( true );
	certChainList.setRootIsDecorated( true );

	pkBrowse.setEnabled( false );
	addCertChainButton.setEnabled( false );
	removeCertChainButton.setEnabled( false );
}

void CertificateDialog::setCertChain( MRef<certificate_chain *> chain ){
	certChain = chain;

	if( chain.isNull() ){
		return;
	}
	MRef<certificate *> item;
	MRef<CertChainItem *> listItem;

	item = chain->get_first();
	if( !(item.isNull()) ){
		cert = item;
		addCertChainButton.setEnabled( true );
		removeCertChainButton.setEnabled( true );
		pkBrowse.setEnabled( true );
		certLabel.setText( cert->get_file().c_str() );
		pkLabel.setText( cert->get_pk_file().c_str() );
	}
	chain->lock();

	item = chain->get_next();
	while( ! (item.isNull()) ){
		if( certChainList.childCount() == 0 ){
			listItem = MRef<CertChainItem*>(new CertChainItem( &certChainList, item ));
			lastChain = listItem;
		}
		else{
			listItem = new CertChainItem(
				(QListViewItem *)*lastChain,
				item );

			lastChain = listItem;
		}

		item = chain->get_next();
		//certChainList.insertItem( listItem );
	}
	chain->unlock();

}

void CertificateDialog::setRootCa( MRef<ca_db *> caDb ){
	ca_db_item * item = NULL;
	QListViewItem * listItem;

	certDb = caDb;
	if( caDb.isNull() ){
		return;
	}

	caDb->lock();
	caDb->init_index();
	item = caDb->get_next();

	while( item != NULL ){
		if( item->getImportMethod() == CertificateSetItem::IMPORTMETHOD_OTHER ){
			listItem = new QListViewItem( &rootCaList,
				"Other",
				"unimplemented" );
		}
		else if( item->getImportMethod() == CertificateSetItem::IMPORTMETHOD_FILE ){
			listItem = new QListViewItem( &rootCaList,
				"File",
    				item->getImportParameter().c_str() );
		}
		else{
			listItem = new QListViewItem( &rootCaList,
				"Directory",
				item->getImportParameter().c_str() );
		}
		item = caDb->get_next();
	}
	caDb->unlock();
}

void CertificateDialog::addCaDir(){
	QListViewItem * listItem = NULL;
	QString result;
#ifndef OPIE
	QFileDialog * fileDialog = new QFileDialog( this, "certDir", TRUE );
	fileDialog->setMode( QFileDialog::Directory );
        fileDialog->setCaption(
			QFileDialog::tr( "Choose a certificate directory" ) );
#else
	OFileDialog * fileDialog = new OFileDialog("Choose a certificate directory", this, TRUE, OFileSelector::EXTENDED_ALL, "." );
#endif

        if( fileDialog->exec() == QDialog::Accepted ){
#ifdef OPIE
		result = fileDialog->fileName();
#else
		result = fileDialog->selectedFile();
#endif
        	//editCertificate.setText( fileCertificate->selectedFile() );
		certDb->add_directory( result.ascii() );
		listItem = new QListViewItem( &rootCaList,
			"Directory",
			result );

	}

        delete fileDialog;
}

void CertificateDialog::addCaFile(){
	QListViewItem * listItem = NULL;
	QString result;
#ifndef OPIE
	QFileDialog * fileDialog = new QFileDialog( this, "certDir", TRUE );
        fileDialog->setCaption(
			QFileDialog::tr( "Choose a CA certificate file" ) );
#else
	OFileDialog * fileDialog = new OFileDialog("Choose a CA certificate file", this, TRUE, OFileSelector::EXTENDED_ALL, "." );
#endif
        if( fileDialog->exec() == QDialog::Accepted ){
#ifdef OPIE
		result = fileDialog->fileName();
#else
		result = fileDialog->selectedFile();
#endif
        	//editCertificate.setText( fileCertificate->selectedFile() );
		try{
			certDb->add_file( result.ascii() );
		} catch( certificate_exception & exc ){
			QMessageBox::critical( this, "Minisip",
  		  	"Minisip could not open that certificate file.\n"
    			"Please check that the file is a correct\n"
			"PEM-encoded certificate." );
			delete fileDialog;
			return;
		}
		listItem = new QListViewItem( &rootCaList,
			"File",
			result );
	}

        delete fileDialog;
}

void CertificateDialog::removeCa(){
	QListViewItem * selected = NULL;
	ca_db_item removed;

	selected = rootCaList.selectedItem();

	if( selected == NULL ){
		return;
	}

	if( selected->text(0) == "File"  ){
		removed.setImportMethod(CertificateSetItem::IMPORTMETHOD_FILE);
	}
	else if( selected->text(0) == "Directory"  ){
		removed.setImportMethod(CertificateSetItem::IMPORTMETHOD_DIRECTORY);
	}
	else{
		removed.setImportMethod(CertificateSetItem::IMPORTMETHOD_OTHER);
	}

	removed.setImportParameter(selected->text(1).ascii());

	certDb->lock();
	certDb->remove( &removed );
	certDb->unlock();

	delete( selected );
}




void CertificateDialog::addCertChain(){
	certificate * cert = NULL;
	CertChainItem * listItem = NULL;
	QString result;
#ifndef OPIE
	QFileDialog * fileDialog = new QFileDialog( this, "certDir", TRUE );
        fileDialog->setCaption(
			QFileDialog::tr( "Choose a certificate file" ) );
#else
	OFileDialog * fileDialog = new OFileDialog("Choose a certificate file", this, TRUE, OFileSelector::EXTENDED_ALL, "." );
#endif
        if( fileDialog->exec() == QDialog::Accepted ){
#ifdef OPIE
		result = fileDialog->fileName();
#else
		result = fileDialog->selectedFile();
#endif
        	//editCertificate.setText( fileCertificate->selectedFile() );
		try{
			cert = new certificate( result.ascii() );

			certChain->lock();
			certChain->add_certificate( cert );
			certChain->unlock();

		} catch( certificate_exception & exc ){
			QMessageBox::critical( this, "Minisip",
  		  	"Minisip could not open that certificate file.\n"
    			"Please check that the file is a correct\n"
			"PEM-encoded certificate." );
			delete fileDialog;
			return;
		}
		if( certChainList.childCount() > 0 ){
			if( lastChain->getCert()->get_issuer()
				!= cert->get_name() ){
				QMessageBox::critical( this, "Minisip",
  		  		"The selected certificate is not\n"
    				"assigned to the issuer of the previous\n"
				"one." );
				delete fileDialog;
				return;
			}
			else
			{

				listItem = new CertChainItem(
				*lastChain,
				cert );

				lastChain = listItem;
			}
		}

	}

        delete fileDialog;
}

void CertificateDialog::removeCertChain(){

	MRef<CertChainItem *> temp;
	temp = (CertChainItem *)lastChain->parent();
//	delete( lastChain );
	lastChain=NULL;

	if( certChainList.childCount() == 0 ){
		addCertChainButton.setEnabled( false );
		removeCertChainButton.setEnabled( false );
		certLabel.setText( "" );
		pkLabel.setText("");
		pkBrowse.setEnabled( false );
		lastChain = NULL;
	}
	else{
		lastChain = temp;
	}
}

void CertificateDialog::chooseCert(){
	CertChainItem * listItem = NULL;
	certificate * chosenCert = NULL;
	QString result;
#ifndef OPIE
	QFileDialog * fileDialog = new QFileDialog( this, "certDir", TRUE );
        fileDialog->setCaption(
			QFileDialog::tr( "Choose a certificate file" ) );
#else
	OFileDialog * fileDialog = new OFileDialog("Choose a certificate file", this, TRUE, OFileSelector::EXTENDED_ALL, "." );
#endif
        if( fileDialog->exec() == QDialog::Accepted ){
#ifdef OPIE
		result = fileDialog->fileName();
#else
		result = fileDialog->selectedFile();
#endif
		try{
			chosenCert = new certificate( result.ascii() );

		} catch( certificate_exception & exc ){
			QMessageBox::critical( this, "Minisip",
  		  	"Minisip could not open that certificate file.\n"
    			"Please check that the file is a correct\n"
			"PEM-encoded certificate." );
			delete fileDialog;
			return;
		}

		cert = chosenCert;
		certLabel.setText( result );

		/* Set this certificate as root of the cert chain */

		listItem = new CertChainItem( &certChainList, chosenCert );
		certChain->clear();
		certChain->add_certificate( chosenCert );

		pkBrowse.setEnabled( true );
		addCertChainButton.setEnabled( true );
		removeCertChainButton.setEnabled( true );
		lastChain = listItem;
	}

        delete fileDialog;
}

void CertificateDialog::choosePk(){
	CertChainItem * listItem = NULL;
	QString result;
#ifndef OPIE
	QFileDialog * fileDialog = new QFileDialog( this, "certDir", TRUE );
        fileDialog->setCaption(
			QFileDialog::tr( "Choose a private key file" ) );
#else
	OFileDialog * fileDialog = new OFileDialog("Choose a private key file", this, TRUE, OFileSelector::EXTENDED_ALL, "." );
#endif
        if( fileDialog->exec() == QDialog::Accepted ){
#ifdef OPIE
		result = fileDialog->fileName();
#else
		result = fileDialog->selectedFile();
#endif
		try{
			cert->set_pk( result.ascii() );
		}
		catch( certificate_exception_pkey & exc ){
			QMessageBox::critical( this, "Minisip",
  		  	"The private key file you selected does not.\n"
    			"match the selected certificate." );
			delete fileDialog;
			return;
		}

		catch( certificate_exception & exc ){
			QMessageBox::critical( this, "Minisip",
  		  	"Minisip could not open that file.\n"
    			"Please check that the file is a correct\n"
			"PEM-encoded private key." );
			delete fileDialog;
			return;
		}

		pkLabel.setText( result );
		certChainList.setEnabled( true );
		((CertChainItem *)(certChainList.firstChild()))->pkFileName = result.ascii();

	}

        delete fileDialog;
}

void CertificateDialog::accept(){
	//CertChainItem * item = (CertChainItem *)certChainList.firstChild();

	/* Rebuild the certificate chain */
	//certChain->clear();
	/*while( item->childCount() != 0 ){
		certChain->lock();
		certChain->add_certificate( item->getCert() );
		certChain->unlock();
		item = (CertChainItem *) item->firstChild();
	}*/

	QTabDialog::accept();
}



CertChainItem::CertChainItem( QListView * parent, MRef<certificate *> cert ):
	QListViewItem( parent, cert->get_cn().c_str(),
			cert->get_issuer_cn().c_str()  ){

	this->cert = cert;
	this->fileName = cert->get_file();
	this->pkFileName = cert->get_pk_file();

}

CertChainItem::CertChainItem( QListViewItem * parent, MRef<certificate *> cert ):
	QListViewItem( parent, cert->get_cn().c_str(),
			cert->get_issuer_cn().c_str()  ){

	this->cert = cert;
	this->fileName = cert->get_file();
	this->pkFileName = cert->get_pk_file();

}



CertChainItem::~CertChainItem(){

	//delete( cert );

}
