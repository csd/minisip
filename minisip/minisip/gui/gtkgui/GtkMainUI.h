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

#ifndef GTK_MAIN_UI_H
#define GTK_MAIN_UI_H

#include <libminisip/gui/Gui.h>
#include <libmutil/MemObject.h>
#include <gtkmm.h>

class GtkMainUI : public Gui, public LogEntryHandler, public DbgHandler
{
	public:
		static GtkMainUI *create( int argc, char ** argv );

	protected:
		GtkMainUI();

	private:
		static Gtk::Main *kit;
};

#endif	
