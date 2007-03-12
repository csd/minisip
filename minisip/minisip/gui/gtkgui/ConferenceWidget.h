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
#include<libminisip/signaling/conference/ConferenceControl.h>
#include<libminisip/gui/Bell.h>
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
		ConferenceWidget( std::string configUri,  std::string confId,  std::string users, std::string remoteUri, std::string callId, MainWindow * mw, bool incoming);

		~ConferenceWidget();

		void hideAcceptButton();
		bool handleCommand( CommandString command );

		void accept();
		void reject();
		void add();

		 std::string getMainCallId();
		 std::string getMainConfId();
                bool handlesConfId(  std::string confId );

		int32_t getState() { return state;  }
		
		/**
		This function should be called (from MainWindow::onTabChange) 
		whenever there is a focus change (gained or lost focus). This
		way, the widget is aware of whether it is active or not.
		@param isActive indicates whether this widget got activated
		@param currentActive index of the page/widget being activated
		*/
		virtual void activeWidgetChanged( bool isActive = false, int currentActive = 0 );

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
		std::list<std::string> confIds;
		std::string mainCallId;
		std::string mainConfId;
		std::string initiatorUri;
		ConferenceControl* conf;
		
		/**
		Indicates whether this callWidget is the one currently
		active (tab selected) or not.
		*/
		bool activeCallWidget;
		
};

#endif
