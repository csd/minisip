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

class gtls_certificate;

class gtls_rsa_priv{
	public:
		gtls_rsa_priv( gnutls_x509_privkey_t aKey );
		~gtls_rsa_priv();

		bool decrypt( const unsigned char *data, int size,
			      unsigned char *retdata, int *retsize) const;

	private:
		gcry_sexp_t m_key;
};

class gtls_rsa_pub{
	public:
		gtls_rsa_pub( gnutls_x509_crt_t aCert );
		~gtls_rsa_pub();

		bool encrypt( const unsigned char *data, int size,
			      unsigned char *retdata, int *retsize) const;

	private:
		gcry_sexp_t m_key;
};

class LIBMCRYPTO_API gtls_ca_db_item: public ca_db_item{
	public:
		gtls_ca_db_item();
		virtual ~gtls_ca_db_item();

		gnutls_x509_crt_t* certs;
		unsigned int num_certs;
};

class LIBMCRYPTO_API gtls_ca_db: public ca_db{
	public:
		gtls_ca_db();
		virtual ~gtls_ca_db();
		
		bool getDb(gnutls_x509_crt_t ** db, size_t * db_length );
		virtual std::string getMemObjectType() const {return "gtls_ca_db";}

	protected:
		ca_db_item* create_dir_item( std::string dir );
		ca_db_item* create_file_item( std::string file );
		ca_db_item* create_cert_item( certificate* cert );

	private:
		gnutls_x509_crt_t * caList;
		size_t caListLength;
};

class LIBMCRYPTO_API gtls_priv_key: public priv_key{
	public:
		gtls_priv_key( const std::string &private_key_filename );
		gtls_priv_key( char *derEncPk, int length,
			       const std::string &password,
			       const std::string &path );

		~gtls_priv_key();

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

		bool private_decrypt( const unsigned char *data, int size,
				     unsigned char *retdata, int *retsize);

		gnutls_x509_privkey_t get_private_key(){return privateKey;};

	private:
		gnutls_x509_privkey_t privateKey;
		gtls_rsa_priv *rsaPriv;
		std::string pk_file;
};

class LIBMCRYPTO_API gtls_certificate: public certificate{
	public:
		gtls_certificate();
		gtls_certificate( const std::string cert_filename );
		gtls_certificate( unsigned char * der_cert, int length );
		~gtls_certificate();
		virtual std::string getMemObjectType() const {return "gtls_certificate";}
		
		int control( ca_db * cert_db );

		int get_der_length();
		void get_der( unsigned char * output );
		void get_der( unsigned char * output, unsigned int * length );
		int envelope_data( unsigned char * data, int size, unsigned char *retdata, int *retsize,
		              unsigned char *enckey, int *enckeylgth, unsigned char** iv);

		int sign_data( unsigned char * data, int data_length, 
			       unsigned char * sign, int * sign_length );
		int verif_sign( unsigned char * data, int data_length,
				unsigned char * sign, int sign_length );

		bool public_encrypt( const unsigned char *data, int size,
				    unsigned char *retdata, int *retsize);

		std::string get_name();
		std::string get_cn();
		std::vector<std::string> get_alt_name( SubjectAltName type );
		std::string get_issuer();
		std::string get_issuer_cn();

		bool check_pk( MRef<priv_key*> pk );

		gnutls_x509_crt_t get_certificate(){return cert;};

	protected:
		void openFromFile( std::string fileName );


	private:
		gnutls_x509_crt_t cert;
		gtls_rsa_pub *rsaKey;
};

class gtls_certificate_chain: public certificate_chain{
	public:
		gtls_certificate_chain();
		gtls_certificate_chain( MRef<certificate *> cert );
		virtual ~gtls_certificate_chain();
		
		virtual std::string getMemObjectType() const {return "gtls_certificate_chain";}
		
		int control( MRef<ca_db *> cert_db );
};

#endif
