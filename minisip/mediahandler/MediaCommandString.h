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

#ifndef MEDIA_COMMAND_STRING_H
#define MEDIA_COMMAND_STRING_H

#include<string>

class MediaCommandString{
	public:
		static const std::string start_ringing;
		static const std::string stop_ringing;
		
		/**
		Used from the console debugger to print debug info 
		about the running media sessions ...
		*/
		static const std::string session_debug;
		
		/**
		Activate the source indicated by "callid" param
		*/
		static const std::string set_session_sound_settings;

		/**
		Tell the media handler to rebuild itself from the new
		configuration
		*/
		static const std::string reload;
		
		
	
};


#endif
