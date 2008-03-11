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
 *          Mikael Magnusson <mikma@users.sourceforge.net>
*/


#ifndef CERT_H
#define CERT_H

#include <libmcrypto/config.h>

#include <libmcrypto/cert.h>

#include<string>
#include<list>
#include<libmutil/Mutex.h>
#include<libmutil/MemObject.h>
#include<libmutil/Exception.h>


#include<gnutls/x509.h>
#include<gcrypt.h>

class GtlsCertificate;

class GtlsRsaPriv{
	public:
		GtlsRsaPriv( gnutls_x509_privkey_t aKey );
		~GtlsRsaPriv();

		bool decrypt( const unsigned char *data, int size,
			      unsigned char *retdata, int *retsize) const;

	private:
		gcry_sexp_t m_key;
};

class Gtlsrsa_pub{
	public:
		Gtlsrsa_pub( gnutls_x509_crt_t aCert );
		~Gtlsrsa_pub();

		bool encrypt( const unsigned char *data, int size,
			      unsigned char *retdata, int *retsize) const;

	private:
		gcry_sexp_t m_key;
};

class LIBMCRYPTO_API GtlsCertificateSetItem: public CertificateSetItem{
	public:
		GtlsCertificateSetItem();
		virtual ~GtlsCertificateSetItem();

		gnutls_x509_crt_t* certs;
		unsigned int num_certs;
};

class LIBMCRYPTO_API GtlsCertificateSet: public CertificateSet{
	public:
		GtlsCertificateSet();
		virtual ~GtlsCertificateSet();

		bool getDb(gnutls_x509_crt_t ** db, size_t * db_length );
		virtual std::string getMemObjectType() const {return "GtlsCertificateSet";}

	protected:
		//MRef<CertificateSetItem*> createDirItem( std::string dir );
		//MRef<CertificateSetItem*> createFileItem( std::string file );
		MRef<CertificateSetItem*> createCertItem( MRef<Certificate*> cert );

	private:
		gnutls_x509_crt_t * caList;
		size_t caListLength;
};

class LIBMCRYPTO_API GtlsPrivateKey: public PrivateKey{
	public:
		GtlsPrivateKey( const std::string &private_key_filename );
		GtlsPrivateKey( char *derEncPk, int length,
			       const std::string &password,
			       const std::string &path );

		~GtlsPrivateKey();

		const std::string &getFile() const;

		bool checkCert( Certificate* cert );

		int signData( unsigned char * data, int data_length,
			       unsigned char * sign,
			       int * sign_length );

		int denvelopeData( unsigned char * data,
				    int size,
				    unsigned char *retdata,
				    int *retsize,
				    unsigned char *enckey,
				    int enckeylgth,
				    unsigned char *iv);

		bool privateDecrypt( const unsigned char *data, int size,
				     unsigned char *retdata, int *retsize);

		gnutls_x509_privkey_t getPrivateKey(){return privateKey;};

	private:
		gnutls_x509_privkey_t privateKey;
		GtlsRsaPriv *rsaPriv;
		std::string pk_file;
};

class LIBMCRYPTO_API GtlsCertificate: public Certificate{
	public:
		GtlsCertificate();
		GtlsCertificate( const std::string cert_filename );
		GtlsCertificate( unsigned char * der_cert, int length );
		~GtlsCertificate();
		virtual std::string getMemObjectType() const {return "GtlsCertificate";}

		int control( CertificateSet * cert_db );

		int getDerLength();
		void getDer( unsigned char * output );
		void getDer( unsigned char * output, unsigned int * length );
		int envelopeData( unsigned char * data, int size, unsigned char *retdata, int *retsize,
		              unsigned char *enckey, int *enckeylgth, unsigned char** iv);

		int signData( unsigned char * data, int data_length,
			       unsigned char * sign, int * sign_length );
		int verifSign( unsigned char * data, int data_length,
				unsigned char * sign, int sign_length );

		bool publicEncrypt( const unsigned char *data, int size,
				    unsigned char *retdata, int *retsize);

		std::string getName();
		std::string getCn();
		std::vector<std::string> getAltName( SubjectAltName type );
		std::vector<std::string> getSubjectInfoAccess();
		std::string getIssuer();
		std::string getIssuerCn();

		bool verifySignedBy(MRef<Certificate*> cert);

		gnutls_x509_crt_t getCertificate(){return cert;};

	protected:
		void openFromFile( std::string fileName );


	private:
		gnutls_x509_crt_t cert;
		Gtlsrsa_pub *rsaKey;
};

class GtlsCertificateChain: public CertificateChain{
	public:
		GtlsCertificateChain();
		GtlsCertificateChain( MRef<Certificate *> cert );
		virtual ~GtlsCertificateChain();

		virtual std::string getMemObjectType() const {return "GtlsCertificateChain";}

		int control( MRef<CertificateSet *> cert_db );
};

#endif
