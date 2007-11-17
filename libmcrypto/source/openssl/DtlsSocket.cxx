/*
  Copyright (C) 2005-2007 Mikael Magnusson
  
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
 * Author(s): Mikael Magnusson <mikma@users.sourceforge.net>
*/


#ifdef HAVE_CONFIG_H
#include<config.h>
#endif

#ifdef WIN32
#include<winsock2.h>
#elif defined HAVE_NETINET_IN_H
#endif

#include<libmnetutil/IPAddress.h>
#include<libmnetutil/UDPSocket.h>
#include<libmnetutil/NetworkException.h>
#include<libmcrypto/openssl/cert.h>

#include"DtlsSocket.h"

#include<iostream>
#include<stdio.h>
#include<stdlib.h>
#include<errno.h>
#include<openssl/err.h>


#ifndef _MSC_VER
// #include<unistd.h>
#endif


using namespace std;

DTLSSocket::DTLSSocket()
{
}

DTLSSocket::~DTLSSocket()
{
}

DTLSSocket* DTLSSocket::create( MRef<DatagramSocket *> sock,
				MRef<Certificate *> cert,
				MRef<CertificateSet *> cert_db )
{
	void *ssl_ctx = NULL;
	MRef<OsslCertificate*> ssl_cert;
	MRef<OsslCertificateSet*> ssl_db;

	if( cert )
		ssl_cert = (OsslCertificate*)*cert;

	if( cert_db )
		ssl_db = (OsslCertificateSet*)*cert_db;

	return new OdtlsSocket( sock, ssl_ctx, ssl_cert, ssl_db );
}


static int bio_socket_new(BIO *bi)
{
    fprintf(stderr, "bio_socket_new\n");

    bi->init = 1;
    bi->flags = 0;
    bi->ptr = NULL;
    return 1;
}

static int bio_socket_free(BIO *bio)
{
    fprintf(stderr, "bio_socket_free\n");

    bio->init = 1;
    bio->flags = 0;
    bio->ptr = NULL;
    return 1;
}

static int bio_socket_read(BIO *bio, char *out, int outl)
{
    fprintf(stderr, "bio_socket_read\n");
    return 0;
}

static int bio_socket_write(BIO *bio, const char *in, int inl)
{
	SSLSession *sess = (SSLSession *)bio->ptr;
	int res = 0;
    
	fprintf(stderr, "bio_socket_write %d\n", inl);

	if (!sess) {
		return -1;
	}

	res = sess->sendDgram( in, inl );

	return res;
}

static long bio_socket_ctrl(BIO *bio, int cmd, long num, void *ptr)
{
    fprintf(stderr, "bio_socket_ctrl %d %ld %p\n", cmd, num, ptr);

    switch (cmd) {
    case BIO_CTRL_WPENDING:
	return 0;
    case BIO_CTRL_DGRAM_SET_PEER:
	bio->ptr = ptr;
	return 1;
    case BIO_CTRL_FLUSH:
	return 1;
    default:
	fprintf(stderr, "Unknown ctrl %d\n", cmd);
	return -1;
    }
}

static int bio_socket_gets(BIO *bp, char *buf, int size)
{
    fprintf(stderr, "bio_socket_gets\n");
    return 0;
}

static int bio_socket_puts(BIO *bp, const char *str)
{
    fprintf(stderr, "bio_socket_puts\n");
    return 0;
}

#define BIO_TYPE_DTLS_SOCKET	(BIO_TYPE_SOURCE_SINK)

static BIO_METHOD socket_methods = {
    BIO_TYPE_DTLS_SOCKET,
    "Socket output",
    bio_socket_write,
    bio_socket_read,
    bio_socket_puts,
    bio_socket_gets,
    bio_socket_ctrl,
    bio_socket_new,
    bio_socket_free,
};


//
// IPSockAddr
// 
IPSockAddr::IPSockAddr( MRef<IPAddress *> addr, int32_t port ){
	this->addr = addr->clone();
	this->port = port;
}

IPSockAddr::~IPSockAddr(){
}

const IPAddress &IPSockAddr::getAddr(){
	return **addr;
}

int32_t IPSockAddr::getPort() const{
	return port;
}

bool IPSockAddr::operator ==(const IPSockAddr &sa) const{
	return *addr == *sa.addr && port == sa.port;
}


//
// SSLSession
// 
SSLSession::SSLSession( MRef<DatagramSocket *> to_sock,
			MRef<IPSockAddr *> to_sa ) 
  : sock( to_sock ), sa( to_sa ), mutex() {
	ssl = NULL;
	wbuf = NULL;
	rbuf = NULL;
	num = 0;
}

SSLSession::~SSLSession(){
	close();
}

void SSLSession::close(){
	if (ssl) {
		SSL_shutdown( ssl );
		SSL_free(ssl);
		ssl = NULL;
	}
}

bool SSLSession::init( SSL_CTX *ssl_ctx )
{
/*     int mtu = 65000; */
	int mtu = 1400;

	ssl = SSL_new( ssl_ctx );
	if (!ssl) {
		printf("SSL_new failed\n");
		ERR_print_errors_fp(stderr);
// 		dtls_close(sock);
		return false;
	}

	SSL_clear( ssl );

	SSL_set_options( ssl, SSL_OP_COOKIE_EXCHANGE );
	SSL_set_options( ssl, SSL_OP_NO_QUERY_MTU );
	SSL_set_mtu( ssl, mtu );
    
	wbuf = BIO_new( &socket_methods );
	BIO_dgram_set_peer( wbuf, this );

	rbuf = BIO_new( BIO_s_mem() );
	BIO_set_mem_eof_return( rbuf, -1);
  
	printf("BIO %p %p\n", rbuf, wbuf );

	SSL_set_bio( ssl, rbuf, wbuf );
	return true;
}


bool SSLSession::connect( SSL_CTX *ssl_ctx ) {
	mutex.lock();
	num++;
	printf("connect begin %d\n", num);

	if (!init( ssl_ctx )) {
		num--;
		mutex.unlock();
		return false;
	}

	SSL_set_connect_state( ssl );

	int res = SSL_connect( ssl );

	if ( res <= 0 ){
		int err = SSL_get_error( ssl, res );
		printf("connect error %d %d\n", res, err);
	}

	printf("connect sent\n");
	num--;
	mutex.unlock();

	return true;
}

bool SSLSession::accept( SSL_CTX *ssl_ctx )
{
	mutex.lock();
	num++;
	printf("accept\n");

	if (!init( ssl_ctx )) {
		mutex.unlock();
		return false;
	}

	SSL_set_accept_state(ssl) ;
	SSL_accept( ssl );
	printf("accepted\n");

	num--;
	mutex.unlock();

	return true;
}


int32_t SSLSession::recv( const void *enc_buf, int enc_len,
			  void *plain_buf, int plain_len ) {

	mutex.lock();
	num++;
	printf("SSLSession::recv %d %d %d\n", num, enc_len, plain_len);

	BIO *rbio = BIO_new_mem_buf((void*)enc_buf, enc_len) ;
	if (!rbio) {
		printf("dtls no bio\n");
		num--;
		mutex.unlock();
		return -1;
	}

	BIO_set_mem_eof_return(rbio, -1);

	ssl->rbio = rbio;

	int32_t res = 0;

	if ( !SSL_is_init_finished( ssl )) {
		res = SSL_do_handshake( ssl );
	} else {
		res = SSL_read(ssl, plain_buf, plain_len);
	}

	ssl->rbio = rbuf;
// 	BIO_free( rbio );

	if (res <= 0) {
		int err = SSL_get_error(ssl, res);
		printf("SSL_read failed: %d err: %d\n", res, err);
		ERR_print_errors_fp(stderr);

		switch (err) {
		case SSL_ERROR_NONE:
		case SSL_ERROR_WANT_WRITE:
		case SSL_ERROR_WANT_READ:
		case SSL_ERROR_WANT_X509_LOOKUP:
			printf("BLOCK\n");
			res = -1;
			break;
			
		case SSL_ERROR_SYSCALL:
		case SSL_ERROR_SSL:
			printf("ERROR\n");
			res = -1;
			break;
			
		case SSL_ERROR_ZERO_RETURN:
			printf("DONE\n");
			res = 0;
			break;
		}
		
		num--;
		mutex.unlock();
		return res;
	}

	if (SSL_is_init_finished(ssl)) {
// 		dtls_send_queued(sock);
	}

	printf("dtls_recv 2 %d\n", res);
	num--;
	mutex.unlock();
	return res;
}

int SSLSession::doSend(const void *buf, int buflen)
{
	int res;

	printf("dtls_do_send %d %d\n", buflen, SSL_is_init_finished(ssl));

	res = SSL_write(ssl, buf, buflen);
	if (res <= 0) {
		printf("SSL_write %d\n", res);
		ERR_print_errors_fp(stderr);
		return res;
	}

	return res;
}

int32_t SSLSession::sendDgram(const void *buf, int len) {
	printf("sendDgram %d\n", len);

	return sock->sendTo( sa->getAddr(), sa->getPort(), buf, len );
}

int32_t SSLSession::send(const void *buf, int len)
{
  int res;

  mutex.lock();
  num++;
  printf("dtls_send %d %d %d\n", num, len, SSL_is_init_finished(ssl));

  if (!SSL_is_init_finished( ssl )) {
	  printf("in init\n");

// 	  SSL_do_handshake( ssl );

// 	  SSL_connect( ssl );

	  // FIXME add to queue
//       transport_message_t *msg = NULL;

//       if (transport_message_create(&msg, (void*)buf, buflen))
// 	  return -1;

//       if (osip_fifo_add(sock->out_queue, msg)) {
// 	  transport_message_free(msg);
// 	  return -1;
//       }

	  num--;
	  mutex.unlock();
	  return 0;
  }

  // FIXME
//   dtls_send_queued(sock);

  res = doSend( buf, len );
  num--;
  mutex.unlock();
  return res;
}


// 
// OdtlsSocket
// 
bool OdtlsSocket::initDtlsSocket( void * &ctx,
				 MRef<OsslCertificate *> cert,
				 MRef<OsslCertificateSet *> cert_db ) {
	type = SOCKET_TYPE_DTLS_UDP;
	
	SSL_library_init();
	SSL_load_error_strings();

	SSL_METHOD *method = DTLSv1_method();
	ssl_ctx = SSL_CTX_new( method );
	if (!ssl_ctx) {
		printf("SSL_CTX_new failed\n");
		ERR_print_errors_fp(stderr);
		return false;
	}

	SSL_CTX_set_read_ahead( ssl_ctx, 1 );

// 	SSL_CTX_set_verify( ssl_ctx, SSL_VERIFY_NONE, verify_callback );

// 	SSL_CTX_set_verify( ssl_ctx, SSL_VERIFY_PEER | SSL_VERIFY_CLIENT_ONCE, 0);
// 	SSL_CTX_set_verify_depth( ssl_ctx, 5);

	if( !cert.isNull() ){
		MRef<PrivateKey*> pk = cert->getPk();
		MRef<OsslPrivateKey*> ssl_pk =
			dynamic_cast<OsslPrivateKey*>(*pk);

		/* Add a client Certificate */
		if( !ssl_pk || SSL_CTX_use_PrivateKey( ssl_ctx, 
						       ssl_pk->getOpensslPrivateKey() ) <= 0 ){
			delete ssl_ctx;
			return false;
		}

		if( SSL_CTX_use_certificate( this->ssl_ctx,
					     cert->getOpensslCertificate() ) <= 0 ){
			delete ssl_ctx;
			return false;
		}

		if( !SSL_CTX_check_private_key( ssl_ctx ) ){
			delete ssl_ctx;
			return false;
		}
	}

	if( !cert_db.isNull() ){
		/* Use this database for the certificates check */
		SSL_CTX_set_cert_store( ssl_ctx, cert_db->getDb());
	}

	ctx = ssl_ctx;

	printf("initDtlsSocket done\n");
	return true;
}


OdtlsSocket::OdtlsSocket( MRef<DatagramSocket *> sock, void * &ssl_ctx,
			MRef<OsslCertificate *> cert,
			MRef<OsslCertificateSet *> cert_db )
	: sock( sock ){
	initDtlsSocket( ssl_ctx, cert, cert_db );
}


OdtlsSocket::~OdtlsSocket(){
}


int32_t OdtlsSocket::getPort() {
	return sock->getPort();
}


int32_t OdtlsSocket::sendTo(const IPAddress &to_addr, int32_t port, const void *msg, int32_t len) {

	printf("sendTo\n");

	MRef<IPSockAddr *> sa = new IPSockAddr( to_addr.clone(), port );

	mutex.lock();
	MRef<SSLSession *> sess = sessions[ sa ];

	if (sess.isNull()) {
		cerr << "Connect session " << to_addr.getString() << ":" << port << endl;

		sess = new SSLSession( sock, sa );
		sessions[ sa ] = sess;

		if ( !sess->connect( ssl_ctx ) ) {
			sessions.erase( sa );
			mutex.unlock();
			return -1;
		}

		printf("Connected session\n");
	}

	mutex.unlock();
	return sess->send( msg, len );
}


int32_t OdtlsSocket::recvFrom(void *buf, int32_t len, MRef<IPAddress *>& from, int &port){
	printf("recvFrom %d\n", len);
	
	void *temp = alloca(len);
	int32_t enc_len = sock->recvFrom( temp, len, from, port );

	printf("recvFrom %d\n", enc_len);

	if (enc_len <= 0)
		return enc_len;

	MRef<IPSockAddr *> sa = new IPSockAddr( from, port );

	mutex.lock();
	MRef<SSLSession *> sess = sessions[ sa ];

	if( sess.isNull() ) {
		cerr << "Accept session " << from->getString() << ":" << port << endl;

		sess = new SSLSession( sock, sa );
		sessions[ sa ] = sess;

		if( !sess->accept( ssl_ctx ) ){
			sessions.erase( sa );
			mutex.unlock();
			return -1;
		}

		printf("Accepted session\n");
	}
	mutex.unlock();

	int32_t res = sess->recv(temp, enc_len, buf, len);

	if ( res == 0 ){
		// Remove session
		mutex.lock();
		sessions.erase( sa );
		mutex.unlock();

		// Don't close socket
		res = -1;
	}

	return res;
}


int32_t OdtlsSocket::recv(void *buf, int32_t len){
	MRef<IPAddress *> from;
	int port = 0;

	printf("recv %d\n", len);
	int32_t res = recvFrom( buf, len, from, port );

	return res;
}

bool OdtlsSocket::setLowDelay(){
	return sock->setLowDelay();
}

int32_t OdtlsSocket::getFd(){
	return sock->getFd();
}

MRef<IPAddress *> OdtlsSocket::getLocalAddress() const{
	cerr << "Getlocaladdress" << endl;
	return sock->getLocalAddress();
}

void OdtlsSocket::close(){
	map<MRef<IPSockAddr *>, MRef<SSLSession *> >::iterator iter;

	for( iter = sessions.begin(); iter != sessions.end(); iter++ ) {
		MRef<SSLSession *> sess = iter->second;

		if( !sess.isNull() ){
			sess->close();
		}
	}

	if ( sock ) {
		sock->close();
	}
}
	
/*
ostream& operator<<(ostream& out, OdtlsSocket& s){
//	int32_t buf[1024*10];
//	int32_t n = s.read(buf,1024*10);
//	out.write(buf,n);
	return out;
}
*/

/*
OdtlsSocket& operator<<(OdtlsSocket& sock, string str){
//	sock.write(str);
	return sock;
}
*/

