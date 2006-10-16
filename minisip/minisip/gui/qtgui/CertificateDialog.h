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

#ifndef CERTIFICATEDIALOG_H
#define CERTIFICATEDIALOG_H

#include<config.h>

#ifdef OPIE
#include<opie/ofiledialog.h>
#else
#include<qfiledialog.h>
#endif

#include<qdialog.h>
#include<qpushbutton.h>
#include<qlistview.h>
#include<qlistbox.h>
#include<qlayout.h>
#include<qgroupbox.h>
#include<qlineedit.h>
#include<qmessagebox.h>
#include<qlabel.h>
#include<qtabdialog.h>

#include<libmutil/cert.h>
#include<libmutil/MemObject.h>


class CertChainItem;

class CertificateDialog : public QTabDialog{
	Q_OBJECT;
	public:
		CertificateDialog( QWidget *parent=0 );

		void setCertChain( MRef<certificate_chain *> chain );
		void setRootCa( MRef<ca_db *> rootDb );

		QListView & getCertChainList(){ return certChainList; };

	public slots:
		void addCaDir();
		void addCaFile();
		void removeCa();
		void addCertChain();
		void removeCertChain();
		void chooseCert();
		void choosePk();

		virtual void accept();

	private:
		QWidget   persTab;
		QGroupBox rootCaBox;
		QGroupBox certChainBox;
		QGroupBox pkBox;

		QVBoxLayout layout;

		//QFrame rootCaFrame;
		QGridLayout rootCaLayout;
		QGridLayout certChainLayout;
		QListView rootCaList;
		QListView certChainList;
		QPushButton addCaDirButton;
		QPushButton addCaFileButton;
		QPushButton removeCaButton;

		QPushButton addCertChainButton;
		QPushButton removeCertChainButton;

		QLabel certLabel;
		QPushButton certBrowse;

		QLabel pkLabel;
		QPushButton pkBrowse;

		MRef<CertChainItem *> lastChain;

		MRef<ca_db *> certDb;
		MRef<certificate_chain *> certChain;
		MRef<certificate *> cert;

};

class CertChainItem: public QListViewItem, public MObject{
	public:
		CertChainItem( QListView * parent, MRef<certificate *> cert );
		CertChainItem( QListViewItem * parent, MRef<certificate *> cert );
		~CertChainItem();

		MRef<certificate *> getCert(){ return cert; }

		virtual string getMemObjectType() const {return "CertChainItem";}

		string fileName;
		string pkFileName;
	private:
		MRef<certificate *> cert;
};


#endif
