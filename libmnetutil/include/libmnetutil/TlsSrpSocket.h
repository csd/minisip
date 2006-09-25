/*
 *  Copyright (C) 2004-2006 the Minisip Team
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation; either
 *  version 2.1 of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
 *  */

/* Copyright (C) 2006
 *
 * Authors: Erik Ehrlund <eehrlund@kth.se>
*/



#include <gnutls/gnutls.h>
#include <gnutls/extra.h>
#include <string>
#include <libmnetutil/StreamSocket.h>
#include <libmnetutil/IP4Address.h>
using namespace std;

class TlsSrpSocket : public StreamSocket
{
   
 public:
   TlsSrpSocket(string addrs, int32_t port, string user, string pass);
   virtual ~TlsSrpSocket();
   virtual int32_t write(const void *msg, int length);
   virtual int32_t write(string msg);
   virtual int32_t read (void *buf, int length);
 private:
   void TlsSrpSocketSrp_init(string addrs, int32_t port, string user, string pass);
   gnutls_session_t session;
   int fd;
   gnutls_srp_client_credentials_t srp_cred;
   
};
