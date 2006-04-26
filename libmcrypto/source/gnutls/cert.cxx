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


#include<config.h>

#include<libmcrypto/gnutls/cert.h>
#include<gnutls/openssl.h>

#include<iostream>

using namespace std;

Certificate::Certificate():/*MObject("Certificate"),*/privateKey(NULL),cert(NULL){
        gnutls_global_init();
}

#if 0
Certificate::Certificate( X509 * openssl_cert ):/*MObject("Certificate"),*/privateKey(NULL){
	if( openssl_cert == NULL ){
		throw new CertificateException;
	}
	
	cert = openssl_cert;
}

#endif

Certificate::Certificate( const string certFilename ):/*MObject("Certificate"),*/privateKey(NULL){
        gnutls_global_init();
	openFromFile( certFilename );

}

Certificate::Certificate( const string certFilename, const string privateKeyFilename )/*:MObject("Certificate")*/{

        gnutls_global_init();
	openFromFile( certFilename );

	set_pk( privateKeyFilename );
	
}

Certificate::Certificate( unsigned char * derCert, int length ):privateKey(NULL){
        int ret;
        gnutls_datum certData;
	
	gnutls_global_init();
        
	ret = gnutls_x509_crt_init( &cert );

        if( ret != 0 ){
		throw new CertificateExceptionInit( 
			"Could not initialize the certificate structure" );
        }

	certData.data = derCert;
	certData.size = length;
        
	ret = gnutls_x509_crt_import( cert, &certData, GNUTLS_X509_FMT_DER );

        if( ret != 0 ){
		throw new CertificateException( 
			"Could not import the given certificate" );
        }

}
	
Certificate::~Certificate(){
	if( cert != NULL ){
		gnutls_x509_crt_deinit( cert );
	}
	
	cert = NULL;
	
	if( privateKey != NULL ){
		gnutls_x509_privkey_deinit( privateKey );
	}
	
	privateKey = NULL;

}

void Certificate::openFromFile( string fileName ){
	int fd;
        void * certBuf = NULL;
        size_t length;
        struct stat fileStat;
        gnutls_datum certData;

        fd = open( CERT_FILE, O_RDONLY );

        if( fd == -1 ){
		throw new CertificateExceptionFile( 
			"Could not open the given certificate file" );

        }

        ret = fstat( fd, &fileStat );

        if( ret == -1 ){
		throw new CertificateExceptionFile( 
			"Could not stat the given certificate file" );
        }

        length = fileStat.st_size;

        certBuf = mmap( 0, length, PROT_READ, MAP_SHARED, fd, 0 );

        if( certBuf == NULL ){
		throw new CertificateExceptionInit( 
			"Could not mmap the certificate file" );
        }

        certData.data = certBuf;
        certData.size = length;


        ret = gnutls_x509_crt_init( &cert );

        if( ret != 0 ){
		throw new CertificateExceptionInit( 
			"Could not initialize the certificate structure" );
        }

        ret = gnutls_x509_crt_import( cert, &certData, GNUTLS_X509_FMT_PEM );

        if( ret != 0 ){
		throw new CertificateExceptionFile( 
			"Could not import the given certificate" );
        }

        munmap( certBuf, length );
        close( fd );

	file = certFilename;
}

void Certificate::signData( unsigned char * data, int dataLength,
	    		   unsigned char * sign, int * signLength ){
	int err;
	
	if( privateKey == NULL ){
		sign = NULL;
		*sign_length = 0;
		throw new CertificateException(
			"A private key is needed to sign data" );
	}
	
	err = gnutls_x509_privkey_sign_data( 
			privateKey, 
			GNUTLS_MAC_SHA /*FIXME*/
			0,
			dataStruct,
			sign, signLength );

	if( err < 0 ){
		throw new CertificateException(
			"Signature of data failed" );
	}
}

int Certificate::verifSign( unsigned char * sign, int signLength,
			    unsigned char * data, int dataLength )
{
	int err;
	gnutls_datum dataStruct;
	gnutls_datum signStruct;

	dataStruct.data = data;
	dataStruct.size = dataLength;
	
	signStruct.sign = sign;
	signStruct.size = signLength;
	
	if( cert == NULL ){
		throw new CertificateException(
			"No certificate open while verifying a signature" );
	}
	
	err = gnutls_x509_crt_verify_data( cert, 0, dataStruct, signStruct );

	return err;
}

void Certificate::getDer( unsigned char * output, unsigned int * length ){
	
	int ret;

	ret = gnutls_x509_crt_export( *cert, GNUTLS_X509_FMT_DER, 
			output, length );
	if( ret == GNUTLS_E_SHORT_MEMORY_BUFFER ){
		throw new CertificateExceptionBuffer(
			"Given buffer is to short" );
	}

	if( ret < 0 ){
		throw new CertificateException(
			"An error occured while exporting the certificate" );
	}
}

string Certificate::getName(){
	int ret;
	char * buf;
	size_t * size;

	buf = (char *)malloc( 1024 );
	if( buf == NULL ){
		throw new CertificateExceptionInit(
			"Not enough memory" );
	}
	
	ret = gnutls_x509_crt_get_dn( cert, buf, size );

	/* This should not happen very often */
	if( ret == GNUTLS_E_SHORT_MEMORY_BUFFER ){
		free( buf );
		gnutls_x509_crt_get_dn( cert, NULL, size );
		buf = (char *) malloc( size );
		if( buf == NULL ){
			throw new CertificateExceptionInit(
				"Not enough memory" );
		}
		ret = gnutls_x509_crt_get_dn( cert, buf, size );
	}

	if( ret < 0 ){
		throw new CertificateException(
			"An error occured in getName()" );
	}

	string output( buf, length );

	free( buf );
	return output;
}

string Certificate::getCn(){
	int ret;
	char * buf;
	size_t * size;

	buf = (char *)malloc( 1024 );
	if( buf == NULL ){
		throw new CertificateExceptionInit(
			"Not enough memory" );
	}
	
	ret = gnutls_x509_crt_get_dn_by_oid( cert, 
			GNUTLS_OID_X520_COMMON_NAME, 0, buf, size );

	/* This should not happen very often */
	if( ret == GNUTLS_E_SHORT_MEMORY_BUFFER ){
		free( buf );
		gnutls_x509_crt_get_dn_by_oid( cert, 
			GNUTLS_OID_X520_COMMON_NAME, 0, NULL, size );
		buf = (char *) malloc( size );
		if( buf == NULL ){
			throw new CertificateExceptionInit(
				"Not enough memory" );
		}
		ret = gnutls_x509_crt_get_dn_by_oid( cert, 
			GNUTLS_OID_X520_COMMON_NAME, 0, buf, size );
	}

	if( ret < 0 ){
		throw new CertificateException(
			"An error occured in getName()" );
	}

	string output( buf, length );

	free( buf );
	return output;
	
}

string Certificate::getIssuer(){
	int ret;
	char * buf;
	size_t * size;

	buf = (char *)malloc( 1024 );
	if( buf == NULL ){
		throw new CertificateExceptionInit(
			"Not enough memory" );
	}
	
	ret = gnutls_x509_crt_get_issuer( cert, buf, size );

	/* This should not happen very often */
	if( ret == GNUTLS_E_SHORT_MEMORY_BUFFER ){
		free( buf );
		gnutls_x509_crt_get_issuer( cert, NULL, size );
		buf = (char *) malloc( size );
		if( buf == NULL ){
			throw new CertificateExceptionInit(
				"Not enough memory" );
		}
		ret = gnutls_x509_crt_get_issuer( cert, buf, size );
	}

	if( ret < 0 ){
		throw new CertificateException(
			"An error occured in getName()" );
	}

	string output( buf, length );

	free( buf );
	return output;
}

string Certificate::getIssuerCn(){
	int ret;
	char * buf;
	size_t * size;

	buf = (char *)malloc( 1024 );
	if( buf == NULL ){
		throw new CertificateExceptionInit(
			"Not enough memory" );
	}
	
	ret = gnutls_x509_crt_get_issuer_dn_by_oid( cert, 
			GNUTLS_OID_X520_COMMON_NAME, 0, buf, size );

	/* This should not happen very often */
	if( ret == GNUTLS_E_SHORT_MEMORY_BUFFER ){
		free( buf );
		gnutls_x509_crt_get_issuer_dn_by_oid( cert, 
			GNUTLS_OID_X520_COMMON_NAME, 0, NULL, size );
		buf = (char *) malloc( size );
		if( buf == NULL ){
			throw new CertificateExceptionInit(
				"Not enough memory" );
		}
		ret = gnutls_x509_crt_get_issuer_dn_by_oid( cert, 
			GNUTLS_OID_X520_COMMON_NAME, 0, buf, size );
	}

	if( ret < 0 ){
		throw new CertificateException(
			"An error occured in getName()" );
	}

	string output( buf, length );

	free( buf );
	return output;
}

string Certificate::getFile(){
	return file;
}

string Certificate::getPkFile(){
	return pkFile;
}

void Certificate::setPk( string file ){
	int fd;
        void * pkBuf = NULL;
        size_t length;
        struct stat fileStat;
        gnutls_datum pkData;
	byte_t publicKeyId[20];
	byte_t privateKeyId[20];
	size_t idLength;

        fd = open( file.c_str(), O_RDONLY );

        if( fd == -1 ){
		throw new CertificateExceptionFile( 
			"Could not open the given private key file" );

        }

        ret = fstat( fd, &fileStat );

        if( ret == -1 ){
		throw new CertificateExceptionFile( 
			"Could not stat the given private key file" );
        }

        length = fileStat.st_size;

        pkBuf = mmap( 0, length, PROT_READ, MAP_SHARED, fd, 0 );

        if( pkBuf == NULL ){
		throw new CertificateExceptionInit( 
			"Could not mmap the certificate file" );
        }

        pkData.data = pkBuf;
        pkData.size = length;


        ret = gnutls_x509_privkey_init( &privateKey );

        if( ret != 0 ){
		throw new CertificateExceptionInit( 
			"Could not initialize the private key structure" );
        }

        ret = gnutls_x509_privkey_import( privateKey, &pkData, 
			GNUTLS_X509_FMT_PEM );

        if( ret != 0 ){
		throw new CertificateExceptionFile( 
			"Could not import the given private key" );
        }

        munmap( pkBuf, length );
        close( fd );

	/* Check that the private key matches the Certificate */

	idLength = 20;
	ret = gnutls_x509_crt_get_key_id( cert, 0, publicKeyId, &idLength );

	if( ret < 0 ){
		throw new CertificateException(
			"An error occured when computing the key id" );
	}
	
	ret = gnutls_x509_privkey_get_key_id( cert, 0, privateKeyId, &idLength );
	
	if( ret < 0 ){
		throw new CertificateException(
			"An error occured when computing the key id" );
	}

	for( i = 0; i < idLength; i++ ){
		if( privateKeyId[i] != publicKeyId[i] ){
			throw new CertificateExceptionPkey(
				"The private key " + file + 
				" does not match the certificate " + this->file
				);
		}
	}

	pkFile = file;

}


int Certificate::control( CaDb * certDb ){
	int result;
	X509_STORE_CTX cert_store_ctx;

	if( X509_STORE_CTX_init( &cert_store_ctx, certDb->get_db(), cert ,NULL ) < 0 ){
		fprintf(stderr, "Could not initialize X509_STORE_CTX");
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
	

CaDb::CaDb(){


	caList = NULL;

	items_index = items.begin();
}

CaDb::~CaDb(){
	if( caList != NULL ){
		free( caList );
	}
}

void CaDb::lock(){
        mLock.lock();
}

void CaDb::unlock(){
        mLock.unlock();
}

gnutls_x509_crt_t * CaDb::getDb(){
	return caList;
}

#if 0
void CaDb::addDirectory( string dir ){
	X509_LOOKUP * lookup = NULL;
	CaDbItem * item = new CaDbItem();
	
	lookup = X509_STORE_add_lookup( 
			certDb, X509_LOOKUP_hash_dir() );
	if( lookup == NULL )
		throw new CertificateExceptionInit(
			string("Could not create a directory lookup") );
	
	if( !X509_LOOKUP_add_dir( lookup, dir.c_str(), X509_FILETYPE_PEM ) )
		throw new CertificateExceptionFile(
			"Could not open the directory "+dir );

	item->item = dir;
	item->type = CERT_DB_ITEM_TYPE_DIR;
	
	items.push_back( item );
	items_index = items.begin();
}
#endif

void CaDb::addFile( string file ){

	Certificate cert( file );


	
	
	item->item = file;
	item->type = CERT_DB_ITEM_TYPE_FILE;
	
	items.push_back( item );
	items_index = items.begin();
}

void CaDb::add_Certificate( Certificate * cert ){
	CaDbItem * item = new CaDbItem();
	X509_STORE_add_cert( certDb, cert->get_openssl_Certificate() );
	
	item->item = "";
	item->type = CERT_DB_ITEM_TYPE_OTHER;
	
	items.push_back( item );
	items_index = items.begin();

}

void CaDb::remove( CaDbItem * removedItem ){
	initIndex();

	while( items_index != items.end() ){
		if( *(*items_index) == *removedItem ){
			items.erase( items_index );
			initIndex();
			return;
		}
		items_index ++;
	}
	initIndex();
}

list<CaDbItem *> &CaDb::get_items(){
	return items;
}

void CaDb::initIndex(){
	items_index = items.begin();
}

CaDbItem * CaDb::get_next(){
	CaDbItem * tmp;
	
	if( items_index == items.end() ){
		items_index = items.begin();
		return NULL;
	}

	tmp = *items_index;
	items_index ++;
	return tmp;
}

CertificateChain::CertificateChain()/*:MObject("CertificateChain")*/{
//	pthread_mutex_init( &mLock, NULL );
	item = certList.begin();

}

CertificateChain::CertificateChain( MRef<Certificate *> cert )/*: MObject("CertificateChain")*/{
	
	certList.push_back( cert );
	item = certList.begin();
}

CertificateChain::~CertificateChain(){
}

void CertificateChain::lock(){
        mLock.lock();
}

void CertificateChain::unlock(){
        mLock.unlock();
}

bool CertificateChain::isEmpty(){
	return certList.empty();
}


void CertificateChain::addCertificate( MRef<Certificate *> cert ){
	
	if( !certList.empty() ){
		MRef<Certificate *> lastCert = *(--certList.end());

		if( lastCert->get_issuer() != cert->get_name() ){
			throw new CertificateExceptionChain(
			 	"The previous Certificate in the chain is not"
				"issued by the given one" );
		}
	}
	
	certList.push_back( cert );
	item = certList.begin();
}

void CertificateChain::removeLast(){
	certList.erase( -- certList.end() );

	item = certList.begin();
}


void CertificateChain::initIndex(){
	item = certList.begin();
}

int CertificateChain::control( MRef<CaDb *> certDb){
	int result;
	X509_STORE_CTX cert_store_ctx;
	/* The first one, the one to verify */
	X509 * cert;
	/* Chain of Certificates */
	STACK_OF(X509) * certStack;
	list< MRef<Certificate *> >::iterator i = certList.begin();

	if( i == certList.end() ){
		cerr << "Certificate: Empty list of Certificates"
			"to verify" << endl;
		return 0;
	}

	cert = (*i)->get_openssl_Certificate();

	certStack = sk_X509_new_null();

	i++;

	for( ; i != certList.end(); i++ ){
		sk_X509_push( certStack, (*i)->get_openssl_Certificate() );
	}

	if( X509_STORE_CTX_init( &cert_store_ctx, certDb->getDb(), cert, certStack ) < 0 ){
		fprintf(stderr, "Could not initialize X509_STORE_CTX");
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

MRef<Certificate *> CertificateChain::get_next(){
	MRef<Certificate *> ret;
	
	if( item == certList.end() ){
		item = certList.begin();
		return NULL;
	}

	ret = *item;
	item ++;
	return ret;
}

MRef<Certificate *> CertificateChain::get_first(){
	if( certList.size() == 0 ){
		return NULL;
	}
	
	return *(certList.begin());
}

void CertificateChain::clear(){
	certList.clear();

}
