/*
  Copyright (C) 2005, 2004 Erik Eliasson, Johan Bilien
  
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
*/


#ifndef CERT_H
#define CERT_H

#include <libmcrypto/config.h>

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
using namespace std;

class certificate;

#define CERT_DB_ITEM_TYPE_OTHER  0
#define CERT_DB_ITEM_TYPE_FILE   1
#define CERT_DB_ITEM_TYPE_DIR    2

class LIBMCRYPTO_API ca_db_item{
	public:
		std::string item;
		int type;

		bool operator ==(const ca_db_item item2){ return (
				item2.item == item && 
				item2.type == type);};
};


class LIBMCRYPTO_API ca_db: public MObject{
	public:
		ca_db();
		~ca_db();
		
		X509_STORE * get_db();
		virtual std::string getMemObjectType() const {return "ca_db";}
		void add_directory( std::string dir );
		void add_file( std::string file );
		void add_certificate( certificate * cert );
		std::list<ca_db_item *> &get_items();
		ca_db_item * get_next();
		void init_index();
		void lock();
		void unlock();

		void remove( ca_db_item * removedItem );

	private:
		X509_STORE * cert_db;

		std::list<ca_db_item *>::iterator items_index;

		std::list<ca_db_item *> items;

//		pthread_mutex_t mLock;
                Mutex mLock;
		
		
};

class LIBMCRYPTO_API certificate: public MObject{
	public:
		certificate();
		certificate( X509 * openssl_cert );
		certificate( const std::string cert_filename );
		certificate( const std::string cert_filename,
			     const std::string private_key_filename );
		certificate( unsigned char * der_cert, int length );
                certificate( unsigned char * certData, int length, string path );
		~certificate();
		virtual std::string getMemObjectType() const {return "certificate";}
		
		int control( ca_db * cert_db );

		int get_der_length();
		void get_der( unsigned char * output );
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

		std::string get_file();
		std::string get_pk_file();
                   
		void set_pk( std::string file );
                void set_encpk(char *derEncPk, int length, string password, string path);
		EVP_PKEY * get_openssl_private_key(){return private_key;};
		X509 * get_openssl_certificate(){return cert;};
	private:
		EVP_PKEY * private_key;
		X509 * cert;

		std::string file;
		std::string pk_file;
};

class LIBMCRYPTO_API certificate_chain: public MObject{
	public:
		certificate_chain();
		certificate_chain( MRef<certificate *> cert );
		~certificate_chain();
		
		virtual std::string getMemObjectType() const {return "certificate_chain";}
		
		void add_certificate( MRef<certificate *> cert );
		void remove_certificate( MRef<certificate *> cert );
		void remove_last();

		int control( MRef<ca_db *> cert_db );
		MRef<certificate *> get_next();
		MRef<certificate *> get_first();

		void clear();

		int length(){ return (int)cert_list.size(); };
		void lock();
		void unlock();

		bool is_empty();

		void init_index();
	private:
		std::list< MRef<certificate *> > cert_list;
		std::list< MRef<certificate *> >::iterator item;
//		pthread_mutex_t mLock;
                Mutex mLock;
};

class LIBMCRYPTO_API certificate_exception : public Exception{
	public:
		certificate_exception( const char *desc):Exception(desc){};
};

class LIBMCRYPTO_API certificate_exception_file : public certificate_exception{
	public:
		certificate_exception_file( const char *message ):certificate_exception(message){};
};

class LIBMCRYPTO_API certificate_exception_init : public certificate_exception{
	public:
		certificate_exception_init( const char *message ):certificate_exception(message){};
};

class LIBMCRYPTO_API certificate_exception_pkey : public certificate_exception{
	public:
		certificate_exception_pkey( const char *message ):certificate_exception(message){};
};

class LIBMCRYPTO_API certificate_exception_chain : public certificate_exception{
	public:
		certificate_exception_chain( const char *message ):certificate_exception(message){};
};

#endif
