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

/* Copyright (C) 2004, 2005
 *
 * Authors: Erik Eliasson <eliasson@it.kth.se>
 *          Johan Bilien <jobi@via.ecp.fr>
 *          Ignacio Sanchez Pardo <isp@kth.se>
*/

#include<config.h>
#include"Resampler.h"
#include"SimpleResampler.h"

#ifdef FLOAT_RESAMPLER
#include"FloatResampler.h"
#endif

#include<iostream>

MRef<Resampler *> Resampler::create( uint32_t inputFreq, uint32_t outputFreq,
		                     uint32_t duration, uint32_t nChannels ){

#if (defined FLOAT_RESAMPLER) && (!defined IPAQ)
	return new FloatResampler( inputFreq, outputFreq, duration, nChannels );
#else
	return new SimpleResampler( inputFreq, outputFreq, duration, nChannels );
#endif
}
