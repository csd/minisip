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

#ifndef SOUNDRECORDERCALLBACK_H
#define SOUNDRECORDERCALLBACK_H

#include<libminisip/libminisip_config.h>

#include<sys/types.h>

/**
 * Callback interface for a receiver of sound data from the
 * soundcard.
 * @author Erik Eliasson, eliasson@it.kth.se
 * @version 0.01
 */
class LIBMINISIP_API SoundRecorderCallback{
	public:
		virtual ~SoundRecorderCallback() {}
		
		/**
		* Function that will be called when sound data is available from
		* the soundcard.
		* @param samplearr Array of raw sound data. Typically 160 
		* samples, 16 bit, but it can be specified in SoundCard.
		* @param length length of samplearr
		* @see SoundCard
		*/
		virtual void srcb_handleSound(void *samplearr, int length, int sample_freq)=0;
		#ifdef AEC_SUPPORT
		virtual void srcb_handleSound(void *samplearr, int length, void *samplearrR)=0;		//hanning
		#endif
	//	virtual uint32_t srcb_getSSRC()=0;
};


#endif
