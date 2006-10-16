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


#ifndef SPATIAL_AUDIO_H
#define SPATIAL_AUDIO_H

#include<libminisip/libminisip_config.h>

#include<libmutil/MemObject.h>

//number of positions in the spatial audio scheme
#define SPATIAL_POS 5  

//Max number of sources for spatial audio
#define SPATIAL_MAXSOURCES 10

class SoundSource;

class LIBMINISIP_API SpAudio: public MObject{

	public:

		/**
		To use the SpAudio Object, you need to create it ... 
		and call the init() function.
		*/
		SpAudio(int32_t numPos);
		
		void init();

		virtual std::string getMemObjectType() const {return "SpAudio";}
		int32_t getNumPos();
		
		int32_t spatialize (short *input,
				MRef<SoundSource *> src,
				short *outbuff);
		
		/**
		Row is the position in the sources list: 1, 2, 3, 4 ...
		Col is the new number of sources (thus, 0 <= row-1 <= col-1 )
		It goes like: 
		the size of the source list gives you the column to use. Then,
		for each source in the list, its position in the list gives you
		the row.
		Currently, for five sources max, positions are like:
			3 - center, front
			1 - one ear, side (left)
			5 - one ear, the other side (right)
			2 - front side, between 1 and 3
			4 - front other side, between 3 and 5
		*/
		int32_t assignPos(int row,
				int col);

		//All these 2D arrays should be dynamically created ...
		//... this way, creating an SpAudio object would not 
		//mean using up all this memory (until init() ).
		/**
		* Definition of the delay values depending on the position.
		* Left channel
		*/
		int32_t lchdelay[SPATIAL_POS];
		/**
		* Definition of the delay values depending on the position.
		* Right channel
		*/
		int32_t rchdelay[SPATIAL_POS];
		
		/**
		* Definition of the matrix with the new position to be assigned depending
		* on the number of calls being maintained
		* FIXME: This needs a lot of improvement ... some kind of memory is needed,
		* so sources dont move around like crazy.
		*/
		int32_t assmatrix[SPATIAL_MAXSOURCES][SPATIAL_MAXSOURCES];

		short int lookupleftGlobal[65536][SPATIAL_POS]; 
		short int lookuprightGlobal[65536][SPATIAL_POS]; 

	private:

		int32_t nPos;
 
};

#endif
