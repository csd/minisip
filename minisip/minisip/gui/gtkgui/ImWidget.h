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

#include<string>

class ImMessageTextView;
class MainWindow;

class ImWidget : public Gtk::VBox{
	public:
		ImWidget( MainWindow * mainWindow, std::string toUri, std::string fromUri );
		~ImWidget();

		bool handleIm( std::string message, std::string from);
		void send( std::string message );

		std::string getToUri(){ return toUri; };

		/**
		This function should be called (from MainWindow::onTabChange) 
		whenever there is a focus change (gained or lost focus). This
		way, the widget is aware of whether it is active or not.
		@param isActive indicates whether this widget got activated
		@param currentActive index of the page/widget being activated
		*/
		virtual void activeWidgetChanged( bool isActive = false, int currentActive = 0 );
	
	private:


		Gtk::ScrolledWindow * historyWindow;
		Gtk::ScrolledWindow * messageWindow;

		Gtk::TextView * historyView;
		ImMessageTextView * messageView;

		Gtk::Label * messageLabel;
		Gtk::TextBuffer::iterator historyIter;

		Gtk::HBox * buttonBox2;
		Gtk::HBox * buttonBox;
		Gtk::Button * closeButton;

		std::string toUri;
		std::string fromUri;
		MainWindow * mainWindow;

		/**
		Indicates whether this callWidget is the one currently
		active (tab selected) or not.
		*/
		bool activeCallWidget;

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
		ImEntry( std::string callId );
	
	private:
		std::string callId;
		Gtk::TextBuffer::iterator startIter;
		Gtk::TextBuffer::iterator endIter;
};

#endif
