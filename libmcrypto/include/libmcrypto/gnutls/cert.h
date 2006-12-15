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

class gtls_certificate;

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

class LIBMCRYPTO_API gtls_certificate: public certificate{
	public:
		gtls_certificate();
		//gtls_certificate( X509 * openssl_cert );
		gtls_certificate( const std::string cert_filename );
		gtls_certificate( const std::string cert_filename,
			     const std::string private_key_filename );
		gtls_certificate( unsigned char * der_cert, int length );
		~gtls_certificate();
		virtual std::string getMemObjectType() const {return "gtls_certificate";}
		
		int control( ca_db * cert_db );

		int get_der_length();
		void get_der( unsigned char * output );
		void get_der( unsigned char * output, unsigned int * length );
		int envelope_data( unsigned char * data, int size, unsigned char *retdata, int *retsize,
		              unsigned char *enckey, int *enckeylgth, unsigned char** iv);
		int denvelope_data(unsigned char * data, int size, unsigned char *retdata, int *retsize,
		               unsigned char *enckey, int enckeylgth, unsigned char *iv);

		int sign_data( unsigned char * data, int data_length, 
			       unsigned char * sign, int * sign_length );
		int verif_sign( unsigned char * sign, int sign_length,
				unsigned char * data, int data_length );

		std::string get_name();
		std::string get_cn();
		std::string get_issuer();
		std::string get_issuer_cn();

		void set_pk( std::string file );
		void set_encpk(char * pkInput, int length,
			       std::string password, std::string path );
		bool has_pk();

		gnutls_x509_crt_t get_certificate(){return cert;};
		gnutls_x509_privkey_t get_private_key(){return privateKey;};

	protected:
		void openFromFile( std::string fileName );


	private:
		gnutls_x509_privkey_t privateKey;
		gnutls_x509_crt_t cert;
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
