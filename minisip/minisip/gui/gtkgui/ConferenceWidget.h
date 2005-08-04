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

#ifndef CONFERENCE_WIDGET_H
#define CONFERENCE_WIDGET_H


#include <config.h>
#include <gtkmm.h>
#include<libmutil/CommandString.h>
#include "../../../conf/ConferenceControl.h"
#include"../../Bell.h"
#include"CallWidget.h"
#include"DtmfWidget.h"

#define CONFERENCE_WIDGET_STATE_TERMINATED 	0
#define CONFERENCE_WIDGET_STATE_INCALL 	1
#define CONFERENCE_WIDGET_STATE_CREATED 	2
#define CONFERENCE_WIDGET_STATE_RINGING 	3
#define CONFERENCE_WIDGET_STATE_INCOMING 	4
#define CONFERENCE_WIDGET_STATE_TRANSFER_PENDING	5
#define CONFERENCE_WIDGET_STATE_INCOMING_TRANSFER	6

class MainWindow;

/* Button that contains a stock icon, but not the associated label */
/*class StockButton : public Gtk::Button{
	public:
		StockButton( Gtk::StockID, Glib::ustring label );
		
//		void setText( const Glib::ustring& label );
		void 	set_label (const Glib::ustring& label);
	private:
		Gtk::HBox box;
		Gtk::Image image;
		Gtk::Label label;
};*/

class ConferenceWidget : public Gtk::VBox
{
	public:
		ConferenceWidget(string configUri, string confId, string users,string remoteUri,string callId, MainWindow * mw, bool incoming);

		~ConferenceWidget();

		void hideAcceptButton();
		bool handleCommand( CommandString command );

		void accept();
		void reject();
		void add();

		string getMainCallId();
		string getMainConfId();
                bool handlesConfId( string confId );

	private:
		void startRinging();
		void stopRinging();

		MainWindow * mainWindow;

		int32_t state;
		//Gtk::VBox vBox;
		Gtk::Label status;
		Gtk::Label secStatus;
		Gtk::HBox buttonBox;
                Gtk::HBox conferenceHBox;
                Gtk::HBox conferenceHBox2;
		Gtk::Entry conferenceEntry;
                Gtk::Button conferenceButton;
		Gtk::Image secureImage;
                Gtk::Image insecureImage;
		StockButton acceptButton;
		StockButton rejectButton;
		//Gtk::Image secIcon;
		//Gtk::HBox secBox;
		MRef<Bell *> bell;
//		TimeoutProvider<string> *timeoutProvider;      
                list<string> confIds;
                string mainCallId;
		string mainConfId;
		string initiatorUri;
		ConferenceControl* conf;
};

#endif
