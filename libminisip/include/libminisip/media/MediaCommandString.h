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

#ifndef MEDIA_COMMAND_STRING_H
#define MEDIA_COMMAND_STRING_H

#include<libminisip/libminisip_config.h>

#include<string>

class LIBMINISIP_API MediaCommandString{
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
/*****************************************************/
		static const std::string start_camera;
		static const std::string start_screen;
		static const std::string stop_camera;
		static const std::string stop_screen;

		/**
		 * Audio forwarding means that the UA acts as a conference
		 * server, and forwards the audio of the participants.
		 */
		static const std::string audio_forwarding_enable;
		static const std::string audio_forwarding_disable;

		static const std::string video_forwarding_enable;
                static const std::string video_forwarding_disable;

		static const std::string send_dtmf;
		
	
};


#endif
