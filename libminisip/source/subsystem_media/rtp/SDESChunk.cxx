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

/* Copyright (C) 2004 
 *
 * Authors: Erik Eliasson <eliasson@it.kth.se>
 *          Johan Bilien <jobi@via.ecp.fr>
*/

#include <config.h>

#include<libminisip/media/rtp/SDESChunk.h>
#include<libminisip/media/rtp/SDESItem.h>

#ifdef DEBUG_OUTPUT
#include<iostream>
#endif

using namespace std;

SDESChunk::SDESChunk(void *build_from, int max_length){
	unsigned int *iptr=(unsigned int *)build_from;
	ssrc_or_csrc = *iptr;
//	sdes_item=SDESItem::build_from(&iptr[1], max_length-4); 

	unsigned char *cptr=(unsigned char *)build_from;
	ssrc_or_csrc = *cptr;
	int start = 4;
	SDESItem *sdes_item;
	while (start<max_length-4-3){				//FIX: check if correct
		sdes_item=SDESItem::build_from(&cptr[start], max_length-start); 
		sdes_items.push_back(sdes_item);
		start+=sdes_item->size();
	}
	
}


int SDESChunk::size(){
	int ret=4;
	for (unsigned i=0; i<sdes_items.size(); i++)
		ret+=sdes_items[i]->size();
	int pad=0;
	if (ret%4 != 0)
		pad = 4-ret%4;
	return ret+pad;
}

#ifdef DEBUG_OUTPUT
void SDESChunk::debug_print(){
	cerr << "SDES chunk: "<< endl;
	cerr << "\tssrc/csrc: "<< ssrc_or_csrc<< endl;
	for (unsigned i=0; i< sdes_items.size(); i++)
		sdes_items[i]->debug_print();

}
#endif
