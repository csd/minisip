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

#ifndef _DBG_H
#define _DBG_H

#include<string>

#define DBG_INFO 0
#define DBG_ERROR 1

class DbgEndl{
    public:
        DbgEndl(){}
    private:
        int i;
};


class Dbg{
    public:
        static const DbgEndl endl;
	Dbg(bool error_output=false, bool enabled=true);
        Dbg &operator<<(std::string);
        Dbg &operator<<(int);
        Dbg &operator<<(char);
        Dbg &operator<<(DbgEndl &endl);
//        void operator<<(bool);
	void setEnabled(bool enabled);
	bool getEnabled();
    private:
	bool error_out;
	bool enabled;
	std::string str;
};

extern Dbg mout;
extern Dbg merr;
extern Dbg mdbg;
extern DbgEndl end;


#endif

