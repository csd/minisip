/*
 *  Copyright (C) 2005  Mikael Magnusson
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

/*
 * Authors: Mikael Magnusson <mikma@users.sourceforge.net>
 */

#include"GtkMainUI.h"
#include"MainWindow.h"
#include<libminisip/signaling/conference/ConfMessageRouter.h>

Gtk::Main *GtkMainUI::kit = NULL;

using namespace std;

GtkMainUI::GtkMainUI()
{
}

GtkMainUI *GtkMainUI::create( int argc, char ** argv )
{
#ifdef G_THREADS_ENABLED
	// Required in win32
	if( !Glib::thread_supported() ){
		Glib::thread_init();
	}
#endif
	if( !kit ){
		kit = new Gtk::Main( argc, argv );

	}

	string programDir = Glib::path_get_dirname( argv[0] );

	return new MainWindow( kit, programDir );
}
