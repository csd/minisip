/*
 Copyright (C) 2004-2006 the Minisip Team
 
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
 Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
 */

/* Copyright (C) 2006
 *
 * Authors: Erik Ehrlund <eehrlund@kth.se>
 *          
*/
#ifndef ONLINECONFBACKEND_H
#define ONLINECONFBACKEND_H

#include<libminisip/libminisip_config.h>

#include<libminisip/config/ConfBackend.h>
#include<vector>
using namespace std;

class TlsSrpSocket;
class Certificate;
struct contdata
{   
	char *data;
	int size;
};


class LIBMINISIP_API OnlineConfBack
{
	public:
		OnlineConfBack(string addr, int port, string user, string pass);
		~OnlineConfBack();
		int downloadReq(string user, string type, vector<struct contdata*> &result);
		void uploadReq(string user, string type, string data );
		string base64Encode(char *data, int length);
		string getUser()
		{
			return usrname;
		}
		void setOnlineCert(Certificate *cer);
		Certificate * getOnlineCert();
		string attachFile(string mimeheader, string data);
	private:
		string readHttpHeader();
		string usrname;
		TlsSrpSocket *tls;
		Certificate *cert;
};
#endif
