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

#include"ImWidget.h"

#include"MainWindow.h"
#include<libmsip/SipCommandString.h>

#ifdef OLDLIBGLADEMM
#define SLOT(a,b) SigC::slot(a,b)
#define BIND SigC::bind
#else
#define SLOT(a,b) sigc::mem_fun(a,b)
#define BIND sigc::bind
#endif

using namespace std;

ImWidget::ImWidget( MainWindow * mainWindow, string toUri, string fromUri ){
	this->mainWindow = mainWindow;
	this->toUri = toUri;
	this->fromUri = fromUri;
	
	this->activeCallWidget = false;
	
	historyWindow = new Gtk::ScrolledWindow();
	messageWindow = new Gtk::ScrolledWindow();
	historyView = new Gtk::TextView();
	messageView = new ImMessageTextView( this );
	messageLabel = new Gtk::Label( "New message:" );
	closeButton = new Gtk::Button( "Close" );
	buttonBox2 = new Gtk::HBox();
	buttonBox = new Gtk::HBox();

	historyView->set_wrap_mode( Gtk::WRAP_WORD );
	messageView->set_wrap_mode( Gtk::WRAP_WORD );

	historyIter = historyView->get_buffer()->begin();

	pack_start( *historyWindow, true, true );
	pack_start( *messageLabel, false, true );
	pack_start( *messageWindow, false, true );
	pack_end( *buttonBox, false, true );
	pack_end( *buttonBox2, false, true );

	buttonBox->pack_end( *closeButton, false, true );

	closeButton->signal_clicked().connect( BIND<string>( 
		SLOT( *mainWindow, &MainWindow::removeIm ), toUri ) );
	

	historyWindow->add( *historyView );
	messageWindow->add( *messageView );

	historyWindow->set_shadow_type( Gtk::SHADOW_OUT );
	messageWindow->set_shadow_type( Gtk::SHADOW_OUT );

	historyWindow->set_policy( Gtk::POLICY_NEVER, Gtk::POLICY_ALWAYS );
	messageWindow->set_policy( Gtk::POLICY_NEVER, Gtk::POLICY_AUTOMATIC );

	historyView->set_editable( false );

//	messageView->signal_key_press_event().connect( SigC::slot( *this, &ImWidget::send ) );


	show_all();


	Glib::RefPtr<Gtk::TextTag> fromTag = 
		historyView->get_buffer()->create_tag( "from" );
	fromTag->property_foreground().set_value( "Red" );
	
	Glib::RefPtr<Gtk::TextTag> processingTag = 
		historyView->get_buffer()->create_tag( "processing" );
	processingTag->property_foreground().set_value( "Black" );
	processingTag->property_weight().set_value( Pango::WEIGHT_ULTRALIGHT );
	
	//handleIm( "hello world", "HAL" );

	
}

ImWidget::~ImWidget(){
	delete historyView;
	delete messageView;
	delete historyWindow;
	delete messageWindow;
	delete closeButton;
	delete buttonBox2;
	delete buttonBox;
	delete messageLabel;
}
	


bool ImWidget::handleIm( string message, string from){
	// FIXME 
	if( from != toUri && from != "Me" ){
		return false;
	}
	
	historyIter = historyView->get_buffer()->
		insert_with_tag( historyIter, from + ": ", "from" );
	historyIter = historyView->get_buffer()->
		insert_with_tag( historyIter, message + "\n", "processing" );

	return true;

}

void ImWidget::send( string message ){
	handleIm( message, "Me" );
	CommandString cmd("",SipCommandString::outgoing_im,message,toUri, fromUri);
	mainWindow->getCallback()->handleCommand("sip", cmd );
	
}

void ImWidget::activeWidgetChanged( bool isActive, int currentActive ) {

	if( isActive == activeCallWidget ) {
		#ifdef DEBUG_OUTPUT
// 		cerr << "ImWidget::activeCall - nothing to do here (no active state change)" << endl;
		#endif
		return;
	} else {
		activeCallWidget = isActive;
	}

	//our status has changed ... do something?
	if( !isActive ) {
		#ifdef DEBUG_OUTPUT
// 		cerr << "ImWidget::activeCall - We were active ... not anymore" << endl;
		#endif
		return;
	} else {
		#ifdef DEBUG_OUTPUT
// 		cerr << "ImWidget::activeCall - We active!" << endl;
		#endif
	}
}

ImMessageTextView::ImMessageTextView( ImWidget * imWidget ){
	this->imWidget = imWidget;
}

bool ImMessageTextView::on_key_press_event( GdkEventKey * event ){
	if( event->keyval == GDK_Return ) {
		//if the CTRL_key is pressed, then we do not send, just make a return on the text,
		//like other IM programs allow.
		if( ( (event->state >> 2) & 0x1 ) == 0){ 
			imWidget->send( get_buffer()->get_text() );
			get_buffer()->erase( get_buffer()->begin(), get_buffer()->end() );
			return true;
		}
	}
	return Gtk::TextView::on_key_press_event( event );
}
