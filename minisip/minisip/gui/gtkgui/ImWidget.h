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

#ifndef IM_WIDGET_H
#define IM_WIDGET_H

#include<config.h>
#include<gtkmm.h>

class ImMessageTextView;
class MainWindow;

class ImWidget : public Gtk::VBox{
	public:
		ImWidget( MainWindow * mainWindow, string toUri, string fromUri );
		~ImWidget();

		bool handleIm( string message, string from);
		void send( string message );

		string getToUri(){ return toUri; };

	private:


		Gtk::ScrolledWindow * historyWindow;
		Gtk::ScrolledWindow * messageWindow;

		Gtk::TextView * historyView;
		ImMessageTextView * messageView;

		Gtk::Label * messageLabel;
		Gtk::TextBuffer::iterator historyIter;

		Gtk::HBox * buttonBox;
		Gtk::Button * closeButton;

		string toUri;
		string fromUri;
		MainWindow * mainWindow;

};

class ImMessageTextView : public Gtk::TextView{
	public:
		ImMessageTextView( ImWidget * );
		virtual bool on_key_press_event( GdkEventKey * event );
	private:
		ImWidget * imWidget;
};

class ImEntry {
	public:
		ImEntry( string callId );
	
	private:
		string callId;
		Gtk::TextBuffer::iterator startIter;
		Gtk::TextBuffer::iterator endIter;
};





#endif
