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
#include<libminisip/gui/Bell.h>

#include"DtmfWidget.h"

#define CALL_WIDGET_STATE_TERMINATED 	0
#define CALL_WIDGET_STATE_INCALL 	1
#define CALL_WIDGET_STATE_CONNECTING 	2
#define CALL_WIDGET_STATE_RINGING 	3
#define CALL_WIDGET_STATE_INCOMING 	4
#define CALL_WIDGET_STATE_TRANSFER_PENDING	5
#define CALL_WIDGET_STATE_INCOMING_TRANSFER	6

class MainWindow;

/* ToggleButton which toggles its icon as well */
class IconToggleButton : public Gtk::ToggleButton{
	public:
		IconToggleButton( Gtk::StockID, Gtk::StockID );
	private:
		Gtk::Image image1;
		Gtk::Image image2;
		virtual void on_toggled();
};

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

class CallWidget : public Gtk::VBox
			#ifndef OLDLIBGLADEMM
			, virtual public DtmfHandler 
			#endif
{
	public:
		CallWidget(  std::string callId,  std::string remoteUri, MainWindow * mw, bool incoming, std::string secure="unprotected");

		virtual ~CallWidget();

		void hideAcceptButton();
		virtual bool handleCommand( CommandString command );

		void accept();
		void reject();
/***********************************************************************/
		void addCamera();
		void addScreen();
		void cancelCamera();
		void cancelScreen();

		 std::string getMainCallId();
		bool handlesCallId(  std::string callId );
		
		int32_t getState() {return state;}
		
		/**
		This function should be called (from MainWindow::onTabChange) 
		whenever there is a focus change (gained or lost focus). This
		way, the widget is aware of whether it is active or not.
		@param isActive indicates whether this widget got activated
		@param currentActive index of the page/widget being activated
		*/
		virtual void activeWidgetChanged( bool isActive = false, int currentActive = 0 );

		/**
		Event handler for monitoring button ... on toggle()
		*/
		void monitorButtonToggled ();

		/**
		Event handler for audioOutSilence button ... on toggle()
		*/
		void audioOutSilenceButtonToggled ();

		/**
		Event handler for call recording button ... on toggle()
		*/
		void callRecordButtonToggled();
		
	protected:
		void startRinging();
		void stopRinging();

#ifndef OLDLIBGLADEMM
		void transfer();
		virtual void dtmfPressed( uint8_t symbol );
#endif

		MainWindow * mainWindow;

		int32_t state;
		//Gtk::VBox vBox;
		Gtk::Label status;
		Gtk::Label secStatus;

                Gtk::Label sasData;

		Gtk::HBox buttonBox2;
		Gtk::HBox buttonBox;
#ifndef OLDLIBGLADEMM
		Gtk::Expander dtmfArrow;
		Gtk::Expander transferArrow;
		Gtk::HBox transferHBox;
		Gtk::HBox transferHBox2;
		Gtk::Entry transferEntry;
		Gtk::Button transferButton;
//		Gtk::ProgressBar transferProgress;
		Gtk::Label transferProgress;
//		Gtk::CheckButton monitoringButton;
//		Gtk::CheckButton audioOutSilenceButton;
		IconToggleButton monitoringButton;
		IconToggleButton audioOutSilenceButton;
#endif
#ifdef HAVE_LIBGLADEMM_2_6
		IconToggleButton callRecordButton;
#endif

		Gtk::Image secureImage;
		Gtk::Image insecureImage;
		StockButton acceptButton;
		StockButton rejectButton;
		
/****************************************************/
		StockButton addCameraButton;
		StockButton addScreenButton;
		StockButton cancelCameraButton;
		StockButton cancelScreenButton;



		
		//Gtk::Image secIcon;
		//Gtk::HBox secBox;
		MRef<Bell *> bell;
//		TimeoutProvider< std::string> *timeoutProvider;      
		std::list< std::string> callIds;
		std::string mainCallId;
		
		/**
		Indicates whether this callWidget is the one currently
		active (tab selected) or not.
		*/
		bool activeCallWidget;
};

#endif
