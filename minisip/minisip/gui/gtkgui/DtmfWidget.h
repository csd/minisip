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

#ifndef DTMF_WIDGET_H
#define DTMF_WIDGET_H

#include<gtkmm.h>


class DtmfHandler{
	public:
		virtual ~DtmfHandler() {}
		virtual void dtmfPressed( uint8_t symbol )=0;
};


class DtmfWidget : public Gtk::Table{
	public:
		DtmfWidget();

		void setHandler( DtmfHandler * handler );
	private:
		void buttonPressed();

		Gtk::Button oneButton;
		Gtk::Button twoButton;
		Gtk::Button threeButton;
		Gtk::Button fourButton;
		Gtk::Button fiveButton;
		Gtk::Button sixButton;
		Gtk::Button sevenButton;
		Gtk::Button eightButton;
		Gtk::Button nineButton;
		Gtk::Button zeroButton;
		Gtk::Button sharpButton;
		Gtk::Button starButton;
};

#endif
