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

#ifndef _NSLOOKUP_H
#define _NSLOOKUP_H

//#include<hash_map>

#include<libmnetutil/NetworkException.h>
#include<libmnetutil/IPAddress.h>


class NsLookup{
    public:
//        NsLookup(int maxCacheTimeout=900);
//        IPAddress *lookup(string addr);
//        string lookup_str(string addr);
        
        static IPAddress *staticLookup(std::string addr);
        static std::string staticLookup_str(std::string addr);
//        void clearCache();
    private:
//        hash_map<string, string> cache;
};

class NsLookupHostNotFound : public NetworkException{
    public:
        NsLookupHostNotFound(std::string host);
        
        virtual std::string errorDescription();
    private:
	std::string host;
};

#endif
