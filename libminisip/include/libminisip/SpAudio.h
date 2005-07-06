/*
  Copyright (C) 2005, 2004 Erik Eliasson, Johan Bilien
  
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
  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/


/* Copyright (C) 2004 
 *
 * Authors: Erik Eliasson <eliasson@it.kth.se>
 *          Johan Bilien <jobi@via.ecp.fr>
*/


#ifndef SPATIAL_AUDIO_H
#define SPATIAL_AUDIO_H

#ifdef _MSC_VER
#ifdef LIBMINISIP_EXPORTS
#define LIBMINISIP_API __declspec(dllexport)
#else
#define LIBMINISIP_API __declspec(dllimport)
#endif
#else
#define LIBMINISIP_API
#endif


#include<libmutil/MemObject.h>

#ifdef _MSC_VER
#ifndef int32_t
typedef __int32  int32_t;
#endif
#else
#include<stdint.h>
#endif

#define POS 5
#define MAXSOURCES 10

class SoundSource;

class LIBMINISIP_API SpAudio{

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
