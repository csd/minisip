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
#include<gcrypt.h>

#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>

#include<iostream>

using namespace std;

#define UNIMPLEMENTED \
	string msg = string( __FUNCTION__ ) + " unimplemented"; \
	throw Exception(msg.c_str());


GtlsCertificate::GtlsCertificate():cert(NULL),rsaKey(NULL){
        gnutls_global_init();
}

//
// Factory methods
//

CertificateSet *CertificateSet::create(){
	return new GtlsCertificateSet();
}

PrivateKey* PrivateKey::load( const std::string private_key_filename ){
	return new GtlsPrivateKey( private_key_filename );
}

PrivateKey* PrivateKey::load( char *derEncPk, int length,
			  std::string password,
			  std::string path ){
	return new GtlsPrivateKey( derEncPk, length, password, path );
}


// Read PEM-encoded Certificate from a file
Certificate* Certificate::load( const std::string cert_filename )
{
	return new GtlsCertificate( cert_filename );
}

// Read PEM-encoded Certificate private key from a file
Certificate* Certificate::load( const std::string cert_filename,
				const std::string private_key_filename ){
	MRef<PrivateKey*> PrivateKey = new GtlsPrivateKey( private_key_filename );
	MRef<Certificate*> cert = new GtlsCertificate( cert_filename );

	cert->setPk( PrivateKey );

	// The caller is responsible for deleting cert
	cert->incRefCount();
	return *cert;
}

// Import DER-encoded Certificate from memory
Certificate* Certificate::load( unsigned char * der_cert,
				int length ){
	return new GtlsCertificate( der_cert, length );
}

// TODO
Certificate* Certificate::load( unsigned char * certData,
				int length,
				std::string path ){
	throw CertificateExceptionInit( "Unimplemented" );
// 	return new GtlsCertificate( certData, length, path );
}

CertificateChain* CertificateChain::create(){
	return new GtlsCertificateChain();
}


//
// GtlsRsaPriv
//
GtlsRsaPriv::GtlsRsaPriv( gnutls_x509_privkey_t aKey ):m_key(NULL){
	gcry_error_t err;
	gnutls_datum_t n[6];
	int i;

	memset(n, 0, sizeof(n));

	if( gnutls_x509_privkey_export_rsa_raw( aKey, &n[0], &n[1], &n[2],
						&n[3], &n[4], &n[5] )){
		// TODO change to Gtlsexception
		throw CertificateExceptionPkey("Private key invalid" );
	}

	gcry_mpi_t mpi[6];

	memset(mpi, 0, sizeof(mpi));
	for( i = 0; i < 6; i++ ){
		size_t nscanned = 0;

		err = gcry_mpi_scan( &mpi[i], GCRYMPI_FMT_USG,
				     n[i].data, n[i].size, &nscanned );
		gnutls_free( n[i].data );
		n[i].data = NULL;
		if( err ){
			for( int j = 0; j < 6; j++ ){
				if( n[j].data ){
					gnutls_free( n[j].data );
					n[j].data = NULL;
				}
			}

			throw CertificateExceptionPkey("Private key parameter invalid" );
		}
	}

//      RSA private-key sexp format
// 	(private-key
// 	 (rsa
// 	  (n n-mpi)
// 	  (e e-mpi)
// 	  (d d-mpi)
// 	  (p p-mpi)
// 	  (q q-mpi)
// 	  (u u-mpi)

	err = gcry_sexp_build( &m_key, NULL,
			       "(private-key(rsa (n %m)(e %m)(d %m)(p %m)(q %m)(u %m)))",
			       mpi[0], mpi[1], mpi[2], mpi[4], mpi[3], mpi[5]);

	for( i = 0; i < 6; i++ ){
		gcry_mpi_release( mpi[i] );
		mpi[i] = NULL;
	}

	if( err ){
		throw CertificateExceptionPkey("Private key parameters invalid" );
	}
}

GtlsRsaPriv::~GtlsRsaPriv(){
	if( m_key ){
		gcry_sexp_release( m_key );
		m_key = NULL;
	}
}

bool GtlsRsaPriv::decrypt( const unsigned char *data, int size,
			     unsigned char *retdata, int *retsize) const{
	gcry_error_t err;
	bool ret = false;
	gcry_mpi_t cipher_mpi = NULL;
	gcry_sexp_t cipher = NULL;
	gcry_sexp_t plain = NULL;
	gcry_mpi_t datampi = NULL;
	unsigned char *ptr = NULL;
	size_t ptrsize = 0;
	size_t pos = 0;
	int len = 0;

	err = gcry_mpi_scan(&cipher_mpi, GCRYMPI_FMT_USG, data, size, NULL);
	if( err ){
		goto error;
	}

	err = gcry_sexp_build( &cipher, NULL,
			       "(enc-val(flags)(rsa(a %m)))",
			       cipher_mpi );

	if( err ){
		goto error;
	}

	err = gcry_pk_decrypt( &plain, cipher, m_key );

	if( err ){
		goto error;
	}

	// gcry_pk_decrypt result:
	// (value plaintext)

	datampi = gcry_sexp_nth_mpi( plain, 1, GCRYMPI_FMT_NONE );

	if( !datampi ){
		goto error;
	}

	err = gcry_mpi_aprint( GCRYMPI_FMT_USG, &ptr, &ptrsize, datampi );

	if( err ){
		goto error;
	}

	// PKCS1 version 1.5 padding (RFC 2313)
	// RSA input value = 00 || BT || PS || 00 || D.
	// BT = 02 (public-key operation)
	// PS = k-3-||D|| random octets

	if( ptr[0] != 0x02 ){
		goto error;
	}

	if( ptrsize < 4 ){
		goto error;
	}

	pos = strnlen( (char*)ptr + 1, ptrsize - 1 ) + 2;
	len = ptrsize - pos;

	// Skip zeros at the beginning
	while( !ptr[pos] && len > 0 ){
		len--;
		pos++;
	}

	if( *retsize < len ){
		goto error;
	}

	*retsize = len;
	memcpy( retdata, ptr + pos, len );

	ret = true;

  error:
	if( cipher_mpi ){
		gcry_mpi_release( cipher_mpi );
	}
	if( cipher ){
		gcry_sexp_release( cipher );
	}
	if( plain ){
		gcry_sexp_release( plain );
	}
	if( datampi ){
		gcry_mpi_release( datampi );
	}
	if( ptr ){
		gcry_free( ptr );
	}

	return ret;
}

//
// Gtlsrsa_pub
//
Gtlsrsa_pub::Gtlsrsa_pub( gnutls_x509_crt_t aCert ):m_key(NULL){
	gcry_error_t err;
	gnutls_datum_t n;
	gnutls_datum_t e;

	memset(&n, 0, sizeof(n));
	memset(&e, 0, sizeof(e));

	if( gnutls_x509_crt_get_pk_rsa_raw( aCert, &n, &e ) ){
		throw CertificateExceptionInit( "Can't get RSA key from cert" );
	}

	gcry_mpi_t n_mpi = NULL;
	size_t nscanned = 0;
	err = gcry_mpi_scan( &n_mpi, GCRYMPI_FMT_USG,
			     n.data, n.size, &nscanned );

	gcry_free( n.data );
	n.data = NULL;

	if( err ){
		gcry_free( e.data );
		throw CertificateExceptionInit( "Invalid public key m parameter" );
	}

	gcry_mpi_t e_mpi = NULL;
	err = gcry_mpi_scan( &e_mpi, GCRYMPI_FMT_USG,
			     e.data, e.size, &nscanned );
	gcry_free( e.data );
	e.data = NULL;

	if( err ){
		gcry_mpi_release( e_mpi );
		throw CertificateExceptionInit( "Invalid public key e parameter" );
	}

	size_t erroff = 0;

	err = gcry_sexp_build( &m_key, &erroff,
			       "(key-data(public-key(rsa (n %m)(e %m))))",
			       n_mpi, e_mpi );

	gcry_mpi_release( n_mpi );
	n_mpi = NULL;
	gcry_mpi_release( e_mpi );
	e_mpi = NULL;

	if( err ){
		throw CertificateExceptionInit( "Invalid public key parameters" );
	}
}

Gtlsrsa_pub::~Gtlsrsa_pub(){
	if( m_key ){
		gcry_sexp_release( m_key );
		m_key = NULL;
	}
}

bool Gtlsrsa_pub::encrypt( const unsigned char *data, int size,
			    unsigned char *retdata, int *retsize) const{
	bool ret = false;
	gcry_error_t err;
	gcry_mpi_t data_mpi = NULL;
	gcry_sexp_t cipher = NULL;
	gcry_sexp_t data_sexp = NULL;
	gcry_sexp_t rsa = NULL;
	gcry_sexp_t a = NULL;
	gcry_mpi_t datampi = NULL;
	size_t erroff = 0;
	size_t len = 0;

	if( gcry_mpi_scan(&data_mpi, GCRYMPI_FMT_USG, data, size, NULL ) ){
		goto error;
	}

	if( gcry_sexp_build( &data_sexp, &erroff,
			     "(data(flags pkcs1)(value %m))",
			     data_mpi ) ){
		goto error;
	}

	if( (err = gcry_pk_encrypt( &cipher, data_sexp, m_key ) ) ){
		goto error;
	}

// 	(enc-val
// 	 (rsa
// 	  (a a-mpi)))

	rsa = gcry_sexp_nth( cipher, 1 );
	if( !rsa ){
		goto error;
	}

	a = gcry_sexp_nth( rsa, 1 );
	if( !a ){
		goto error;
	}

	if( !(datampi = gcry_sexp_nth_mpi( a, 1, GCRYMPI_FMT_USG )) ){
		goto error;
	}

	if( gcry_mpi_print( GCRYMPI_FMT_USG,
			    (unsigned char*)retdata,
			    *retsize, &len, datampi ) ){
		goto error;
	}

	*retsize = len;
	ret = true;

  error:
	if( data_mpi )
		gcry_mpi_release( data_mpi );
	if( cipher )
		gcry_sexp_release( cipher );
	if( data_sexp )
		gcry_sexp_release( data_sexp );
	if( rsa )
		gcry_sexp_release( rsa );
	if( a )
		gcry_sexp_release( a );
	if( datampi )
		gcry_mpi_release( datampi );

	return ret;
}


GtlsPrivateKey::~GtlsPrivateKey(){
	if( privateKey != NULL ){
		gnutls_x509_privkey_deinit( privateKey );
	}

	privateKey = NULL;

	if( rsaPriv ){
		delete rsaPriv;
		rsaPriv = NULL;
	}
}

const string &GtlsPrivateKey::getFile() const{
	return pk_file;
}


// Read PEM-encoded Certificate from a file
GtlsCertificate::GtlsCertificate( const string certFilename ):rsaKey(NULL){
        gnutls_global_init();
	openFromFile( certFilename );
}

// Import DER-encoded Certificate from memory
GtlsCertificate::GtlsCertificate( unsigned char * derCert, int length ):rsaKey(NULL){
        int ret;
        gnutls_datum certData;

	gnutls_global_init();

	ret = gnutls_x509_crt_init( (gnutls_x509_crt_t*)&cert );

        if( ret != 0 ){
		throw CertificateExceptionInit(
			"Could not initialize the Certificate structure" );
        }

	certData.data = derCert;
	certData.size = length;

	ret = gnutls_x509_crt_import( cert, &certData, GNUTLS_X509_FMT_DER );

        if( ret != 0 ){
		throw CertificateException(
			"Could not import the given Certificate" );
        }

	if( rsaKey ){
		delete rsaKey;
		rsaKey = NULL;
	}

	rsaKey = new Gtlsrsa_pub( cert );
}

GtlsCertificate::~GtlsCertificate(){
	if( cert != NULL ){
		gnutls_x509_crt_deinit( cert );
	}

	cert = NULL;

	if( rsaKey ){
		delete rsaKey;
		rsaKey = NULL;
	}
}

// Read PEM-encoded Certificate from a file
void GtlsCertificate::openFromFile( string fileName ){
	int fd;
        void * certBuf = NULL;
        size_t length;
        struct stat fileStat;
        gnutls_datum certData;

        fd = open( fileName.c_str(), O_RDONLY );

        if( fd == -1 ){
		throw CertificateExceptionFile(
			"Could not open the given Certificate file" );

        }

        int ret = fstat( fd, &fileStat );

        if( ret == -1 ){
		throw CertificateExceptionFile(
			"Could not stat the given Certificate file" );
        }

        length = fileStat.st_size;

        certBuf = mmap( 0, length, PROT_READ, MAP_SHARED, fd, 0 );

        if( certBuf == NULL ){
		throw CertificateExceptionInit(
			"Could not mmap the Certificate file" );
        }

        certData.data = (unsigned char*)certBuf;
        certData.size = length;


        ret = gnutls_x509_crt_init( (gnutls_x509_crt_t*)&cert );

        if( ret != 0 ){
		throw CertificateExceptionInit(
			"Could not initialize the Certificate structure" );
        }

        ret = gnutls_x509_crt_import( cert, &certData, GNUTLS_X509_FMT_PEM );

        if( ret != 0 ){
		throw CertificateExceptionFile(
			"Could not import the given Certificate" );
        }

        munmap( certBuf, length );
        close( fd );

	file = fileName;

	if( rsaKey ){
		delete rsaKey;
		rsaKey = NULL;
	}

	rsaKey = new Gtlsrsa_pub( cert );
}

int GtlsPrivateKey::signData( unsigned char * data, int dataLength,
			    unsigned char * sign, int * sign_length ){
	int err;
	size_t length = *sign_length;

	if( privateKey == NULL ){
		sign = NULL;
		*sign_length = 0;
		throw CertificateException(
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

	if( err == GNUTLS_E_SHORT_MEMORY_BUFFER ){
		memset( sign, 0, *sign_length );
		*sign_length = length;
		return 0;
	}
	else if( err < 0 ){
		cerr << "GNUTLS error " << gnutls_strerror( err ) << endl;
		throw CertificateException(
			"Signature of data failed" );
	}

	*sign_length = length;

	return 0;
}

int GtlsCertificate::verifSign( unsigned char * data, int data_length,
				  unsigned char * sign, int sign_length )
{
	int err;
	gnutls_datum dataStruct;
	gnutls_datum signStruct;

	dataStruct.data = data;
	dataStruct.size = data_length;

	signStruct.data = sign;
	signStruct.size = sign_length;

	if( cert == NULL ){
		throw CertificateException(
			"No Certificate open while verifying a signature" );
	}

	err = gnutls_x509_crt_verify_data( cert, 0, &dataStruct, &signStruct );

	return err;
 }

bool GtlsCertificate::publicEncrypt( const unsigned char *data, int size,
				      unsigned char *retdata, int *retsize){
	if( !rsaKey )
		return false;

	return rsaKey->encrypt( data, size, retdata, retsize );
}

int GtlsCertificate::getDerLength(){
	size_t size = 0;

	int ret = gnutls_x509_crt_export( cert, GNUTLS_X509_FMT_DER,
					  NULL, &size );

	if( ret == GNUTLS_E_SHORT_MEMORY_BUFFER )
		return size;
	else
		return -1;
}

void GtlsCertificate::getDer( unsigned char * output, unsigned int * length ){

	int ret;

	ret = gnutls_x509_crt_export( cert, GNUTLS_X509_FMT_DER,
			output, length );
	if( ret == GNUTLS_E_SHORT_MEMORY_BUFFER ){
		throw CertificateException(
			"Given buffer is to short" );
	}

	if( ret < 0 ){
		throw CertificateException(
			"An error occured while exporting the Certificate" );
	}
}

string GtlsCertificate::getName(){
	int ret;
	char * buf;
	size_t size = 1024;

	buf = (char *)malloc( size );
	if( buf == NULL ){
		throw CertificateExceptionInit(
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
			throw CertificateExceptionInit(
				"Not enough memory" );
		}
		ret = gnutls_x509_crt_get_dn( cert, buf, &size );
	}

	if( ret < 0 ){
		throw CertificateException(
			"An error occured in getName()" );
	}

	string output( buf, size );

	free( buf );
	return output;
}

string GtlsCertificate::getCn(){
	int ret;
	char * buf;
	size_t size = 1024;

	buf = (char *)malloc( size );
	if( buf == NULL ){
		throw CertificateExceptionInit(
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
			throw CertificateExceptionInit(
				"Not enough memory" );
		}
		ret = gnutls_x509_crt_get_dn_by_oid( cert,
						     GNUTLS_OID_X520_COMMON_NAME,
						     0, 0, buf, &size );
	}

	if( ret < 0 ){
		throw CertificateException(
			"An error occured in getName()" );
	}

	string output( buf, size );

	free( buf );
	return output;

}

std::vector<std::string> GtlsCertificate::getAltName( SubjectAltName type ){
	int ret;
	char * buf;
	size_t bufSize = 1024;
	gnutls_x509_subject_alt_name_t gType;
	vector<string> output;

	switch( type ){
		case SAN_DNSNAME: gType = GNUTLS_SAN_DNSNAME; break;
		case SAN_RFC822NAME: gType = GNUTLS_SAN_RFC822NAME; break;
		case SAN_URI: gType = GNUTLS_SAN_URI; break;
		case SAN_IPADDRESS: gType = GNUTLS_SAN_IPADDRESS; break;
		default:
			throw CertificateException( "Unsupported SubjectAltName type" );
	}

	buf = (char *)malloc( bufSize );
	if( buf == NULL ){
		throw CertificateExceptionInit(
			"Not enough memory" );
	}

	for( int i = 0;;i++ ){
		size_t size = bufSize;
		ret = gnutls_x509_crt_get_subject_alt_name( cert, i,
							    buf, &size, NULL );

		/* This should not happen very often */
		if( ret == GNUTLS_E_SHORT_MEMORY_BUFFER ){
			bufSize = size;
			buf = (char *) realloc( buf, bufSize );
			if( buf == NULL ){
				throw CertificateExceptionInit(
					"Not enough memory" );
			}

			ret = gnutls_x509_crt_get_subject_alt_name( cert, i,
								    buf, &size,
								    NULL );
		}

		if( ret == gType ){
			string name( buf, size );

			output.push_back( name );
		}
		else if( ret == GNUTLS_E_REQUESTED_DATA_NOT_AVAILABLE ){
			break;
		}
		else if( ret == GNUTLS_E_X509_UNKNOWN_SAN ){
			continue;
		}
		else if( ret < 0 ){
			cerr << "GNUTLS error " << gnutls_strerror( ret ) << endl;
			throw CertificateException(
				"An error occured in get_alt_name()" );
		}
	}

	free( buf );
	return output;
}

vector<string> GtlsCertificate::getSubjectInfoAccess() {
	// TODO

	vector<string> output;
	/*
	string oid("1.3.6.1.5.5.7.1.11");
	int ret;
	char * buf;
	size_t bufSize = 4096;

	for (int i=0;; i++) {
	size_t size = bufSize;
	buf = new char[size];
	unsigned int critical;
	ret = gnutls_x509_crt_get_extension_oid(cert, i, buf, &size);
		//ret = gnutls_x509_crt_get_extension_info(cert, i, buf, &bufSize, &critical);
		//ret = gnutls_x509_crt_get_extension_by_oid (cert, oid.c_str(), i, buf, &bufSize, &critical);
	if (ret == GNUTLS_E_REQUESTED_DATA_NOT_AVAILABLE) {
	delete[] buf;
	break;
} else if (ret < 0) {
	cerr << "GNUTLS error " << gnutls_strerror( ret ) << endl;
	delete[] buf;
	throw CertificateException(
	"An error occured in getSubjectInfoAccess()" );
}
	string name( buf, size );
	std::cerr << "GNUTLS: ext #" << i << ": " << name << std::endl;
	delete[] buf;
}
	std::cerr << "TESTING" << std::endl;
	for( int i = 0;;i++ ){
	size_t size = bufSize;
	buf = new char[size];
	unsigned int critical;
	gnutls_datum_t siaDatum;
		//ret = gnutls_x509_crt_get_extension_data (cert, i, buf, &size);— Function: int gnutls_x509_crt_get_extension_by_oid (gnutls_x509_crt_t cert, const char * oid, int indx, void * buf, size_t * sizeof_buf, unsigned int * critical);
	ret = gnutls_x509_crt_get_extension_by_oid (cert, oid.c_str(), i, &siaDatum, &size, &critical);
	if (ret == GNUTLS_E_REQUESTED_DATA_NOT_AVAILABLE) {
	delete[] buf;
	break;
} else if (ret < 0) {
	cerr << "GNUTLS error " << gnutls_strerror( ret ) << endl;
	delete[] buf;
	throw CertificateException(
	"An error occured in getSubjectInfoAccess()" );
}

	string name( buf, size );

	output.push_back( name );
	delete[] buf;
}
	*/
	return output;
}

string GtlsCertificate::getIssuer(){
	int ret;
	char * buf;
	size_t size = 1024;

	buf = (char *)malloc( size );
	if( buf == NULL ){
		throw CertificateExceptionInit(
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
			throw CertificateExceptionInit(
				"Not enough memory" );
		}
		ret = gnutls_x509_crt_get_issuer_dn( cert, buf, &size );
	}

	if( ret < 0 ){
		throw CertificateException(
			"An error occured in getName()" );
	}

	string output( buf, size );

	free( buf );
	return output;
}

string GtlsCertificate::getIssuerCn(){
	int ret;
	char * buf;
	size_t size = 1024;

	buf = (char *)malloc( size );
	if( buf == NULL ){
		throw CertificateExceptionInit(
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
			throw CertificateExceptionInit(
				"Not enough memory" );
		}
		ret = gnutls_x509_crt_get_issuer_dn_by_oid( cert,
							    GNUTLS_OID_X520_COMMON_NAME,
							    0, 0, buf, &size );
	}

	if( ret < 0 ){
		throw CertificateException(
			"An error occured in getName()" );
	}

	string output( buf, size );

	free( buf );
	return output;
}

// Read PEM-encoded private key from a file
GtlsPrivateKey::GtlsPrivateKey( const string &file ){
	int fd;
        void * pkBuf = NULL;
        size_t length;
        struct stat fileStat;
        gnutls_datum pkData;

        fd = open( file.c_str(), O_RDONLY );

        if( fd == -1 ){
		throw CertificateExceptionFile(
			"Could not open the given private key file" );

        }

        int ret = fstat( fd, &fileStat );

        if( ret == -1 ){
		throw CertificateExceptionFile(
			"Could not stat the given private key file" );
        }

        length = fileStat.st_size;

        pkBuf = mmap( 0, length, PROT_READ, MAP_SHARED, fd, 0 );

        if( pkBuf == NULL ){
		throw CertificateExceptionInit(
			"Could not mmap the Certificate file" );
        }

        pkData.data = (unsigned char*)pkBuf;
        pkData.size = length;


        ret = gnutls_x509_privkey_init( (gnutls_x509_privkey_t*)&privateKey );

        if( ret != 0 ){
		throw CertificateExceptionInit(
			"Could not initialize the private key structure" );
        }

        ret = gnutls_x509_privkey_import( privateKey, &pkData,
			GNUTLS_X509_FMT_PEM );

        if( ret != 0 ){
		throw CertificateExceptionFile(
			"Could not import the given private key" );
        }

        munmap( pkBuf, length );
        close( fd );

	pk_file = file;
	rsaPriv = new GtlsRsaPriv( privateKey );
}

// Import DER-encoded private key from memory
GtlsPrivateKey::GtlsPrivateKey(char * pkInput, int length,
			     const string &password,
			     const string &path )
{
   /*Not checked if working correctly*/

   gnutls_datum pkData;

   int ret = gnutls_x509_privkey_init( &privateKey );

   if( ret != 0 )
     {
	throw CertificateExceptionInit(
					   "Could not initialize the private key structure" );
     }

   pkData.data = (unsigned char*)pkInput;
   pkData.size = length;


   ret = gnutls_x509_privkey_import_pkcs8 (privateKey, &pkData, GNUTLS_X509_FMT_DER, password.c_str(), 0);

   if( ret != 0 )
     {
	throw CertificateExceptionFile("Could not import the given private key" );
     }

   pk_file = path;
}


bool GtlsPrivateKey::checkCert( Certificate* cert ){
	GtlsCertificate* Gtlscert =
		dynamic_cast<GtlsCertificate*>( cert );

	if( !Gtlscert ){
		return false;
	}

	byte_t publicKeyId[20];
	byte_t privateKeyId[20];
	size_t idLength;

	/* Check that the private key matches the Certificate */
	idLength = 20;
	int ret = gnutls_x509_crt_get_key_id( Gtlscert->getCertificate(),
					      0, publicKeyId, &idLength );

	if( ret < 0 ){
		throw CertificateException("An error occured when computing the key id" );
	}

	ret = gnutls_x509_privkey_get_key_id( privateKey, 0, privateKeyId, &idLength );

	if( ret < 0 ){
		throw CertificateException("An error occured when computing the key id" );
	}
	for( unsigned int i = 0; i < idLength; i++ ){
		if( privateKeyId[i] != publicKeyId[i] ){
			return false;
		}
	}

	return true;
}


int GtlsCertificate::control( CertificateSet * certDb ){
	int result;
	unsigned int verify = 0;
	MRef<GtlsCertificateSet*> Gtlsdb =
		dynamic_cast<GtlsCertificateSet*>( certDb );
	gnutls_x509_crt_t* ca_list = NULL;
	size_t ca_list_length = 0;

	if( !Gtlsdb ){
		cerr << "Not gtls CA db" << endl;
		return 0;
	}

	if( !Gtlsdb->getDb( &ca_list, &ca_list_length ) ){
		cerr << "No CA db" << endl;
		return 0;
	}

	result = gnutls_x509_crt_verify( cert,
					 ca_list, ca_list_length,
					 GNUTLS_VERIFY_ALLOW_X509_V1_CA_CRT,
					 &verify);

	if( result < 0 ){
		cerr << "gnutls_x509_crt_verify failed" << endl;
		return 0;
	}

#ifdef DEBUG_OUTPUT
	cerr << "gnutls_x509_crt_verify returns " << verify << endl;
#endif
	return verify ? 0 : 1;
}

int GtlsCertificate::envelopeData( unsigned char * data, int size, unsigned char *retdata, int *retsize,
				     unsigned char *enckey, int *enckeylgth, unsigned char **iv){
	UNIMPLEMENTED;
}

int GtlsPrivateKey::denvelopeData(unsigned char * data, int size, unsigned char *retdata, int *retsize,
				     unsigned char *enckey, int enckeylgth, unsigned char *iv){
	UNIMPLEMENTED;
}

bool GtlsPrivateKey::privateDecrypt( const unsigned char *data, int size,
				    unsigned char *retdata, int *retsize){
	if( !rsaPriv )
		return false;

	return rsaPriv->decrypt( data, size, retdata, retsize );
}

//
// End of GtlsCertificate
//

GtlsCertificateSetItem::GtlsCertificateSetItem(): certs(NULL), num_certs(0){
}

GtlsCertificateSetItem::~GtlsCertificateSetItem(){
	if( certs ){
		for( unsigned int i=0; i < num_certs; i++ ){
			gnutls_x509_crt_deinit( certs[i] );
			certs[i] = NULL;
		}

		delete[] certs;
		certs = NULL;
		num_certs = 0;
	}
}

GtlsCertificateSet::GtlsCertificateSet(): caList(NULL), caListLength(0){
}

GtlsCertificateSet::~GtlsCertificateSet(){
	if( caList != NULL ){
		delete[] caList;
		caList = NULL;
		caListLength = 0;
	}
}

bool GtlsCertificateSet::getDb(gnutls_x509_crt_t ** db, size_t * db_length){
	if( !caList ){
// 		TODO: Results in deadlock in GtlsCertificateChain::control
// 		lock();

		std::list<MRef<CertificateSetItem*> > &items = getItems();
		std::list<MRef<CertificateSetItem*> >::iterator i;
		std::list<MRef<CertificateSetItem*> >::iterator last = items.end();

		caListLength = 0;

		for( i = items.begin(); i != last; i++ ){
			GtlsCertificateSetItem *item =
				dynamic_cast<GtlsCertificateSetItem*>(**i);
			caListLength += item->num_certs;
		}

		caList = new gnutls_x509_crt_t[ caListLength ];

		size_t pos = 0;
		for( i = items.begin(); i != last; i++ ){
			GtlsCertificateSetItem *item =
				dynamic_cast<GtlsCertificateSetItem*>(**i);

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
void GtlsCertificateSet::addDirectory( string dir ){
	X509_LOOKUP * lookup = NULL;
	CertificateSetItem * item = new CertificateSetItem();

	lookup = X509_STORE_add_lookup(
			certDb, X509_LOOKUP_hash_dir() );
	if( lookup == NULL )
		throw CertificateExceptionInit(
			string("Could not create a directory lookup") );

	if( !X509_LOOKUP_add_dir( lookup, dir.c_str(), X509_FILETYPE_PEM ) )
		throw CertificateExceptionFile(
			"Could not open the directory "+dir );

	item->item = dir;
	item->type = CERT_DB_ITEM_TYPE_DIR;

	items.push_back( item );
	items_index = items.begin();
}
#endif



bool readFile( string file, gnutls_datum* data ){
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
			throw CertificateException("Read file error");
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
		throw CertificateExceptionInit(
			"Could not initialize the Certificate structure" );
        }

        ret = gnutls_x509_crt_import( cert, &certData, GNUTLS_X509_FMT_PEM );

        if( ret != 0 ){
		throw CertificateExceptionFile(
			"Could not import the given Certificate" );
        }

        munmap( buf, length );
        close( fd );

	file = fileName;
}
*/
/*
MRef<CertificateSetItem*> GtlsCertificateSet::createDirItem( std::string dir ){
	CertificateSetItem * item = new GtlsCertificateSetItem();

	item->item = dir;
	item->type = CERT_DB_ITEM_TYPE_DIR;
	return item;
}

MRef<CertificateSetItem*> GtlsCertificateSet::createFileItem( std::string file ){
	gnutls_datum_t data;

	memset(&data, 0, sizeof(data));

	if( !readFile( file, &data ) ){
		string msg = string("Can't find Certificate file ") + file;
		throw CertificateException( msg.c_str() );
	}

	size_t num_certs = 0;

	if( !gnutls_x509_crt_list_import(NULL, &num_certs, &data, GNUTLS_X509_FMT_PEM, GNUTLS_X509_CRT_LIST_IMPORT_FAIL_IF_EXCEED ) ){
		delete[] data.data;
		throw CertificateException( "Can't load Certificate file" );
// 		return NULL;
	}

 	gnutls_x509_crt_t *certs = new gnutls_x509_crt_t[ num_certs ];
	memset(certs, 0, sizeof(certs));

	int res = gnutls_x509_crt_list_import( certs, &num_certs, &data,
					       GNUTLS_X509_FMT_PEM, 0 );

	delete[] data.data;
	data.data = NULL;

	if( res < 0 ){
		cerr << "GNUTLS error " << gnutls_strerror( res ) << endl;
		throw CertificateException( "Can't load Certificate file (2)" );
// 		return NULL;
	}

#ifdef DEBUG_OUTPUT
	cerr << "Loaded " << res << " Certificates" << endl;
#endif

	GtlsCertificateSetItem * item = new GtlsCertificateSetItem();
	item->item = file;
	item->type = CERT_DB_ITEM_TYPE_FILE;
	item->certs = certs;
	item->num_certs = num_certs;

// 	return NULL;
	return item;
}
*/
MRef<CertificateSetItem*> GtlsCertificateSet::createCertItem( MRef<Certificate*> cert ){
	GtlsCertificateSetItem * item = new GtlsCertificateSetItem();

	//item->item = "";
	//item->type = CERT_DB_ITEM_TYPE_OTHER;
	item->num_certs = 1;
	item->certs = new gnutls_x509_crt_t[item->num_certs];
	item->certs[0] = NULL;
	item->setCertificate(cert);
	item->reindexCert();

	int ret = gnutls_x509_crt_init( &item->certs[0] );

	if( ret != 0 ){
		throw CertificateExceptionInit(
		 	"Could not initialize the Certificate structure" );
	}

	gnutls_datum der;

	der.size = cert->getDerLength();
	der.data = new byte_t[ der.size ];
	cert->getDer( der.data, &der.size );

	ret = gnutls_x509_crt_import( item->certs[0], &der, GNUTLS_X509_FMT_DER );

	delete[] der.data;
	der.data = NULL;

	if( ret != 0 ){
	 	throw CertificateException(
		 	"Could not import the given Certificate" );
	}

	return item;
}

GtlsCertificateChain::GtlsCertificateChain(){
}

GtlsCertificateChain::GtlsCertificateChain( MRef<Certificate *> cert ){
}

GtlsCertificateChain::~GtlsCertificateChain(){
}

int GtlsCertificateChain::control( MRef<CertificateSet *> certDb){
	int result;
	unsigned int verify = 0;
	MRef<GtlsCertificateSet*> Gtlsdb =
		dynamic_cast<GtlsCertificateSet*>(*certDb);
	gnutls_x509_crt_t* ca_list = NULL;
	size_t ca_list_length = 0;
	gnutls_x509_crt_t* Gtlslist = NULL;
	size_t Gtlslist_length = 0;

	if( !Gtlsdb ){
		cerr << "Not gtls CA db" << endl;
		return 0;
	}

// 	lock();
	Gtlslist_length = cert_list.size();

	if( Gtlslist_length == 0 ){
#ifdef DEBUG_OUTPUT
		cerr << "Certificate: Empty list of Certificates"
			"to verify" << endl;
#endif
		// Return success
		return 1;
	}

	/* Chain of Certificates */
	list< MRef<Certificate *> >::iterator i = cert_list.begin();

	Gtlslist = new gnutls_x509_crt_t[ Gtlslist_length ];
	memset( Gtlslist, 0, Gtlslist_length * sizeof( gnutls_x509_crt_t ));

	for( size_t j = 0; j < Gtlslist_length; j++,i++ ){
		MRef<GtlsCertificate *> cert =
			dynamic_cast<GtlsCertificate*>(**i);

		if( !cert ){
// 			unlock();
			delete[] Gtlslist;
			// Not gtls cert
			cerr << "Not a gtls cert" << endl;
			return 0;
		}

		Gtlslist[j] = cert->getCertificate();
	}
// 	unlock();

	if( !Gtlsdb->getDb( &ca_list, &ca_list_length ) ){
		delete[] Gtlslist;
		cerr << "No CA db" << endl;
		return 0;
	}

	result = gnutls_x509_crt_list_verify( Gtlslist, Gtlslist_length,
					      ca_list, ca_list_length,
// 					      crl_list, crl_list_length,
					      NULL, 0,
					      GNUTLS_VERIFY_ALLOW_X509_V1_CA_CRT,
					      &verify);

	delete[] Gtlslist;
	Gtlslist = NULL;

	if( result < 0 ){
		cerr << "gnutls_x509_crt_list_verify failed" << endl;
		return 0;
	}

#ifdef DEBUG_OUTPUT
	cerr << "gnutls_x509_crt_list_verify returns " << verify << endl;
#endif
	return verify ? 0 : 1;
}
