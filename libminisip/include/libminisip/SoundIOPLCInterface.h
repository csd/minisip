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

/*
 * Purpose
 * 	In VoIP buffer underflows are common. Instead of playing silence or 
 * 	white noice, more advanced Packet Loss Concealment methods may 
 * 	be used. This is the interface used by the SoundCard to request
 * 	such plc generated sound.
 * Author Erik Eliasson, eliasson@it.kth.se
 */

#ifndef SOUNDCARDPLCINTERFACE_H
#define SOUNDCARDPLCINTERFACE_H

/**
 * @author Erik Eliasson
 */
class SoundIOPLCInterface{

	public:
		virtual short *get_plc_sound(uint32_t &ret_size)=0;
		
};


#endif
