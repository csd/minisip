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

/* Copyright (C) 2007
 *
 * Authors: Mikael Magnusson <mikma@users.sourceforge.net>
*/

#include<libmnetutil/init.h>
#include<libmnetutil/DnsNaptr.h>
#include<iostream>
#include<list>
#include<sys/types.h>

using namespace std;

/* 
Test

ISN:
613*262 	Free World Dialup echo test.
2425*259 	Tello "success" message, followed by an echo test.
1234*256 	John's annoying screaming monkeys, followed by an echo test.

./002_naptr sip:skinner.hem.za.org
./002_naptr sips:skinner.hem.za.org
./002_naptr +4687904321
./002_naptr 1234*256

0.0.0.4.0.9.7.8.6.4.e164.arpa -> pbx2.enum.kth.se.
pbx2.enum.kth.se.
pbx2.enum.kth.se has NAPTR record 100 10 "u" "SIP+E2U" "!^\\+468790(.*)$!sip:\\1@sip-ccm.kth.se!" .
pbx2.enum.kth.se has NAPTR record 100 10 "u" "E2U+sip" "!^\\+468790(.*)$!sip:\\1@sip-ccm.kth.se!" .

 */

int main(int argc, char *argv[])
{
	libmnetutilInit();

	if (argc < 2) {
		cerr << "Usage: " << argv[0] << " <e164 number>|<isn>|<sip server url>" << endl;
		return 1;
	}

	string str = argv[1];
	string result;
	bool res;
	MRef<DnsNaptrQuery*> query = DnsNaptrQuery::create();

	if (str.find('+') != string::npos){
		res = query->resolveEnum( str );
	} else if (str.find('*') != string::npos){
		res = query->resolveIsn( str );
	} else if (str.find("sip:") == 0) {
		res = query->resolveSip( str.substr( 4 ) );
	} else if (str.find("sips:") == 0) {
		res = query->resolveSips( str.substr( 5 ) );
	} else {
		cerr << "Unknown " << str << endl;
	}

	cerr << "lookup: " << res << " " << query->getResult() << " '" << query->getService() << "'" << endl;

	return 1;
}
