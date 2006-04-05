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

#ifndef _SILENCESENSOR_H
#define _SILENCESENSOR_H

#include<libminisip/libminisip_config.h>

class LIBMINISIP_API SilenceSensor{
	public:
		virtual ~SilenceSensor() {}
		/**
		* 
		* @argument buf        Pointer to the raw audio samples.
		* @argument n          Number of samples in the buffer.
		* @return              True if the audio is in a silence period, and false
		*                      if not.
		*/
		virtual bool silence(uint16_t *buf, int n)=0;
	private:
	
};
	
class LIBMINISIP_API SimpleSilenceSensor : public SilenceSensor{
	public:
		SimpleSilenceSensor();
		virtual bool silence(uint16_t *buf, int n);
	
	private:
		bool inSilence;
		float noiceLevel;
		int limit_on;
		int limit_off;
};

#endif
