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
#ifdef HAVE_CONFIG_H
#include<config.h>
#endif


#include<libmnetutil/NsLookup.h>

#ifdef HAVE_NETDB_H
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#endif

//#ifdef WIN32
//#error NsLookup_not_implemented_for_win32__ee
//#endif

#include<iostream>
using namespace std;

IPAddress *staticLookup(std::string addr){
	std::cerr << "ERROR: NsLookup::staticLookup: UNIMPLEMENTED"<< std::endl;
	exit(1);
}

string NsLookup::staticLookup_str(string addr){
#ifdef HAVE_NETDB_H

        hostent* ret = gethostbyname2(addr.c_str(), AF_INET); //FIXME: Only IPv4 - BUG - do getaddrinfo + change API of netutil
        return string((ret->h_addr_list[0]));
/*
        struct addrinfo *res;
        err = getaddrinfo(addr.c_str(), NULL, NULL, &res);
        
        if (err) {
            cerr << "staticlookup_str: error when doing getaddrinfo: "<< gai_streror(err)<< endl;
//            fprintf(stderr, "error : %s", gai_strerror(err));
            freeaddrinfo(res);
            throw new NsLookupHostNotFound(addr);
            exit(1);
        }
*/        
    
#elif defined WIN32
    cerr << "ERROR: NsLookup::staticLookup_str: UNIMPLEMENTED"<< endl;
    exit(1);

#endif
}


NsLookupHostNotFound::NsLookupHostNotFound(string h):host(h){

}

string NsLookupHostNotFound::errorDescription(){
    return host;
}

