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

#include<assert.h>
#include"SDES_PHONE.h"

#ifdef DEBUG_OUTPUT
#include<iostream>
#endif

SDES_PHONE::SDES_PHONE(void *buildfrom, int max_length){
	unsigned char *lengthptr = (unsigned char *)buildfrom;
	lengthptr++;
	length=*lengthptr;
	
	char *cptr = (char *)buildfrom;
	assert(*cptr == PHONE);
//	type=*cptr;
	phone="";
	
	cptr+=2;
	for (int i=0 ; i<*lengthptr; i++){
		phone+=*cptr;
		cptr++;
	}	
#ifdef DEBUG_OUTPUT
	cerr << "In SDES_PHONE: Parsed string name to <"<< phone << ">"<< endl;
#endif
}

int SDES_PHONE::size(){
//	cerr << "WARNING: returning unaligned size - FIX"<< endl;
//	int npad = 4-((2+cname.length())%4);
//	if (npad==4)
//		npad=0;
	return 2+phone.length()/*+npad*/;
}

#ifdef DEBUG_OUTPUT
void SDES_PHONE::debug_print(){
	cerr << "SDES PHONE:"<< endl;
	cerr << "\tlength: "<< length << endl;
	cerr << "\tname: "<< phone << endl;
}
#endif
