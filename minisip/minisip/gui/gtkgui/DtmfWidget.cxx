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

#include"DtmfWidget.h"


#ifdef OLDLIBGLADEMM
#define SLOT(a,b) SigC::slot(a,b)
#define BIND SigC::bind
#else
#define SLOT(a,b) sigc::mem_fun(a,b)
#define BIND sigc::bind
#endif

using namespace std;

DtmfWidget::DtmfWidget():
		Gtk::Table( 4, 3, true ),
		oneButton( "1" ),
		twoButton( "2" ),
		threeButton( "3" ),
		fourButton( "4" ),
		fiveButton( "5" ),
		sixButton( "6" ),
		sevenButton( "7" ),
		eightButton( "8" ),
		nineButton( "9" ),
		zeroButton( "0" ),
		sharpButton( "#" ),
		starButton( "*" ){
	
	attach( oneButton, 0, 1, 0, 1 );
	attach( twoButton, 1, 2, 0, 1 );
	attach( threeButton, 2, 3, 0, 1 );
	
	attach( fourButton, 0, 1, 1, 2 );
	attach( fiveButton, 1, 2, 1, 2 );
	attach( sixButton, 2, 3, 1, 2 );
	
	attach( sevenButton, 0, 1, 2, 3 );
	attach( eightButton, 1, 2, 2, 3 );
	attach( nineButton, 2, 3, 2, 3 );
	
	attach( starButton, 0, 1, 3, 4 );
	attach( zeroButton, 1, 2, 3, 4 );
	attach( sharpButton, 2, 3, 3, 4 );

	show_all();
}

void DtmfWidget::setHandler( DtmfHandler * handler ){
#ifndef OLDLIBGLADEMM
	oneButton.signal_clicked().connect(
			BIND<uint8_t>(
			SLOT( *handler, &DtmfHandler::dtmfPressed ),
			1 ));
	
	twoButton.signal_clicked().connect(
			BIND<uint8_t>(
			SLOT( *handler, &DtmfHandler::dtmfPressed ),
			2 ));
	
	threeButton.signal_clicked().connect(
		BIND<uint8_t>(
		SLOT( *handler, &DtmfHandler::dtmfPressed ),
		3 ));
	
	fourButton.signal_clicked().connect(
		BIND<uint8_t>(
		SLOT( *handler, &DtmfHandler::dtmfPressed ),
		4 ));
	
	fiveButton.signal_clicked().connect(
		BIND<uint8_t>(
		SLOT( *handler, &DtmfHandler::dtmfPressed ),
		5 ));
	
	sixButton.signal_clicked().connect(
		BIND<uint8_t>(
		SLOT( *handler, &DtmfHandler::dtmfPressed ),
		6 ));
	
	sevenButton.signal_clicked().connect(
		BIND<uint8_t>(
		SLOT( *handler, &DtmfHandler::dtmfPressed ),
		7 ));
	
	eightButton.signal_clicked().connect(
		BIND<uint8_t>(
		SLOT( *handler, &DtmfHandler::dtmfPressed ),
		8 ));
	
	nineButton.signal_clicked().connect(
		BIND<uint8_t>(
		SLOT( *handler, &DtmfHandler::dtmfPressed ),
		9 ));
	
	zeroButton.signal_clicked().connect(
		BIND<uint8_t>(
		SLOT( *handler, &DtmfHandler::dtmfPressed ),
		0 ));
	
	starButton.signal_clicked().connect(
		BIND<uint8_t>(
		SLOT( *handler, &DtmfHandler::dtmfPressed ),
		10 ));
	
	sharpButton.signal_clicked().connect(
		BIND<uint8_t>(
		SLOT( *handler, &DtmfHandler::dtmfPressed ),
		11 ));
#endif
}


