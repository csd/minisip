/*
 This library is free software; you can redistribute it and/or
 modify it under the terms of the GNU Lesser General Public
 License as published by the Free Software Foundation; either
 version 2.1 of the License, or (at your option) any later version.
 
 This library is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 Lesser General Public License for more details.
 
 You should have received a copy of the GNU Lesser General Public
 License along with this library; if not, write to the Free Software
 Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
 */

/* Copyright (C) 2004 
 *
 * Authors: Erik Eliasson <eliasson@it.kth.se>
 *          Johan Bilien <jobi@via.ecp.fr>
*/

#ifndef CERTIFICATE_H
#define CERTIFICATE_H


#include<string>
#include<list>
#include<libmutil/Mutex.h>
#include<libmutil/MemObject.h>


class Certificate;
//struct gnutls_x509_crt_t;
//struct gnutls_x509_privkey_t;

//struct X509;
//struct EVP_PKEY;

#define CERT_DB_ITEM_TYPE_OTHER  0
#define CERT_DB_ITEM_TYPE_FILE   1
#define CERT_DB_ITEM_TYPE_DIR    2

#include<gnutls/openssl.h>

class CADbItem{
	public:
		std::string item;
		int type;

		bool operator ==(const CADbItem item2){ return (
				item2.item == item && 
				item2.type == type);};
};


class CADb: public MObject{
	public:
		CADb();
		~CADb();
		
		//X509_STORE * getDb();
		virtual std::string getMemObjectType() const {return "ca_db";}
		//void addDirectory( std::string dir );
		void addFile( std::string file );
		void addCertificate( Certificate * cert );
		std::list<CADbItem *> &get_items();
		CADbItem * get_next();
		void initIndex();
		void lock();
		void unlock();

		void remove( CADbItem * removedItem );

	private:
		//X509_STORE * certDb;
		gnutls_x509_crt_t * caList;

		std::list<CADbItem *>::iterator itemsIndex;

		std::list<CADbItem *> items;

                Mutex mLock;
		
		
};

class Certificate: public MObject{
	public:
		Certificate();
		//Certificate( X509 * openssl_cert );
		Certificate( const std::string certFilename );
		Certificate( const std::string certFilename,
			     const std::string privateKeyFilename );
		Certificate( unsigned char * derCert, int length );
		~Certificate();
		virtual std::string getMemObjectType() const {return "Certificate";}
		
		int control( CADb * certDb );

		void getDer( unsigned char * output, unsigned int * length );
		int signData( unsigned char * data, int data_length, 
			      unsigned char * sign, int * sign_length );
		int verifSign( unsigned char * sign, int sign_length,
			       unsigned char * data, int data_length );

		std::string getName();
		std::string getCn();
		std::string getIssuer();
		std::string getIssuerCn();

		std::string getFile();
		std::string getPkFile();

		void setPk( std::string file );
                void setEncPk(char * pkInput, int length, string password  )
		//EVP_PKEY * get_openssl_private_key(){return private_key;};
		//X509 * get_openssl_Certificate(){return cert;};
	private:
		EVP_PKEY * private_key;
		X509 * cert;
		gnutls_x509_crt_t * cert;
		gnutls_x509_privkey_t * privateKey;

		std::string file;
		std::string pkFile;
};

class CertificateChain: public MObject{
	public:
		CertificateChain();
		CertificateChain( MRef<Certificate *> cert );
		~CertificateChain();
		
		virtual std::string getMemObjectType() const {return "CertificateChain";}
		
		void addCertificate( MRef<Certificate *> cert );
		void removeCertificate( MRef<Certificate *> cert );
		void removeLast();

		int control( MRef<CADb *> certDb );
		MRef<Certificate *> getNext();
		MRef<Certificate *> getFirst();

		void clear();

		int length(){ return certList.size(); };
		void lock();
		void unlock();

		bool isEmpty();

		void initIndex();
	private:
		std::list< MRef<Certificate *> > certList;
		std::list< MRef<Certificate *> >::iterator item;
//		pthread_mutex_t mLock;
                Mutex mLock;
};

class CertificateException{
	public:
		CertificateException(){};
		CertificateException( std::string message ):message(message){};

		std::string getMessage(){ return message; };
	protected:
		std::string message;
};

class CertificateExceptionFile : public CertificateException{
	public:
		CertificateExceptionFile( std::string message ):CertificateException(message){};
};

class CertificateExceptionInit : public CertificateException{
	public:
		CertificateExceptionInit( std::string message ):CertificateException(message){};
};

class CertificateExceptionPkey : public CertificateException{
	public:
		CertificateExceptionPkey( std::string message ):CertificateException(message){};
};

class CertificateExceptionChain : public CertificateException{
	public:
		CertificateExceptionChain( std::string message ):CertificateException(message){};
};

#endif
