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

#include<config.h>

#include<libminisip/media/spaudio/SpAudio.h>

#include<string.h> //for memset

#include<libminisip/media/soundcard/SoundSource.h>

SpAudio::SpAudio(int32_t numPos){
	nPos=numPos;
}


void SpAudio::init(){
	
	int32_t lchdelay_[SPATIAL_POS]={0,0,0,36,0};
	memcpy( lchdelay, lchdelay_, SPATIAL_POS * sizeof(int32_t) );
	
	int32_t rchdelay_[SPATIAL_POS]={0,36,0,0,0};
	memcpy( rchdelay, rchdelay_, SPATIAL_POS * sizeof(int32_t) );
	
	int32_t assmatrix_[SPATIAL_MAXSOURCES][SPATIAL_MAXSOURCES]={
								{3,1,1,1,1,1,1,1,1,1},
								{0,5,3,2,2,2,2,2,2,2},
								{0,0,5,4,3,3,3,3,3,3},
								{0,0,0,5,4,4,4,4,4,4},
								{0,0,0,0,5,5,5,5,5,5},
								{0,0,0,0,0,3,1,1,1,1},
								{0,0,0,0,0,0,5,3,2,2},
								{0,0,0,0,0,0,0,5,4,3},
								{0,0,0,0,0,0,0,0,5,4},
								{0,0,0,0,0,0,0,0,0,5}
								};
	memcpy( assmatrix, assmatrix_, SPATIAL_MAXSOURCES * SPATIAL_MAXSOURCES * sizeof(int32_t) );
	
	/* lookup tables without gain control */
	float lchvol[SPATIAL_POS]={	(float)1,
								(float)0.8,
								(float)1,
								(float)0.6,
								(float)0};
	float rchvol[SPATIAL_POS]={	(float)0,
								(float)0.6,
								(float)1,
								(float)0.8,
								(float)1};
	
	/* lookup tables for gain control
	float lchvol[SPATIAL_POS][SPATIAL_POS]={{1,0.8,1,0.6,0},{1,0,0,0,0},{0.5,0,0.5,0,0},{0.5,0.5,0,0.3,0},{0.5,0.5,0.5,0.3,0}};
	float rchvol[SPATIAL_POS][SPATIAL_POS]={{0,0.6,1,0.8,1},{0,0,0,0,1},{0,0,0.5,0,0.5},{0,0.3,0,0.5,0.5},{0,0.3,0.5,0.5,0.5}};
	*/
	
	for(int32_t j=0; j < SPATIAL_POS; j++ ){
		for(int32_t i=0;i<65536;i++){
			/* without gain control */
			lookupleftGlobal[i][j]=(short)((float)(i-32768)*lchvol[j]);
			lookuprightGlobal[i][j]=(short)((float)(i-32768)*rchvol[j]);

			/* with gain control 
			   lookupleft[i]=(short)((float)(i-32768)*lchvol[nSources-1][position-1]);
			   lookupright[i]=(short)((float)(i-32768)*rchvol[nSources-1][position-1]);
			   */
		} 
	}
}


int32_t SpAudio::spatialize (short *input,
			MRef<SoundSource *> src,
			short *outbuff
			)  {
	int nSamples = (SOUND_CARD_FREQ * 20) / 1000;
	if( src->leftch && src->rightch ){
		int32_t i;
		for(i=0;i<nSamples*2;i++){
			if(i%2 == 0){
				src->leftch [(src->j + lchdelay[src->position-1]) % 1028] = input[i];
				src->j = (src->j + 1 ) % 1028;
			}

			else {
				src->rightch [(src->k + rchdelay[src->position-1]) % 1028 ] = input[i];
				src->k = (src->k + 1) % 1028;
			}
		}

		for(i=0; i<nSamples;i++){
			outbuff[(2*i)] = 
				lookupleftGlobal   [src->leftch[ src->pointer ]+32768]   [src->position-1];
			outbuff[(2*i)+1] = 
				lookuprightGlobal  [src->rightch[ src->pointer ]+32768]  [src->position -1];
			src->pointer = (src->pointer + 1) % 1028;
		}
		return src->pointer;
	}
	else{
		memcpy( outbuff, input, nSamples * 2 );
		return 0;
	}
}

int32_t SpAudio::assignPos(int row,
			   int col)
{
	return assmatrix[row-1][col-1];
}
