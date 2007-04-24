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

#ifndef WIN32
#include"TrayIcon.h"
#include"MainWindow.h"

#ifdef OLDLIBGLADEMM
#define SLOT(a,b) SigC::slot(a,b)
#define BIND SigC::bind
#else
#define SLOT(a,b) sigc::mem_fun(a,b)
#define BIND sigc::bind
#endif

using namespace std;


MTrayIcon::MTrayIcon( MainWindow * mainWindow, 
		      Glib::RefPtr<Gnome::Glade::Xml>  refXml ){
	this->mainWindow = mainWindow;
	this->refXml = refXml;

	Gtk::MenuItem * quitTrayMenu;
	Gtk::MenuItem * prefTrayMenu;
	

	trayIcon = egg_tray_icon_new( "minisip" );
	image = new Gtk::Image( mainWindow->getDataFileName( "tray_icon.png" ) );

	box.add( *image );
	gtk_container_add( GTK_CONTAINER(trayIcon), GTK_WIDGET(box.gobj()));
	gtk_widget_show_all( GTK_WIDGET( trayIcon ) );

//	button.signal_clicked().connect( SigC::slot( *this, 
//				&MTrayIcon::buttonPressed ) );
	box.signal_button_press_event().connect( SLOT( *this, 
				&MTrayIcon::buttonPressed ) );

	gtk_widget_show_all( GTK_WIDGET( trayIcon ));

	refXml->get_widget( "trayMenu", trayMenu );
	refXml->get_widget( "prefTrayMenu", prefTrayMenu );
	refXml->get_widget( "quitTrayMenu", quitTrayMenu );

	//prefTrayMenu->signal_activate().connect( 
	//	SigC::slot( *mainWindow, &MainWindow::runPref ) );

	quitTrayMenu->signal_activate().connect(
		SLOT( *getWindow(), &Gtk::EventBox::hide ) );
	prefTrayMenu->signal_activate().connect(
		SLOT( *mainWindow, &MainWindow::runPref ) );

}

MTrayIcon::~MTrayIcon(){
	delete image;
	

}

Gtk::Window *  MTrayIcon::getWindow(){
	return Glib::wrap( GTK_WINDOW( trayIcon ) );
}

void MTrayIcon::embedded(){
}

void MTrayIcon::destroy(){
}

bool MTrayIcon::buttonPressed( GdkEventButton * event ){
	switch( event->button ){
		case 3:
			trayMenu->popup( event->button, gtk_get_current_event_time());
			break;
		case 1:
			if( mainWindow->isVisible() ){
				mainWindow->hide();
			}
			else{
				mainWindow->show();
			}
			break;
		default:
			break;
	}
			
	return true;
}
#endif
