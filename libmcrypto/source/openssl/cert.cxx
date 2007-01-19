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

#include<config.h>

#include<libmcrypto/openssl/cert.h>

extern "C"{
	#include<openssl/rsa.h>
	#include<openssl/evp.h>
	#include<openssl/objects.h>
	#include<openssl/x509.h>
	#include<openssl/x509v3.h>
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

static string NAME_to_string( X509_NAME * x509Name ){
	char *name = X509_NAME_oneline( x509Name, 0, 0 );
	string ret( name );

	CRYPTO_free( name );
	name = NULL;
	return ret;
}


//
// Factory methods
// 

ca_db *ca_db::create(){
	return new ossl_ca_db();
}

priv_key* priv_key::load( const std::string private_key_filename ){
	return new ossl_priv_key( private_key_filename );
}

priv_key* priv_key::load( char *derEncPk, int length,
			  std::string password, std::string path ){
	return new ossl_priv_key( derEncPk, length, password, path );
}

certificate* certificate::load( const std::string cert_filename )
{
	return new ossl_certificate( cert_filename );
}

certificate* certificate::load( const std::string cert_filename,
				const std::string private_key_filename ){
	MRef<priv_key*> priv_key = new ossl_priv_key( private_key_filename );
	certificate* cert = new ossl_certificate( cert_filename );

	cert->set_pk( priv_key );
	return cert;
}

certificate* certificate::load( unsigned char * der_cert,
				int length ){
	return new ossl_certificate( der_cert, length );
}

certificate* certificate::load( unsigned char * certData,
				int length,
				std::string path ){
	return new ossl_certificate( certData, length, path );
}

certificate_chain* certificate_chain::create(){
	return new ossl_certificate_chain();
}

//
// ossl_priv_key
// 
ossl_priv_key::~ossl_priv_key(){
	if( private_key )
		EVP_PKEY_free( private_key );
	private_key = NULL;
}

const string &ossl_priv_key::get_file() const{
	return pk_file;
}


//
// ossl_certificate
// 

ossl_certificate::ossl_certificate():cert(NULL){

}

ossl_certificate::ossl_certificate( X509 * ossl_cert ){
	if( ossl_cert == NULL ){
		throw certificate_exception("X509 certificate is NULL");
	}
	
	cert = ossl_cert;
}

ossl_certificate::ossl_certificate( const string &cert_filename ){
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

ossl_certificate::ossl_certificate( unsigned char * certData, int length, string path )
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


ossl_certificate::ossl_certificate( unsigned char * der_cert, int length ){
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
	
ossl_certificate::~ossl_certificate(){
	if( cert )
		X509_free( cert );
	cert = NULL;
	
}

int ossl_certificate::envelope_data(unsigned char * data, int size, unsigned char *retdata, int *retsize,
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

int ossl_priv_key::denvelope_data(unsigned char * data, int size, unsigned char *retdata, int *retsize,
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

int ossl_priv_key::sign_data( unsigned char * data, int data_length,
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

int ossl_certificate::verif_sign( unsigned char * data, int data_length,
				  unsigned char * sign, int sign_length ){
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
	
bool ossl_priv_key::private_decrypt( const unsigned char *data, int size,
				    unsigned char *retdata, int *retsize ){
	//adding PKE payload
	RSA* rsa = EVP_PKEY_get1_RSA( private_key );

// 	if( size >= RSA_size( rsa ) - 11 ){
// 		return false;
// 	}

	if( RSA_size( rsa ) > *retsize ){
		*retsize = RSA_size( rsa );
		return false;
	}

	int ret = RSA_private_decrypt( size, data, retdata, rsa, RSA_PKCS1_PADDING );

	if( ret < 0 ){
		unsigned long err = ERR_get_error();
		char buf[128]="";
		ERR_error_string_n( err, buf, sizeof(buf) );

		cerr << "RSA_private_decrypt: " << buf << endl;
		//cerr << binToHex( data, size ) << endl;

		return false;
	}

	*retsize = ret;
	return true;
}

bool ossl_certificate::public_encrypt( const unsigned char *data, int size,
				      unsigned char *retdata, int *retsize ){
	//adding PKE payload
	EVP_PKEY *public_key = X509_get_pubkey( cert );
	RSA* rsa = EVP_PKEY_get1_RSA( public_key );

	if( size >= RSA_size( rsa ) - 11 ){
		cerr << "RSA_public_encrypt: data size to large: " << size << ">=" << RSA_size(rsa)-11 << endl;
		return false;
	}

	if( RSA_size( rsa ) > *retsize ){
		cerr << "RSA_public_encrypt: buffer to small ("
		     <<	*retsize << "<" << RSA_size(rsa) << ")" << endl;
		*retsize = RSA_size( rsa );
		return false;
	}

	int ret = RSA_public_encrypt( size, data, retdata, rsa, RSA_PKCS1_PADDING );

	if( ret < 0 ){
		unsigned long err = ERR_get_error();
		char buf[128]="";
		ERR_error_string_n( err, buf, sizeof(buf) );

		cerr << "RSA_public_encrypt: " << buf << endl;
		//cerr << binToHex( data, size ) << endl;

		return false;
	}

	*retsize = ret;
	return true;
}

int ossl_certificate::get_der_length(){
	return i2d_X509( cert, NULL );
}

void ossl_certificate::get_der( unsigned char * output, unsigned int * length ){
	if( *length < get_der_length() ){
 		throw certificate_exception(
			"Given buffer is to short" );
	}
	
	int temp = i2d_X509( cert, &output);

	// don't want it to be incremented:
	output -= temp;
}

string ossl_certificate::get_name(){
	return NAME_to_string( X509_get_subject_name( cert ) );
}

string ossl_certificate::get_cn(){
	string name = get_name();
	size_t pos, pos2;

	pos = name.find( "/CN=" );

	if( pos == string::npos ){
		return "No common name";
	}

	pos2 = name.find( "/", pos + 1 );

	return name.substr( pos + 4, pos2 - pos - 4 );
}

vector<string> ossl_certificate::get_alt_name( SubjectAltName type ){
	vector<string> output;

	int genType = -1;
	switch( type ){
		case SAN_DNSNAME:
			genType = GEN_DNS;
			break;
		case SAN_RFC822NAME:
			genType = GEN_EMAIL;
			break;
		case SAN_URI:
			genType = GEN_URI;
			break;
		case SAN_IPADDRESS:
			// Unsupported
// 			genType = GEN_IPADD;
// 			break;
		default:
			return output;
	}

	int pos = -1;
 	pos = X509_get_ext_by_NID(cert, NID_subject_alt_name, -1);
	if( pos == -1 ){
		return output;
	}

	X509_EXTENSION * ext = X509_get_ext( cert, pos );
	if( !ext ){
		return output;
	}

	STACK_OF(GENERAL_NAMES) * altNames = NULL;
	altNames = (STACK_OF(GENERAL_NAMES)*) X509V3_EXT_d2i( ext );
	if( !altNames ){
		return output;
	}

	int altNamesCount = sk_GENERAL_NAME_num( altNames );
	for( int i=0; i < altNamesCount; i++ ){
		GENERAL_NAME * name = sk_GENERAL_NAME_value( altNames, i );

		if( name->type == genType ){
			ASN1_IA5STRING * ia5 = NULL;

			ia5 = name->d.ia5;
			
			size_t len = ASN1_STRING_length( ia5 );
			char buf[ len + 1 ];
			strncpy( buf, (const char*)ASN1_STRING_data( ia5 ),
				 len );

			string str( buf, len );

			output.push_back( str );
		}

	}

	GENERAL_NAMES_free( altNames );
	altNames = NULL;

	return output;
}

string ossl_certificate::get_issuer(){
	return NAME_to_string( X509_get_issuer_name( cert ) );
}

string ossl_certificate::get_issuer_cn(){
	string name = get_issuer();
	size_t pos, pos2;

	pos = name.find( "/CN=" );

	if( pos == string::npos ){
		return "No common name";
	}

	pos2 = name.find( "/", pos + 1 );

	return name.substr( pos + 4, pos2 - pos - 4 );
}


ossl_priv_key::ossl_priv_key( const string &file ){
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

	pk_file = file;
}

bool ossl_certificate::check_pk( MRef<priv_key*> pk ){
	MRef<ossl_priv_key*> ssl_pk =
		dynamic_cast<ossl_priv_key*>( *pk );

	/* Check that the private key matches the certificate */

	if( X509_check_private_key( cert, ssl_pk->get_openssl_private_key() ) != 1 ){
		return false;
	}

	return true;
}


ossl_priv_key::ossl_priv_key( char *derEncPk, int length,
			      const std::string &password,
			      const std::string &path )
{
   BIO *mem;  
   mem = BIO_new_mem_buf((void *)derEncPk, length);
   
   if(mem == NULL )
     {
	cerr << "Couldn't initiate bio buffer" << endl;
	throw certificate_exception_pkey("Couldn't initiate bio buffer" );
     }
      
   private_key = PEM_read_bio_PrivateKey(mem, NULL, 0, (void*)password.c_str());
 
   if(private_key == NULL )
     {
	cerr << "Invalid private key data or password" << endl;
	throw certificate_exception_pkey("The private key is invalid or wrong password was used" );
     }
   
   pk_file=path;
}


int ossl_certificate::control( ca_db * cert_db ){
	int result;
	X509_STORE_CTX cert_store_ctx;
	ossl_ca_db *ssl_db = (ossl_ca_db*)cert_db;

	X509_STORE_CTX_init( &cert_store_ctx, ssl_db->get_db(), cert ,NULL );
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

	X509_STORE_CTX_cleanup( &cert_store_ctx );

	return result;
}
	

// 
// ossl_ca_db
// 

ossl_ca_db::ossl_ca_db(){
	cert_db = X509_STORE_new();

	if( cert_db == NULL ){
		throw certificate_exception_init(
				"Could not create the certificate db" );
	}
}

ossl_ca_db::~ossl_ca_db(){
	X509_STORE_free( cert_db );
}

X509_STORE * ossl_ca_db::get_db(){
	return cert_db;
}

void ossl_ca_db::add_directory( string dir ){
	X509_LOOKUP * lookup = NULL;
	
	lookup = X509_STORE_add_lookup( 
			cert_db, X509_LOOKUP_hash_dir() );
	if( lookup == NULL )
		throw certificate_exception_init(
			"Could not create a directory lookup");
	
	if( !X509_LOOKUP_add_dir( lookup, dir.c_str(), X509_FILETYPE_PEM ) )
		throw certificate_exception_file(
			(string("Could not open the directory ")+dir).c_str() );

	ca_db::add_directory( dir );
}

void ossl_ca_db::add_file( string file ){
	X509_LOOKUP * lookup = NULL;
	
	lookup = X509_STORE_add_lookup( 
			cert_db, X509_LOOKUP_file() );
	if( lookup == NULL )
		throw certificate_exception_init(
			"Could not create a file lookup" );
	
	if( !X509_LOOKUP_load_file( lookup, file.c_str(), X509_FILETYPE_PEM ) )
		throw certificate_exception_file(
			("Could not open the file "+file).c_str() );

	ca_db::add_file( file );
}

void ossl_ca_db::add_certificate( certificate * cert ){
	ossl_certificate *ssl_cert = (ossl_certificate *)cert;
	X509_STORE_add_cert( cert_db, ssl_cert->get_openssl_certificate() );

	ca_db::add_certificate( cert );
}


//
// ossl_certificate_chain
// 

ossl_certificate_chain::ossl_certificate_chain(){
}

ossl_certificate_chain::ossl_certificate_chain( MRef<certificate *> cert ): certificate_chain( cert ){
}

ossl_certificate_chain::~ossl_certificate_chain(){
}

int ossl_certificate_chain::control( MRef<ca_db *> cert_db){
	MRef<ossl_ca_db*>ssl_db = (ossl_ca_db*)*cert_db;
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

	MRef<ossl_certificate*>ssl_cert = (ossl_certificate *)**i;

	cert = ssl_cert->get_openssl_certificate();

	cert_stack = sk_X509_new_null();

	i++;

	for( ; i != cert_list.end(); i++ ){
		sk_X509_push( cert_stack, ssl_cert->get_openssl_certificate() );
	}

	X509_STORE_CTX_init( &cert_store_ctx, ssl_db->get_db(), cert, cert_stack);
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

	X509_STORE_CTX_cleanup( &cert_store_ctx );
	sk_X509_free( cert_stack );
	cert_stack = NULL;

	return result;
}
