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

#ifndef CALL_WIDGET_H
#define CALL_WIDGET_H


#include <config.h>
#include <gtkmm.h>
#include<libmutil/CommandString.h>
#include<libmutil/TimeoutProvider.h>
#include"../../Bell.h"

#define CALL_WIDGET_STATE_TERMINATED 	0
#define CALL_WIDGET_STATE_INCALL 	1
#define CALL_WIDGET_STATE_CONNECTING 	2
#define CALL_WIDGET_STATE_RINGING 	3
#define CALL_WIDGET_STATE_INCOMING 	4

class MainWindow;

/* Button that contains a stock icon, but not the associated label */
class StockButton : public Gtk::Button{
	public:
		StockButton( Gtk::StockID, Glib::ustring label );
		
//		void setText( const Glib::ustring& label );
		void 	set_label (const Glib::ustring& label);
	private:
		Gtk::HBox box;
		Gtk::Image image;
		Gtk::Label label;
};

class CallWidget : public Gtk::VBox{
	public:
		CallWidget( string callId, string remoteUri, MainWindow * mw, bool incoming, std::string secure="unprotected");

		~CallWidget();

		void hideAcceptButton();
		bool handleCommand( CommandString command );

		void accept();
		void reject();

		string getCallId();

	private:
		void startRinging();
		void stopRinging();

		MainWindow * mainWindow;
		string callId;

		int32_t state;
		//Gtk::VBox vBox;
		Gtk::Label status;
		Gtk::Label secStatus;
		Gtk::HBox buttonBox;
		StockButton acceptButton;
		StockButton rejectButton;
		//Gtk::Image secIcon;
		//Gtk::HBox secBox;
		MRef<Bell *> bell;
//		TimeoutProvider<string> *timeoutProvider;
};

#endif
