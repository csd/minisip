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


#include<samplerate.h> //include this in the installation instructions
#include <assert.h>
#include <iostream>
#include "SpAudio.h"
#include<libmutil/MemObject.h>


#define POS 5       //number of positions in the spatial audio scheme


/**
 * Definition of the delay and scaling values depending on the position.
 * 30 samples is the maximum delay chosen, theoretical support for
 * this elecion should be seen in
 */

int32_t SpAudio::lchdelay[POS]={0,0,0,30,0};
int32_t SpAudio::rchdelay[POS]={0,30,0,0,0};

/**
 ** Definition of the matrix with the new position to be assigned depending
 ** on the number of calls being maintained
 **/

int32_t SpAudio::assmatrix[POS][POS]={{3,1,3,2,3},{0,5,1,4,2},{0,0,5,1,4},{0,0,0,5,1},{0,0,0,0,5}};


SpAudio::SpAudio(int32_t numPos){

  nPos=numPos;

}

/*void SpAudio::resample(short *input,
		       short *output,
		       int32_t isize,
		       int32_t osize,
		       SRC_DATA *src_data,
		       SRC_STATE *src_state)
{

  int32_t error;

  src_short_to_float_array (input, src_data->data_in, isize);
  
  src_process(src_state,src_data);
  
  src_float_to_short_array(src_data->data_out,output,osize);
}

*/

int32_t SpAudio::spatialize(short *input,
			    short *leftch,
			    short *rightch,
			    short *lookupleft,
			    short *lookupright,
			    int32_t position,
			    int32_t pointer,
			    short *outbuff)
{
  
  static int32_t j=0,k=0;

  for(int32_t i=0;i<1764;i++){
    if(i%2 == 0){
      leftch[(j+lchdelay[position])%950]=input[i];
      j=(j+1)%950;
    }

    else {
      rightch[(k+rchdelay[position])%950]=input[i];
      k=(k+1)%950;
    }
  }

  for(int32_t i=0; i<882;i++){
    outbuff[(2*i)]=lookupleft[leftch[pointer]+32768];
    outbuff[(2*i)+1]=lookupright[rightch[pointer]+32768];
    pointer=(pointer+1)%950;
  }
  return pointer;
}


int32_t SpAudio::assignPos(int row,
			   int col)
{
  return assmatrix[row][col];
}
