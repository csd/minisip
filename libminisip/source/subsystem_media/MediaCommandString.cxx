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

#include <config.h>

#include<libminisip/media/MediaCommandString.h>

using namespace std;

const string MediaCommandString::start_ringing="start_ringing";
const string MediaCommandString::stop_ringing="stop_ringing";

const string MediaCommandString::session_debug="session_debug";

const string MediaCommandString::set_session_sound_settings="set_session_sound_settings";

const string MediaCommandString::reload="reload";

const string MediaCommandString::audio_forwarding_enable="audio_forwarding_enable";
const string MediaCommandString::audio_forwarding_disable="audio_forwarding_disable";

const string MediaCommandString::video_forwarding_enable="video_forwarding_enable";
const string MediaCommandString::video_forwarding_disable="video_forwarding_disable";

const string MediaCommandString::send_dtmf="send_dtmf";

/********************************************************/
const string MediaCommandString:: start_camera = "start_camera";
const string MediaCommandString:: start_screen="start_screen";
const string MediaCommandString:: stop_camera="stop_camera";
const string MediaCommandString:: stop_screen="stop_screen";

