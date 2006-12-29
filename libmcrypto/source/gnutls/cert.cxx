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


#include<config.h>

#include<libmcrypto/gnutls/cert.h>
#include<gnutls/x509.h>

#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>

#include<iostream>

using namespace std;


#define UNIMPLEMENTED \
	string msg = string( __FUNCTION__ ) + " unimplemented"; \
	throw Exception(msg.c_str());


gtls_certificate::gtls_certificate():cert(NULL){
        gnutls_global_init();
}

//
// Factory methods
// 

ca_db *ca_db::create(){
	return new gtls_ca_db();
}

priv_key* priv_key::load( const std::string private_key_filename ){
	return new gtls_priv_key( private_key_filename );
}

priv_key* priv_key::load( char *derEncPk, int length,
			  std::string password,
			  std::string path ){
	return new gtls_priv_key( derEncPk, length, password, path );
}


// Read PEM-encoded certificate from a file
certificate* certificate::load( const std::string cert_filename )
{
	return new gtls_certificate( cert_filename );
}

// Read PEM-encoded certificate private key from a file
certificate* certificate::load( const std::string cert_filename,
				const std::string private_key_filename ){
	MRef<priv_key*> priv_key = new gtls_priv_key( private_key_filename );
	certificate* cert = new gtls_certificate( cert_filename );

	cert->set_pk( priv_key );
	return cert;
}

// Import DER-encoded certificate from memory
certificate* certificate::load( unsigned char * der_cert,
				int length ){
	return new gtls_certificate( der_cert, length );
}

#if 0
// TODO
certificate* certificate::load( unsigned char * certData,
				int length,
				std::string path ){
	return new gtls_certificate( certData, length, path );
}
#endif

certificate_chain* certificate_chain::create(){
	return new gtls_certificate_chain();
}

gtls_priv_key::~gtls_priv_key(){
	if( privateKey != NULL ){
		gnutls_x509_privkey_deinit( privateKey );
	}
	
	privateKey = NULL;
}

const string &gtls_priv_key::get_file() const{
	return pk_file;
}


// Read PEM-encoded certificate from a file
gtls_certificate::gtls_certificate( const string certFilename ){
        gnutls_global_init();
	openFromFile( certFilename );
}

// Import DER-encoded certificate from memory
gtls_certificate::gtls_certificate( unsigned char * derCert, int length ){
        int ret;
        gnutls_datum certData;
	
	gnutls_global_init();
        
	ret = gnutls_x509_crt_init( (gnutls_x509_crt_t*)&cert );

        if( ret != 0 ){
		throw certificate_exception_init( 
			"Could not initialize the certificate structure" );
        }

	certData.data = derCert;
	certData.size = length;
        
	ret = gnutls_x509_crt_import( cert, &certData, GNUTLS_X509_FMT_DER );

        if( ret != 0 ){
		throw certificate_exception( 
			"Could not import the given certificate" );
        }

}
	
gtls_certificate::~gtls_certificate(){
	if( cert != NULL ){
		gnutls_x509_crt_deinit( cert );
	}
	
	cert = NULL;
	
}

// Read PEM-encoded certificate from a file
void gtls_certificate::openFromFile( string fileName ){
	int fd;
        void * certBuf = NULL;
        size_t length;
        struct stat fileStat;
        gnutls_datum certData;

        fd = open( fileName.c_str(), O_RDONLY );

        if( fd == -1 ){
		throw certificate_exception_file( 
			"Could not open the given certificate file" );

        }

        int ret = fstat( fd, &fileStat );

        if( ret == -1 ){
		throw certificate_exception_file( 
			"Could not stat the given certificate file" );
        }

        length = fileStat.st_size;

        certBuf = mmap( 0, length, PROT_READ, MAP_SHARED, fd, 0 );

        if( certBuf == NULL ){
		throw certificate_exception_init( 
			"Could not mmap the certificate file" );
        }

        certData.data = (unsigned char*)certBuf;
        certData.size = length;


        ret = gnutls_x509_crt_init( (gnutls_x509_crt_t*)&cert );

        if( ret != 0 ){
		throw certificate_exception_init( 
			"Could not initialize the certificate structure" );
        }

        ret = gnutls_x509_crt_import( cert, &certData, GNUTLS_X509_FMT_PEM );

        if( ret != 0 ){
		throw certificate_exception_file( 
			"Could not import the given certificate" );
        }

        munmap( certBuf, length );
        close( fd );

	file = fileName;
}

int gtls_priv_key::sign_data( unsigned char * data, int dataLength,
			    unsigned char * sign, int * sign_length ){
	int err;
	size_t length = *sign_length;
	
	if( privateKey == NULL ){
		sign = NULL;
		*sign_length = 0;
		throw certificate_exception(
			"A private key is needed to sign data" );
	}

	gnutls_datum_t dataStruct;

	dataStruct.data = data;
	dataStruct.size = dataLength;
	
	err = gnutls_x509_privkey_sign_data( 
			privateKey, 
			GNUTLS_DIG_SHA1,
			0,
			&dataStruct,
			sign, &length );

	if( err < 0 ){
		throw certificate_exception(
			"Signature of data failed" );
	}

	*sign_length = length;

	return 0;
}

int gtls_certificate::verif_sign( unsigned char * data, int data_length,
				  unsigned char * sign, int sign_length ){
{
	int err;
	gnutls_datum dataStruct;
	gnutls_datum signStruct;

	dataStruct.data = data;
	dataStruct.size = dataLength;
	
	signStruct.data = sign;
	signStruct.size = signLength;
	
	if( cert == NULL ){
		throw certificate_exception(
			"No certificate open while verifying a signature" );
	}
	
	err = gnutls_x509_crt_verify_data( cert, 0, &dataStruct, &signStruct );

	return err;
}

bool gtls_certificate::public_encrypt(unsigned char *data, int size,
				      unsigned char *retdata, int *retsize){
	UNIMPLEMENTED;
}

int gtls_certificate::get_der_length(){
	size_t size = 0;

	int ret = gnutls_x509_crt_export( cert, GNUTLS_X509_FMT_DER, 
					  NULL, &size );

	if( ret == GNUTLS_E_SHORT_MEMORY_BUFFER )
		return size;
	else
		return -1;
}

void gtls_certificate::get_der( unsigned char * output, unsigned int * length ){
	
	int ret;

	ret = gnutls_x509_crt_export( cert, GNUTLS_X509_FMT_DER, 
			output, length );
	if( ret == GNUTLS_E_SHORT_MEMORY_BUFFER ){
		throw certificate_exception(
			"Given buffer is to short" );
	}

	if( ret < 0 ){
		throw certificate_exception(
			"An error occured while exporting the certificate" );
	}
}

string gtls_certificate::get_name(){
	int ret;
	char * buf;
	size_t size = 1024;

	buf = (char *)malloc( size );
	if( buf == NULL ){
		throw certificate_exception_init(
			"Not enough memory" );
	}
	
	ret = gnutls_x509_crt_get_dn( cert, buf, &size );

	/* This should not happen very often */
	if( ret == GNUTLS_E_SHORT_MEMORY_BUFFER ){
		free( buf );
		size = 0;
		gnutls_x509_crt_get_dn( cert, NULL, &size );
		buf = (char *) malloc( size );
		if( buf == NULL ){
			throw certificate_exception_init(
				"Not enough memory" );
		}
		ret = gnutls_x509_crt_get_dn( cert, buf, &size );
	}

	if( ret < 0 ){
		throw certificate_exception(
			"An error occured in getName()" );
	}

	string output( buf, size );

	free( buf );
	return output;
}

string gtls_certificate::get_cn(){
	int ret;
	char * buf;
	size_t size = 1024;

	buf = (char *)malloc( size );
	if( buf == NULL ){
		throw certificate_exception_init(
			"Not enough memory" );
	}
	
	ret = gnutls_x509_crt_get_dn_by_oid( cert, 
					     GNUTLS_OID_X520_COMMON_NAME,
					     0, 0, buf, &size );

	/* This should not happen very often */
	if( ret == GNUTLS_E_SHORT_MEMORY_BUFFER ){
		free( buf );
		size = 0;
		gnutls_x509_crt_get_dn_by_oid( cert, 
					       GNUTLS_OID_X520_COMMON_NAME,
					       0, 0, NULL, &size );
		buf = (char *) malloc( size );
		if( buf == NULL ){
			throw certificate_exception_init(
				"Not enough memory" );
		}
		ret = gnutls_x509_crt_get_dn_by_oid( cert, 
						     GNUTLS_OID_X520_COMMON_NAME,
						     0, 0, buf, &size );
	}

	if( ret < 0 ){
		throw certificate_exception(
			"An error occured in getName()" );
	}

	string output( buf, size );

	free( buf );
	return output;
	
}

string gtls_certificate::get_issuer(){
	int ret;
	char * buf;
	size_t size = 1024;

	buf = (char *)malloc( size );
	if( buf == NULL ){
		throw certificate_exception_init(
			"Not enough memory" );
	}
	
	ret = gnutls_x509_crt_get_issuer_dn( cert, buf, &size );

	/* This should not happen very often */
	if( ret == GNUTLS_E_SHORT_MEMORY_BUFFER ){
		free( buf );
		size = 0;
		gnutls_x509_crt_get_issuer_dn( cert, NULL, &size );
		buf = (char *) malloc( size );
		if( buf == NULL ){
			throw certificate_exception_init(
				"Not enough memory" );
		}
		ret = gnutls_x509_crt_get_issuer_dn( cert, buf, &size );
	}

	if( ret < 0 ){
		throw certificate_exception(
			"An error occured in getName()" );
	}

	string output( buf, size );

	free( buf );
	return output;
}

string gtls_certificate::get_issuer_cn(){
	int ret;
	char * buf;
	size_t size = 1024;

	buf = (char *)malloc( size );
	if( buf == NULL ){
		throw certificate_exception_init(
			"Not enough memory" );
	}
	
	ret = gnutls_x509_crt_get_issuer_dn_by_oid( cert, 
						    GNUTLS_OID_X520_COMMON_NAME,
						    0, 0, buf, &size );

	/* This should not happen very often */
	if( ret == GNUTLS_E_SHORT_MEMORY_BUFFER ){
		free( buf );
		size = 0;
		gnutls_x509_crt_get_issuer_dn_by_oid( cert, 
						      GNUTLS_OID_X520_COMMON_NAME,
						      0, 0, NULL, &size );
		buf = (char *) malloc( size );
		if( buf == NULL ){
			throw certificate_exception_init(
				"Not enough memory" );
		}
		ret = gnutls_x509_crt_get_issuer_dn_by_oid( cert, 
							    GNUTLS_OID_X520_COMMON_NAME,
							    0, 0, buf, &size );
	}

	if( ret < 0 ){
		throw certificate_exception(
			"An error occured in getName()" );
	}

	string output( buf, size );

	free( buf );
	return output;
}

// Read PEM-encoded private key from a file
gtls_priv_key::gtls_priv_key( const string &file ){
	int fd;
        void * pkBuf = NULL;
        size_t length;
        struct stat fileStat;
        gnutls_datum pkData;

        fd = open( file.c_str(), O_RDONLY );

        if( fd == -1 ){
		throw certificate_exception_file( 
			"Could not open the given private key file" );

        }

        int ret = fstat( fd, &fileStat );

        if( ret == -1 ){
		throw certificate_exception_file( 
			"Could not stat the given private key file" );
        }

        length = fileStat.st_size;

        pkBuf = mmap( 0, length, PROT_READ, MAP_SHARED, fd, 0 );

        if( pkBuf == NULL ){
		throw certificate_exception_init( 
			"Could not mmap the certificate file" );
        }

        pkData.data = (unsigned char*)pkBuf;
        pkData.size = length;


        ret = gnutls_x509_privkey_init( (gnutls_x509_privkey_t*)&privateKey );

        if( ret != 0 ){
		throw certificate_exception_init( 
			"Could not initialize the private key structure" );
        }

        ret = gnutls_x509_privkey_import( privateKey, &pkData, 
			GNUTLS_X509_FMT_PEM );

        if( ret != 0 ){
		throw certificate_exception_file( 
			"Could not import the given private key" );
        }

        munmap( pkBuf, length );
        close( fd );

	pk_file = file;
}

// Import DER-encoded private key from memory
gtls_priv_key::gtls_priv_key(char * pkInput, int length,
			     const string &password,
			     const string &path )
{
   /*Not checked if working correctly*/
   
   gnutls_datum pkData;
   
   int ret = gnutls_x509_privkey_init( &privateKey );
   
   if( ret != 0 )
     {	
	throw certificate_exception_init(
					   "Could not initialize the private key structure" );
     }
   
   pkData.data = (unsigned char*)pkInput;
   pkData.size = length;
   
   
   ret = gnutls_x509_privkey_import_pkcs8 (privateKey, &pkData, GNUTLS_X509_FMT_DER, password.c_str(), 0);
   
   if( ret != 0 )
     {
	throw certificate_exception_file("Could not import the given private key" );
     }
   
   pk_file = path;
}

bool gtls_certificate::check_pk( MRef<priv_key*> pk ){
	MRef<gtls_priv_key*> gtls_pk =
		dynamic_cast<gtls_priv_key*>( *pk );

	if( !gtls_pk ){
		return false;
	}

	gnutls_x509_privkey_t privateKey = gtls_pk->get_private_key();
	byte_t publicKeyId[20];
	byte_t privateKeyId[20];
	size_t idLength;

	/* Check that the private key matches the certificate */
	idLength = 20;
	int ret = gnutls_x509_crt_get_key_id( cert, 0, publicKeyId, &idLength );
   
	if( ret < 0 ){
		throw certificate_exception("An error occured when computing the key id" );
	}
   
	ret = gnutls_x509_privkey_get_key_id( privateKey, 0, privateKeyId, &idLength );
   
	if( ret < 0 ){	
		throw certificate_exception("An error occured when computing the key id" );
	}
	for( unsigned int i = 0; i < idLength; i++ ){
		if( privateKeyId[i] != publicKeyId[i] ){
			return false;
		}
	}

	return true;
}


// TODO convert to gnutls
int gtls_certificate::control( ca_db * certDb ){
	UNIMPLEMENTED;
}

int gtls_certificate::envelope_data( unsigned char * data, int size, unsigned char *retdata, int *retsize,
				     unsigned char *enckey, int *enckeylgth, unsigned char **iv){
	UNIMPLEMENTED;
}

int gtls_priv_key::denvelope_data(unsigned char * data, int size, unsigned char *retdata, int *retsize,
				     unsigned char *enckey, int enckeylgth, unsigned char *iv){
	UNIMPLEMENTED;
}

bool gtls_priv_key::private_decrypt(unsigned char *data, int size,
				    unsigned char *retdata, int *retsize){
	UNIMPLEMENTED;
}


// 
// End of gtls_certificate
// 

gtls_ca_db_item::gtls_ca_db_item(): certs(NULL), num_certs(0){
	cerr << "gtls_ca_db_item ctor" << endl;
}

gtls_ca_db_item::~gtls_ca_db_item(){
	cerr << "gtls_ca_db_item dtor" << endl;
	if( certs ){
		delete[] certs;
		certs = NULL;
		num_certs = 0;
	}
}

gtls_ca_db::gtls_ca_db(): caList(NULL), caListLength(0){
	cerr << "gtls_ca_db ctor" << endl;
}

gtls_ca_db::~gtls_ca_db(){
	cerr << "gtls_ca_db dtor" << endl;
	if( caList != NULL ){
		delete[] caList;
		caList = NULL;
		caListLength = 0;
	}
}

bool gtls_ca_db::getDb(gnutls_x509_crt_t ** db, size_t * db_length){
	if( !caList ){
// 		TODO: Results in deadlock in gtls_certificate_chain::control
// 		lock();

		std::list<ca_db_item*> &items = get_items();
		std::list<ca_db_item*>::iterator i;
		std::list<ca_db_item*>::iterator last = items.end();

		caListLength = 0;

		for( i = items.begin(); i != last; i++ ){
			gtls_ca_db_item *item = (gtls_ca_db_item*)*i;
			caListLength += item->num_certs;
		}

		caList = new gnutls_x509_crt_t[ caListLength ];

		size_t pos = 0;
		for( i = items.begin(); i != last; i++ ){
			gtls_ca_db_item *item = (gtls_ca_db_item*)*i;

			for( size_t k = 0; k < item->num_certs; k++ ){
				caList[ pos++ ] = item->certs[ k ];
			}
		}

// 		unlock();
	}

	*db = caList;
	*db_length = caListLength;
	return true;
}

#if 0
void gtls_ca_db::addDirectory( string dir ){
	X509_LOOKUP * lookup = NULL;
	ca_dbItem * item = new ca_dbItem();
	
	lookup = X509_STORE_add_lookup( 
			certDb, X509_LOOKUP_hash_dir() );
	if( lookup == NULL )
		throw certificate_exception_init(
			string("Could not create a directory lookup") );
	
	if( !X509_LOOKUP_add_dir( lookup, dir.c_str(), X509_FILETYPE_PEM ) )
		throw certificate_exception_file(
			"Could not open the directory "+dir );

	item->item = dir;
	item->type = CERT_DB_ITEM_TYPE_DIR;
	
	items.push_back( item );
	items_index = items.begin();
}
#endif



bool read_file( string file, gnutls_datum* data ){
	int fd;
        unsigned char* buf = NULL;
        size_t length;
        struct stat fileStat;

        fd = open( file.c_str(), O_RDONLY );

        if( fd < 0 ){
		return false;
        }

        int ret = fstat( fd, &fileStat );

        if( ret == -1 ){
		close( fd );
		return false;
        }

        length = fileStat.st_size;

	buf = new unsigned char[ length + 1 ];

        if( buf == NULL ){
		close( fd );
		return false;
        }

	size_t remaining = length;
	size_t pos = 0;

	while( remaining > 0 ){
		int res = read(fd, buf, remaining);

		if( res < 0 ){
			perror("read");
			throw certificate_exception("Read file error");
		}
		else if( res == 0 ){
			break;
		}

		remaining -= res;
		pos += res;
	}

	buf[ pos ] = 0;
        data->data = buf;
        data->size = pos+1;

// 	cerr << "Data: size " << pos + 1 << endl;
// 	cerr << buf << endl;

	close( fd );
	return true;
}

/*
        ret = gnutls_x509_crt_init( (gnutls_x509_crt_t*)&cert );

        if( ret != 0 ){
		throw certificate_exception_init( 
			"Could not initialize the certificate structure" );
        }

        ret = gnutls_x509_crt_import( cert, &certData, GNUTLS_X509_FMT_PEM );

        if( ret != 0 ){
		throw certificate_exception_file( 
			"Could not import the given certificate" );
        }

        munmap( buf, length );
        close( fd );

	file = fileName;
}
*/

ca_db_item* gtls_ca_db::create_dir_item( std::string dir ){
	ca_db_item * item = new gtls_ca_db_item();
	
	item->item = dir;
	item->type = CERT_DB_ITEM_TYPE_DIR;
	return item;
}

ca_db_item* gtls_ca_db::create_file_item( std::string file ){
	gnutls_datum_t data;

	memset(&data, 0, sizeof(data));
	
	if( !read_file( file, &data ) ){
		string msg = string("Can't find certificate file ") + file;
		throw certificate_exception( msg.c_str() );
	}

	size_t num_certs = 0;

	if( !gnutls_x509_crt_list_import(NULL, &num_certs, &data, GNUTLS_X509_FMT_PEM, GNUTLS_X509_CRT_LIST_IMPORT_FAIL_IF_EXCEED ) ){
		delete[] data.data;
		throw certificate_exception( "Can't load certificate file" );
// 		return NULL;
	}

 	gnutls_x509_crt_t *certs = new gnutls_x509_crt_t[ num_certs ];
	memset(certs, 0, sizeof(certs));

	int res = gnutls_x509_crt_list_import( certs, &num_certs, &data,
					       GNUTLS_X509_FMT_PEM, 0 );

	delete[] data.data;
	data.data = NULL;

	if( res < 0 ){
		cerr << "Error " << res << endl;
		throw certificate_exception( "Can't load certificate file (2)" );
// 		return NULL;
	}

	cerr << "Loaded " << res << " certificates" << endl;

	gtls_ca_db_item * item = new gtls_ca_db_item();
	item->item = file;
	item->type = CERT_DB_ITEM_TYPE_FILE;
	item->certs = certs;
	item->num_certs = num_certs;

// 	return NULL;
	return item;
}

ca_db_item* gtls_ca_db::create_cert_item( certificate* cert ){
	gtls_ca_db_item * item = new gtls_ca_db_item();
	
	item->item = "";
	item->type = CERT_DB_ITEM_TYPE_OTHER;
	item->num_certs = 1;
	item->certs = new gnutls_x509_crt_t[item->num_certs];
	item->certs[0] = dynamic_cast<gtls_certificate*>(cert)->get_certificate();
	return item;
}

gtls_certificate_chain::gtls_certificate_chain(){
}

gtls_certificate_chain::gtls_certificate_chain( MRef<certificate *> cert ){
}

gtls_certificate_chain::~gtls_certificate_chain(){
}

int gtls_certificate_chain::control( MRef<ca_db *> certDb){
	int result;
	unsigned int verify = 0;
	MRef<gtls_ca_db*> gtls_db =
		dynamic_cast<gtls_ca_db*>(*certDb);
	gnutls_x509_crt_t* ca_list = NULL;
	size_t ca_list_length = 0;
	gnutls_x509_crt_t* gtls_list = NULL;
	size_t gtls_list_length = 0;

	cerr << "gtls_certificate_chain::control" << endl;
	if( !gtls_db ){
		cerr << "Not gtls CA db" << endl;
		return 1;
	}

// 	lock();
	gtls_list_length = cert_list.size();

	if( gtls_list_length == 0 ){
		cerr << "certificate: Empty list of certificates"
			"to verify" << endl;
		return 0;
	}

	cerr << "Cert chain length " << gtls_list_length << endl;

	/* Chain of certificates */
	list< MRef<certificate *> >::iterator i = cert_list.begin();

	gtls_list = new gnutls_x509_crt_t[ gtls_list_length ];
	memset( gtls_list, 0, gtls_list_length * sizeof( gnutls_x509_crt_t ));

	for( size_t j = 0; j < gtls_list_length; j++,i++ ){
		MRef<gtls_certificate *> cert =
			dynamic_cast<gtls_certificate*>(**i);

		if( !cert ){
// 			unlock();
			delete[] gtls_list;
			// Not gtls cert
			cerr << "Not a gtls cert" << endl;
			return 1;
		}
		
		gtls_list[j] = cert->get_certificate();
	}
// 	unlock();

	if( !gtls_db->getDb( &ca_list, &ca_list_length ) ){
		delete[] gtls_list;
		cerr << "No CA db" << endl;
		return 1;
	}

	cerr << "CA db size " << ca_list_length << endl;

	result = gnutls_x509_crt_list_verify( gtls_list, gtls_list_length,
					      ca_list, ca_list_length,
// 					      crl_list, crl_list_length,
					      NULL, 0,
					      0, &verify);

	delete[] gtls_list;
	gtls_list = NULL;

	if( result < 0 ){
		cerr << "gnutls_x509_crt_list_verify failed" << endl;
		return 1;
	}

	cerr << "gnutls_x509_crt_list_verify returns " << verify << endl;
	return verify ? 0 : 1;
}
