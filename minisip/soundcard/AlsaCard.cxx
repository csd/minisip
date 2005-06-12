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

#include<config.h>
#ifdef HAVE_LIBASOUND

#include"AlsaCard.h"

AlsaCard::AlsaCard( string cardname, string devname ):
		cardName(cardname),
		devName(devname){

}

string AlsaCard::getCardName(){
	return cardName;
}

string AlsaCard::getDevName(){
	return devName;
}


list<AlsaCard *> AlsaCard::getCardList(){
	
	list<AlsaCard *> output = list<AlsaCard *>::list();
	snd_ctl_t *handle;
	int card = -1;
	char name[ 32 ];
	
	snd_ctl_card_info_t *info;
	snd_pcm_info_t *pcminfo;
	snd_ctl_card_info_alloca(&info);
	snd_pcm_info_alloca(&pcminfo);

	while( snd_card_next( &card ) >= 0 && card >= 0 )
	{
		sprintf( name, "hw:%u", card );
		
		if( snd_ctl_open( &handle, name, 0 ) < 0 ){
			cerr << "Could not open card n°" << card << endl;
			continue;
		}

		if( snd_ctl_card_info( handle, info ) < 0 ){
			cerr << "Could not get info regarding card n°" << card << endl;
			continue;
		}

		output.push_back( new AlsaCard( 
				string( snd_ctl_card_info_get_name( info ) ),
				string( snd_ctl_card_info_get_mixername( info )
				      )));
	}

	return output;
}

#endif	
