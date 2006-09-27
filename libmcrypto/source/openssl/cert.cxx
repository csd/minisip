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

#include<config.h>

#include<libmcrypto/cert.h>

extern "C"{
	#include<openssl/rsa.h>
	#include<openssl/evp.h>
	#include<openssl/objects.h>
	#include<openssl/x509.h>
	#include<openssl/err.h>
	#include<openssl/pem.h>
	#include<openssl/ssl.h>
        #include<openssl/bio.h>
        #include <openssl/pkcs12.h>
}


#include<stdio.h>
#include<assert.h>

#include<iostream>
#include <fstream>

using namespace std;

certificate::certificate():private_key(NULL),cert(NULL){

}

certificate::certificate( X509 * openssl_cert ):private_key(NULL){
	if( openssl_cert == NULL ){
		throw certificate_exception("X509 certificate is NULL");
	}
	
	cert = openssl_cert;
}

certificate::certificate( const string cert_filename ):private_key(NULL){
	FILE * fp;

	fp = fopen( cert_filename.c_str(), "r" );
	if( fp == NULL ){
		cerr << "Could not open the certificate file" << endl;
		throw certificate_exception_file(
				"Could not open the certificate file" );
	}

	cert = PEM_read_X509( fp, NULL, NULL, NULL );
	
	fclose( fp );
	
	if( cert == NULL ){
		cerr << "Invalid certificate file" << endl;
		throw certificate_exception_file(
				"Invalid certificate file" );
	}

	file = cert_filename;
}

certificate::certificate( const string cert_filename, const string private_key_filename ){
	FILE * fp;
	
	fp = fopen( cert_filename.c_str(), "r" );
	if( fp == NULL ){
		cerr << "Could not open the certificate file" << endl;
		throw certificate_exception_file(
				"Could not open the certificate file" );
	}

	cert = PEM_read_X509( fp, NULL, NULL, NULL );
	
	fclose( fp );
	
	if( cert == NULL ){
		cerr << "Invalid certificate file" << endl;
		throw certificate_exception_file(
				"Invalid certificate file" );
	}

	set_pk( private_key_filename );
	
	file = cert_filename;
}

certificate::certificate( unsigned char * certData, int length, string path ):private_key(NULL)
{
 /* tries to read a PEM certificate from memory, if that fails it tries to read it as a DER encoded cert*/
   BIO *mem;
   mem = BIO_new_mem_buf((void *)certData, length);
   if( cert == NULL )
     throw certificate_exception_init(
				      "Could not create the certificate" );
   
   cert = PEM_read_bio_X509(mem, NULL, 0 , NULL);
   if (cert == NULL)/*check if its a der encoded certificate*/
     {
	cert = d2i_X509_bio(mem, NULL);/*FIX, for some reason
					this does never succeed */
	if(NULL == cert)
	  {	     
	     cerr << "Invalid certificate file" << endl;
	     throw certificate_exception_file("Invalid certificate" );
	  }	
     }
   file = path;
}


certificate::certificate( unsigned char * der_cert, int length ):private_key(NULL){
	cert = X509_new();

	if( cert == NULL )
		throw certificate_exception_init(
				"Could not create the certificate" );

#if OPENSSL_VERSION_NUMBER >= 0x00908000L 
	d2i_X509( &cert, (const unsigned char**)&der_cert, length );
#else
	d2i_X509( &cert, (unsigned char**)&der_cert, length );
#endif
}
	
certificate::~certificate(){
	if( cert )
		X509_free( cert );
	cert = NULL;
	
	if( private_key )
		EVP_PKEY_free( private_key );
	private_key = NULL;

}

int certificate::envelope_data(unsigned char * data, int size, unsigned char *retdata, int *retsize,
				unsigned char *enckey, int* enckeylgth, unsigned char** iv){

	EVP_CIPHER_CTX ctx;
	*iv = (unsigned char*)malloc(EVP_CIPHER_iv_length(EVP_aes_128_cbc()));
	EVP_PKEY *public_key;
	int err;
	ERR_load_crypto_strings();
	int temp =0, tmp= 0;
	if( cert == NULL ){
#ifdef DEBUG_OUTPUT
                cerr << "You need a certificate to envelope the data" << endl;

#endif
                return -1;
	}

	public_key = X509_get_pubkey( cert );
	
	if( public_key == NULL ){
#ifdef DEBUG_OUTPUT
                cerr << "Cound not read public key from certificate" << endl;
#endif
                return -1;
	}
	
	/*inits*/
	EVP_CIPHER_CTX_init(&ctx);
	EVP_SealInit(&ctx, EVP_aes_128_cbc(), &enckey, enckeylgth, *iv, &public_key, 1);
	
	/*encrypt*/
	EVP_SealUpdate(&ctx, retdata, &temp, data, size);
	err = EVP_SealFinal(&ctx, retdata + temp, &tmp);
	if(err != 1){
		cout<<"An error occurred when enveloping the data"<<endl;
		return -1;
	}
	*retsize = temp + tmp;
	return 0;
}

int certificate::denvelope_data(unsigned char * data, int size, unsigned char *retdata, int *retsize,
                                unsigned char *enckey, int enckeylgth, unsigned char *iv){

        /*begin decrypt*/
        EVP_CIPHER_CTX ctx;
        int err, temp=0, tmp=0;

        ERR_load_crypto_strings();
        EVP_CIPHER_CTX_init(&ctx);
        EVP_OpenInit(&ctx, EVP_aes_128_cbc(), enckey, enckeylgth, iv, private_key);
        EVP_OpenUpdate(&ctx, retdata, &temp, data, size);
        err = EVP_OpenFinal(&ctx, retdata + temp , &tmp);
        if(err != 1){
		cout<<"An error occurred when deenevolping the data"<<endl; 		
		return -1;
	}
	*retsize = temp +tmp;
	/*end decrypt*/

	return 0;
}

int certificate::sign_data( unsigned char * data, int data_length,
	    		    unsigned char * sign, int * sign_length ){
	EVP_MD_CTX     ctx;
	int err;

	ERR_load_crypto_strings();
	
	if( private_key == NULL )
	{
		sign = NULL;
		*sign_length = 0;
#ifdef DEBUG_OUTPUT
		cerr << "You need a private key to sign data" << endl;
#endif

		return 1;
	}
	
	// FIXME
	EVP_SignInit( &ctx, EVP_sha1() );
	EVP_SignUpdate( &ctx, data, data_length );
	err = EVP_SignFinal( &ctx, sign, 
			(unsigned int*)sign_length, 
			private_key );

	//EVP_MD_CTX_cleanup( &ctx );

	if( err != 1 )
	{
#ifdef DEBUG_OUTPUT
		cerr << "An error occured when signing data" << endl;
#endif

		ERR_print_errors_fp( stderr );
		free( sign );
		return 1;
	}
	return 0;
}

int certificate::verif_sign( unsigned char * sign, int sign_length,
			     unsigned char * data, int data_length )
{
	EVP_PKEY *      public_key;
	EVP_MD_CTX      ctx;
	int err;
	
	ERR_load_crypto_strings();
	
	if( cert == NULL )
	{
#ifdef DEBUG_OUTPUT
		cerr << "You need a certificate to verify a signature" << endl;
#endif

		return -1;
	}

	public_key = X509_get_pubkey( cert );
	if( public_key == NULL )
	{
#ifdef DEBUG_OUTPUT
		cerr << "Cound not read public key from certificate" << endl;
#endif

		return -1;
	}
	EVP_VerifyInit( &ctx, EVP_sha1() );
	EVP_VerifyUpdate( &ctx, data, data_length );
	err = EVP_VerifyFinal( &ctx, sign, sign_length, public_key );
	EVP_PKEY_free( public_key );
	//EVP_MD_CTX_cleanup( &ctx );

	if( err < 0 )
	{
#ifdef DEBUG_OUTPUT
		cerr << "An error occured while verifying signature" << endl;
#endif

		ERR_print_errors_fp( stderr );
	}

	return err;
}
	
int certificate::get_der_length(){
	return i2d_X509( cert, NULL );
}

void certificate::get_der( unsigned char * output ){
	
	int temp = i2d_X509( cert, &output);

	// don't want it to be incremented:
	output -= temp;
}

string certificate::get_name(){
	string ret(
		X509_NAME_oneline( X509_get_subject_name( cert ),0 ,0 ));

	return ret;
}

string certificate::get_cn(){
	string name = get_name();
	size_t pos, pos2;

	pos = name.find( "/CN=" );

	if( pos == string::npos ){
		return "No common name";
	}

	pos2 = name.find( "/", pos + 1 );

	return name.substr( pos + 4, pos2 - pos - 4 );
}

string certificate::get_issuer(){
	string ret(
		X509_NAME_oneline( X509_get_issuer_name( cert ),0 ,0 ));

	return ret;
}

string certificate::get_issuer_cn(){
	string name = get_issuer();
	size_t pos, pos2;

	pos = name.find( "/CN=" );

	if( pos == string::npos ){
		return "No common name";
	}

	pos2 = name.find( "/", pos + 1 );

	return name.substr( pos + 4, pos2 - pos - 4 );
}

string certificate::get_file(){
	return file;
}

string certificate::get_pk_file(){
	return pk_file;
}


void certificate::set_pk( string file ){
	FILE * fp = NULL;
	
	fp = fopen( file.c_str(), "r" );
	if( fp == NULL ){
		cerr << "Could not open the private key file" << endl;
		throw certificate_exception_file(
				"Could not open the private key file" );
	}

	private_key = PEM_read_PrivateKey(fp, NULL, NULL, NULL);
	fclose( fp );
	
	if( private_key == NULL ){
		cerr << "Invalid private key file" << endl;
		throw certificate_exception_file(
				"Invalid private key file" );
	}

	/* Check that the private key matches the certificate */

	if( X509_check_private_key( cert, private_key ) != 1 ){
		cerr << "Private key does not match the certificate" << endl;
		throw certificate_exception_pkey(
			"The private key does not match the certificate" );
	}

	pk_file = file;



}

void certificate::set_encpk(char *derEncPk, int length, string password, string path)
{
   BIO *mem;  
   mem = BIO_new_mem_buf((void *)derEncPk, length);
   
   if(mem == NULL )
     {
	cerr << "Couldn't initiate bio buffer" << endl;
	throw certificate_exception_pkey("Couldn't initiate bio buffer" );
     }
      
   SSLeay_add_all_algorithms();
   
   private_key = PEM_read_bio_PrivateKey(mem, NULL, 0, (void*)password.c_str());
 
   if(private_key == NULL )
     {
	cerr << "Invalid private key data or password" << endl;
	throw certificate_exception_pkey("The private key is invalid or wrong password was used" );
     }
   
   /* Check that the private key matches the certificate */
   
   if( X509_check_private_key( cert, private_key ) != 1 )
     {
	cerr << "Private key does not match the certificate" << endl;
	throw certificate_exception_pkey(
					 "The private key does not match the certificate" );
     }
   pk_file=path;
}



int certificate::control( ca_db * cert_db ){
	int result;
	X509_STORE_CTX cert_store_ctx;

	X509_STORE_CTX_init( &cert_store_ctx, cert_db->get_db(), cert ,NULL );
	if( X509_STORE_CTX_get_error( &cert_store_ctx) != 0 ){
		//fprintf(stderr, "Could not initialize X509_STORE_CTX");
		cerr << "Could not initialize X509_STORE_CTX" << endl;
		exit( 1 );
	}

	result = X509_verify_cert( &cert_store_ctx );

	//fprintf(stderr, "result: %d\n", result );
#ifdef DEBUG_OUTPUT
	if( result == 0 ){
		cerr << result << endl;
		cerr << X509_verify_cert_error_string( cert_store_ctx.error ) << endl;
	}
#endif
	return result;
}
	

ca_db::ca_db(){
	cert_db = X509_STORE_new();

	if( cert_db == NULL ){
		throw certificate_exception_init(
				"Could not create the certificate db" );
	}
	items_index = items.begin();
}

ca_db::~ca_db(){
	X509_STORE_free( cert_db );
}

void ca_db::lock(){
        mLock.lock();
}

void ca_db::unlock(){
        mLock.unlock();
}

X509_STORE * ca_db::get_db(){
	return cert_db;
}

void ca_db::add_directory( string dir ){
	X509_LOOKUP * lookup = NULL;
	ca_db_item * item = new ca_db_item();
	
	lookup = X509_STORE_add_lookup( 
			cert_db, X509_LOOKUP_hash_dir() );
	if( lookup == NULL )
		throw certificate_exception_init(
			"Could not create a directory lookup");
	
	if( !X509_LOOKUP_add_dir( lookup, dir.c_str(), X509_FILETYPE_PEM ) )
		throw certificate_exception_file(
			(string("Could not open the directory ")+dir).c_str() );

	item->item = dir;
	item->type = CERT_DB_ITEM_TYPE_DIR;
	
	items.push_back( item );
	items_index = items.begin();
}

void ca_db::add_file( string file ){
	X509_LOOKUP * lookup = NULL;
	ca_db_item * item = new ca_db_item;
	
	lookup = X509_STORE_add_lookup( 
			cert_db, X509_LOOKUP_file() );
	if( lookup == NULL )
		throw certificate_exception_init(
			"Could not create a file lookup" );
	
	if( !X509_LOOKUP_load_file( lookup, file.c_str(), X509_FILETYPE_PEM ) )
		throw certificate_exception_file(
			("Could not open the file "+file).c_str() );
	
	item->item = file;
	item->type = CERT_DB_ITEM_TYPE_FILE;
	
	items.push_back( item );
	items_index = items.begin();
}

void ca_db::add_certificate( certificate * cert ){
	ca_db_item * item = new ca_db_item();
	X509_STORE_add_cert( cert_db, cert->get_openssl_certificate() );
	
	item->item = "";
	item->type = CERT_DB_ITEM_TYPE_OTHER;
	
	items.push_back( item );
	items_index = items.begin();

}

void ca_db::remove( ca_db_item * removedItem ){
	init_index();

	while( items_index != items.end() ){
		if( *(*items_index) == *removedItem ){
			items.erase( items_index );
			init_index();
			return;
		}
		items_index ++;
	}
	init_index();
}

list<ca_db_item *> &ca_db::get_items(){
	return items;
}

void ca_db::init_index(){
	items_index = items.begin();
}

ca_db_item * ca_db::get_next(){
	ca_db_item * tmp;
	
	if( items_index == items.end() ){
		items_index = items.begin();
		return NULL;
	}

	tmp = *items_index;
	items_index ++;
	return tmp;
}

certificate_chain::certificate_chain(){
	item = cert_list.begin();

}

certificate_chain::certificate_chain( MRef<certificate *> cert ){
	
	cert_list.push_back( cert );
	item = cert_list.begin();
}

certificate_chain::~certificate_chain(){
}

void certificate_chain::lock(){
        mLock.lock();
}

void certificate_chain::unlock(){
        mLock.unlock();
}

bool certificate_chain::is_empty(){
	return cert_list.empty();
}


void certificate_chain::add_certificate( MRef<certificate *> cert ){
	
	if( !cert_list.empty() ){
		MRef<certificate *> lastCert = *(--cert_list.end());

		if( lastCert->get_issuer() != cert->get_name() ){
			throw certificate_exception_chain(
			 	"The previous certificate in the chain is not"
				"issued by the given one" );
		}
	}
	
	cert_list.push_back( cert );
	item = cert_list.begin();
}

void certificate_chain::remove_last(){
	cert_list.erase( -- cert_list.end() );

	item = cert_list.begin();
}


void certificate_chain::init_index(){
	item = cert_list.begin();
}

int certificate_chain::control( MRef<ca_db *> cert_db){
	int result;
	X509_STORE_CTX cert_store_ctx;
	/* The first one, the one to verify */
	X509 * cert;
	/* Chain of certificates */
	STACK_OF(X509) * cert_stack;
	list< MRef<certificate *> >::iterator i = cert_list.begin();

	if( i == cert_list.end() ){
		cerr << "Certificate: Empty list of certificates"
			"to verify" << endl;
		return 0;
	}

	cert = (*i)->get_openssl_certificate();

	cert_stack = sk_X509_new_null();

	i++;

	for( ; i != cert_list.end(); i++ ){
		sk_X509_push( cert_stack, (*i)->get_openssl_certificate() );
	}

	X509_STORE_CTX_init( &cert_store_ctx, cert_db->get_db(), cert, cert_stack);
	if( X509_STORE_CTX_get_error( &cert_store_ctx) != 0 ){
		//fprintf(stderr, "Could not initialize X509_STORE_CTX");
		cerr << "Could not initialize X509_STORE_CTX" << endl;
		exit( 1 );
	}

	result = X509_verify_cert( &cert_store_ctx );

#ifdef DEBUG_OUTPUT
	if( result == 0 ){
		cerr << result << endl;
		cerr << X509_verify_cert_error_string( cert_store_ctx.error ) << endl;
	}
#endif
	return result;
}

MRef<certificate *> certificate_chain::get_next(){
	MRef<certificate *> ret;
	
	if( item == cert_list.end() ){
		item = cert_list.begin();
		return NULL;
	}

	ret = *item;
	item ++;
	return ret;
}

MRef<certificate *> certificate_chain::get_first(){
	if( cert_list.size() == 0 ){
		return NULL;
	}
	
	return *(cert_list.begin());
}

void certificate_chain::clear(){
	cert_list.clear();

}


