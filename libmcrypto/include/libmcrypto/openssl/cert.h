/*
  Copyright (C) 2005, 2004 Erik Eliasson, Johan Bilien
  Copyright (C) 2006 Mikael Magnusson

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

/*
 * Authors: Erik Eliasson <eliasson@it.kth.se>
 *          Johan Bilien <jobi@via.ecp.fr>
 *          Mikael Magnusson <mikma@users.sourceforge.net>
*/


#ifndef CERT_H
#define CERT_H

#include <libmcrypto/config.h>
#include<libmcrypto/cert.h>

/*Include openssl/err.h before any <list/map/hash/vector> ... it causes
compilation under EVC 4.0 to fail, collision between STLPort and Openssl
.....\minisip.evc4\openssl098a\inc32\openssl\err.h(297) : error C2955: 'hash' : use of class template requires template argument list
        ....\minisip.evc4\stlport501\stlport\stl\_hash_fun.h(40) : see declaration of 'hash'
*/
#ifdef _WIN32_WCE
//openssl's err.h must be included before ANY <vector/map/list/hash/...> include ...
//otherwise, it causes some conflict between STLPort and OpenSSL (in MS EVC++ 4.0)
#	include<openssl/err.h>
#endif
#include<openssl/ossl_typ.h> //include only type definitions ... nothing else is needed here

#include<string>
#include<list>
#include<libmutil/Mutex.h>
#include<libmutil/MemObject.h>
#include<libmutil/Exception.h>

class Certificate;

class LIBMCRYPTO_API OsslCertificateSet: public CertificateSet{
	public:
		OsslCertificateSet();
		~OsslCertificateSet();

		X509_STORE * getDb();
		virtual std::string getMemObjectType() const {return "OsslCertificateSet";}
		//void addDirectory( std::string dir );
		//void addFile( std::string file );
		MRef<CertificateSetItem*> addCertificate( MRef<Certificate *> cert );

	private:
		X509_STORE * cert_db;
};

class LIBMCRYPTO_API OsslPrivateKey: public PrivateKey{
	public:
		OsslPrivateKey( const std::string &private_key_filename );
		OsslPrivateKey( char *derEncPk, int length,
			       const std::string &password,
			       const std::string &path );

		~OsslPrivateKey();

		const std::string &getFile() const;

		bool checkCert( Certificate * cert);

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

		bool privateDecrypt(const unsigned char *data, int size,
				     unsigned char *retdata, int *retsize);

		EVP_PKEY * getOpensslPrivateKey(){return private_key;};

	private:
		EVP_PKEY * private_key;
		std::string pk_file;
};

class LIBMCRYPTO_API OsslCertificate: public Certificate{
	public:
		OsslCertificate();
		OsslCertificate( X509 * Osslcert );
		OsslCertificate( const std::string &cert_filename );
		OsslCertificate( unsigned char * der_cert, int length );
		OsslCertificate( unsigned char * certData, int length, std::string path );
		~OsslCertificate();
		virtual std::string getMemObjectType() const {return "Certificate";}

		int control( CertificateSet * cert_db );

		int getDerLength();
		void getDer( unsigned char * output,
			      unsigned int * length );
		int envelopeData( unsigned char * data, int size, unsigned char *retdata, int *retsize,
		              unsigned char *enckey, int *enckeylgth, unsigned char** iv);
		int denvelopeData(unsigned char * data, int size, unsigned char *retdata, int *retsize,
		               unsigned char *enckey, int enckeylgth, unsigned char *iv);

		int signData( unsigned char * data, int data_length,
			       unsigned char * sign, int * sign_length );
		int verifSign( unsigned char * data, int data_length,
				unsigned char * sign, int sign_length );

		bool publicEncrypt(const unsigned char *data, int size,
				    unsigned char *retdata, int *retsize);

		std::string getName();
		std::string getCn();
		std::vector<std::string> getAltName( SubjectAltName type );
		std::vector<std::string> getSubjectInfoAccess();
		std::string getIssuer();
		std::string getIssuerCn();

		bool verifySignedBy(MRef<Certificate*> cert);

		X509 * getOpensslCertificate(){return cert;};
	private:
		X509 * cert;
};

class LIBMCRYPTO_API OsslCertificateChain: public CertificateChain{
	public:
		OsslCertificateChain();
		OsslCertificateChain( MRef<Certificate *> cert );
		virtual ~OsslCertificateChain();

		virtual std::string getMemObjectType() const {return "OsslCertificateChain";}

		int control( MRef<CertificateSet *> cert_db );
};

#endif
