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


#ifndef SPATIAL_AUDIO_H
#define SPATIAL_AUDIO_H

#include<samplerate.h>
#include<libmutil/MemObject.h>


#define POS 5
#define MAXSOURCES 10

class SoundSource;

class SpAudio{

 public:

  SpAudio(int32_t numPos);
  

  int32_t getNumPos();

  int32_t spatialize (short *input,

		      MRef<SoundSource *> src,
		      short *outbuff);


  int32_t assignPos(int row,
		    int col);

  static int32_t lchdelay[POS];
  static int32_t rchdelay[POS];

  static int32_t assmatrix[MAXSOURCES][MAXSOURCES];

 private:

  int32_t nPos;
 
};

#endif
