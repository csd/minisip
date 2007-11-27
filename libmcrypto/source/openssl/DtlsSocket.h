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
 * Authors: Mikael Magnusson <mikma@users.sourceforge.net>
 */

#ifndef ODTLSSOCKET_H
#define ODTLSSOCKET_H

#include<map>
#include<libmcrypto/DtlsSocket.h>

#include <openssl/ssl.h>

class IPAddress;

class LIBMCRYPTO_API IPSockAddr : public MObject {
	public:
		IPSockAddr( MRef<IPAddress *> addr, int32_t port );
		virtual ~IPSockAddr();

		virtual bool operator ==(const IPSockAddr &sa) const;

		virtual const IPAddress &getAddr();

		virtual int32_t getPort() const;

	private:
		MRef<IPAddress *> addr;
		int32_t port;
};

struct mrefIpSockAddrCmp {
		bool operator()( MRef<IPSockAddr *> ip1,
				 MRef<IPSockAddr *> ip2 ) const {
			if( ip1.isNull() ){
				return ip2.isNull();
			}
			else if( ip2.isNull() ){
				return false;
			}
			else{
				return **ip1 == **ip2;
			}
		}
};

class LIBMCRYPTO_API SSLSession : public MObject{
	public:
		SSLSession( MRef<DatagramSocket *> sock,
			    MRef<IPSockAddr *> sa );
		virtual ~SSLSession();
		virtual bool init( SSL_CTX *ssl_ctx );
		virtual bool connect( SSL_CTX *ssl_ctx );
		virtual bool accept( SSL_CTX *ssl_ctx );
		virtual int32_t sendDgram( const void *buf, int len );
		virtual int32_t send( const void *buf, int len );
		virtual int32_t recv( const void *enc_buf, int enc_len,
				      void *plain_buf, int plain_len );
		virtual void close();
		
	protected:	
		virtual int doSend(const void *buf, int buflen);

	private:
		MRef<DatagramSocket *> sock;
		MRef<IPSockAddr *> sa;
		SSL *ssl;
		BIO *wbuf;
		BIO *rbuf;
		Mutex mutex;
		int num;
};

class LIBMCRYPTO_API OdtlsSocket : public DTLSSocket {
	public:
	
		OdtlsSocket( MRef<DatagramSocket *> sock, void * &ssl_ctx,
			    MRef<OsslCertificate *> cert = NULL,
			    MRef<OsslCertificateSet *> cert_db=NULL );
		
		virtual ~OdtlsSocket();

		virtual std::string getMemObjectType() const{return "OdtlsSocket";}

		virtual int32_t getPort();
		
		virtual int32_t sendTo(const IPAddress &to_addr, int32_t port, const void *msg, int32_t len);
		
		virtual int32_t recvFrom(void *buf, int32_t len, MRef<IPAddress *>& from, int &port);
		
		virtual int32_t recv(void *buf, int32_t len);

		virtual bool setLowDelay();

		virtual int32_t getFd();

		virtual MRef<IPAddress *> getLocalAddress() const;

		virtual void close();

	private:
		bool initDtlsSocket( void * &ssl_ctx,
				     MRef<OsslCertificate *> cert,
				     MRef<OsslCertificateSet *> cert_db );

		MRef<DatagramSocket *> sock;
		MRef<OsslCertificate *> peer_cert;
		MRef<OsslCertificateSet *> cert_db;
		std::map<MRef<IPSockAddr *>, MRef<SSLSession *>, mrefIpSockAddrCmp > sessions;
		SSL_CTX *ssl_ctx;
		Mutex mutex;	/* Protects sessions */
};
#endif
