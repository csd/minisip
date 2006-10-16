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

#ifndef TRAY_ICON_H
#define TRAY_ICON_H

#include<config.h>
#include<libmutil/MemObject.h>
#include<gtkmm.h>
#include<libglademm/xml.h>

#include"eggtrayicon.h"

class MainWindow;

class MTrayIcon: public MObject
#ifdef OLDLIBGLADEMM
, public SigC::Object
#endif
{
	public:
		MTrayIcon( MainWindow * mainWindow, 
			   Glib::RefPtr<Gnome::Glade::Xml>  refXml );
		~MTrayIcon();

		virtual std::string getMemObjectType() const {return "MTrayIcon";}

		Gtk::Window * getWindow();
	private:
		void embedded();
		void destroy();
		bool buttonPressed( GdkEventButton* );

		MainWindow * mainWindow;
		Glib::RefPtr<Gnome::Glade::Xml>  refXml;
		
		EggTrayIcon * trayIcon;
		Gtk::EventBox box;
		//Gtk::Button button;
		Gtk::Image * image;

		Gtk::Menu * trayMenu;
		
};



#endif
