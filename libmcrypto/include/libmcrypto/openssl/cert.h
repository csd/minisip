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

class certificate;

class LIBMCRYPTO_API ossl_ca_db: public ca_db{
	public:
		ossl_ca_db();
		~ossl_ca_db();
		
		X509_STORE * get_db();
		virtual std::string getMemObjectType() const {return "ossl_ca_db";}
		void add_directory( std::string dir );
		void add_file( std::string file );
		void add_certificate( certificate * cert );

	private:
		X509_STORE * cert_db;		
};

class LIBMCRYPTO_API ossl_priv_key: public priv_key{
	public:
		ossl_priv_key( const std::string &private_key_filename );
		ossl_priv_key( char *derEncPk, int length,
			       const std::string &password,
			       const std::string &path );

		~ossl_priv_key();

		const std::string &get_file() const;

		int sign_data( unsigned char * data, int data_length, 
			       unsigned char * sign,
			       int * sign_length );

		int denvelope_data( unsigned char * data,
				    int size,
				    unsigned char *retdata,
				    int *retsize,
				    unsigned char *enckey,
				    int enckeylgth,
				    unsigned char *iv);

		bool private_decrypt(const unsigned char *data, int size,
				     unsigned char *retdata, int *retsize);

		EVP_PKEY * get_openssl_private_key(){return private_key;};

	private:
		EVP_PKEY * private_key;
		std::string pk_file;
};

class LIBMCRYPTO_API ossl_certificate: public certificate{
	public:
		ossl_certificate();
		ossl_certificate( X509 * ossl_cert );
		ossl_certificate( const std::string &cert_filename );
		ossl_certificate( unsigned char * der_cert, int length );
		ossl_certificate( unsigned char * certData, int length, std::string path );
		~ossl_certificate();
		virtual std::string getMemObjectType() const {return "certificate";}
		
		int control( ca_db * cert_db );

		int get_der_length();
		void get_der( unsigned char * output,
			      unsigned int * length );
		int envelope_data( unsigned char * data, int size, unsigned char *retdata, int *retsize,
		              unsigned char *enckey, int *enckeylgth, unsigned char** iv);
		int denvelope_data(unsigned char * data, int size, unsigned char *retdata, int *retsize,
		               unsigned char *enckey, int enckeylgth, unsigned char *iv);

		int sign_data( unsigned char * data, int data_length, 
			       unsigned char * sign, int * sign_length );
		int verif_sign( unsigned char * data, int data_length,
				unsigned char * sign, int sign_length );

		bool public_encrypt(const unsigned char *data, int size,
				    unsigned char *retdata, int *retsize);

		std::string get_name();
		std::string get_cn();
		std::string get_issuer();
		std::string get_issuer_cn();

		bool check_pk( MRef<priv_key *> pk);

		X509 * get_openssl_certificate(){return cert;};
	private:
		X509 * cert;
};

class LIBMCRYPTO_API ossl_certificate_chain: public certificate_chain{
	public:
		ossl_certificate_chain();
		ossl_certificate_chain( MRef<certificate *> cert );
		virtual ~ossl_certificate_chain();
		
		virtual std::string getMemObjectType() const {return "ossl_certificate_chain";}
		
		int control( MRef<ca_db *> cert_db );
};

#endif
