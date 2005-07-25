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

#ifndef LOG_WIDGET_H
#define LOG_WIDGET_H

#include<config.h>

#include<gtkmm.h>
#include<libmutil/MemObject.h>

class LogEntry;
class MainWindow;
class ContactDb;



class LogWidget: public Gtk::TreeView{
	public:
		LogWidget( MainWindow * mainWindow );
		~LogWidget();

		void addLogEntry( MRef<LogEntry *> );

		void setContactDb( MRef<ContactDb *> );

	private:
		//list< MRref<LogEntry *> > entries;
		MainWindow * mainWindow;

		Gtk::TreeModelColumn<Glib::ustring> startColumn;
		Gtk::TreeModelColumn<Glib::RefPtr<Gdk::Pixbuf> > typeColumn;
		Gtk::TreeModelColumn<Glib::ustring> uriColumn;
		Gtk::TreeModelColumn<Glib::ustring> statusColumn;
		Gtk::TreeModelColumn<MRef<LogEntry *> > entry;
		Gtk::TreeModel::ColumnRecord columns;

		Gtk::CellRendererPixbuf * iconRenderer;

		Glib::RefPtr<Gtk::ListStore> listStore;

		bool lineSelect( const Glib::RefPtr<Gtk::TreeModel>& model,
				 const Gtk::TreeModel::Path& path, bool );

		void setFont( Gtk::CellRenderer * renderer,
			      const Gtk::TreeModel::iterator & iter );

		MRef<ContactDb *> contactDb;

};


#endif
