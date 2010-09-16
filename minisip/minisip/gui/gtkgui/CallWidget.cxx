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

/* Copyright (C) 2004, 2005 
 *
 * Authors: Erik Eliasson <eliasson@it.kth.se>
 *          Johan Bilien <jobi@via.ecp.fr>
*/

#include"CallWidget.h"
#include"MainWindow.h"
//#include<libminisip/sip/state_machines/SipSMCommand.h>
//#include<libminisip/sip/state_machines/SipSoftPhone.h>
#include<libmsip/SipSMCommand.h>
#include<libmsip/SipCommandString.h>
#include<libminisip/gui/Bell.h>
#include<libminisip/media/MediaCommandString.h>
//#include<libminisip/media/Session.h>
//#include<libminisip/media/MediaStream.h>

#ifdef OLDLIBGLADEMM
#define SLOT(a,b) SigC::slot(a,b)
#define BIND SigC::bind
#else
#define SLOT(a,b) sigc::mem_fun(a,b)
#define BIND sigc::bind
#endif

using namespace std;

CallWidget::CallWidget( string callId, string remoteUri, 
                        MainWindow * mw, bool incoming, string secure):
		mainWindow( mw ),
		status( "", Gtk::ALIGN_LEFT ),
		secStatus( "", Gtk::ALIGN_LEFT ),
		buttonBox2(/*homogenius*/ true ),
		buttonBox(/*homogenius*/ true ),
#ifndef OLDLIBGLADEMM
		dtmfArrow( "Dialpad" ),
		transferArrow( "Call transfer" ),
		transferHBox( false ),
		transferButton( "Transfer" ),
//		monitoringButton( "Mute microphone" ),
//		audioOutSilenceButton( "Silence for my ears"),
		monitoringButton( Gtk::StockID( "minisip_record" ), Gtk::StockID( "minisip_norecord" ) ),
		audioOutSilenceButton( Gtk::StockID( "minisip_play" ), Gtk::StockID( "minisip_noplay" ) ),
#endif
#ifdef HAVE_LIBGLADEMM_2_6
		callRecordButton( Gtk::Stock::MEDIA_RECORD, Gtk::Stock::MEDIA_RECORD ),
#endif
		secureImage(),// Gtk::StockID( "minisip_insecure" ), 
					//   Gtk::ICON_SIZE_DIALOG ),
		insecureImage( Gtk::StockID( "minisip_insecure" ), 
		Gtk::ICON_SIZE_DIALOG ),
		acceptButton( Gtk::Stock::OK, "Accept" ),
		rejectButton( Gtk::Stock::CANCEL, "Reject" ),
/***************************************************************************/
		addCameraButton(Gtk::Stock::ADD,"Add Camera"),
		addScreenButton(Gtk::Stock::ADD,"Share Screen"),
		cancelCameraButton(Gtk::Stock::CANCEL,"Stop additional camera"),
		cancelScreenButton(Gtk::Stock::CANCEL,"Stop screen sharing"),		



		bell(),
		mainCallId( callId )
{

	activeCallWidget = false;

	bell = NULL;
	callIds.push_back( callId );


	Gtk::HBox * topBox = manage( new Gtk::HBox );

	topBox->pack_start( secureImage, false, false, 5 );

	Gtk::VBox * rightTopBox = manage( new Gtk::VBox );
	topBox->pack_start( *rightTopBox, false, false, 5 );

	
	rightTopBox->pack_start( status, false, false, 5 );
	rightTopBox->pack_start( secStatus, false, false, 5 );
        rightTopBox->pack_start( sasData, false, false, 5 );
	pack_start( *topBox, false, false, 5 );

	Pango::AttrList attrList( "<big><b></b></big>" );
	status.set_attributes( attrList );

	Gtk::HSeparator * separator = manage( new Gtk::HSeparator );
	pack_start( *separator, false, false, 5 );
	separator->show();

//	add( status );
//	add( secStatus );
	pack_end( buttonBox2, false, true );
	pack_end( buttonBox, false, true );

	//buttonBox.set_expand( false );

	status.set_use_markup( true );
	secStatus.set_use_markup( true );
	topBox->show_all();

#ifndef OLDLIBGLADEMM
	topBox = manage( new Gtk::HBox );
	topBox->pack_start( monitoringButton, false, false );
	monitoringButton.signal_toggled().connect( 
		SLOT( *this, 
			&CallWidget::monitorButtonToggled ) );
			
	topBox->pack_start( audioOutSilenceButton, false, false );
	audioOutSilenceButton.signal_toggled().connect( 
		SLOT( *this, 
			&CallWidget::audioOutSilenceButtonToggled ) );
	
#ifdef HAVE_LIBGLADEMM_2_6
	topBox->pack_start( callRecordButton, false, false );
	callRecordButton.signal_toggled().connect( 
		SLOT( *this, 
			&CallWidget::callRecordButtonToggled ) );
#endif
	pack_start( *topBox, false, false, 4 );
	
	DtmfWidget * dtmfWidget = manage( new DtmfWidget() );
	dtmfWidget->setHandler( this );
	dtmfArrow.add( *dtmfWidget ); 
	pack_start( dtmfArrow, false, false, 4 );
	
	Gtk::VBox * vbox = manage( new Gtk::VBox );
	vbox->add( transferHBox );
	vbox->add( transferHBox2 );
	transferArrow.add( *vbox );
	
	transferButton.signal_clicked().connect( 
					SLOT( *this, &CallWidget::transfer ) );
			
	transferHBox.pack_end( transferButton, false, false ), 
	transferHBox.pack_end( transferEntry, true, true ), 
	
	transferHBox2.pack_end( transferProgress, true, true );
	
	pack_start( transferArrow, false, false, 4 );

#endif
	buttonBox2.add( addCameraButton );
	buttonBox2.add( cancelCameraButton );
	buttonBox2.add( addScreenButton );
	buttonBox2.add( cancelScreenButton );
/*************************************************/
	buttonBox.add( acceptButton );
	buttonBox.add( rejectButton );


//	status.show();
//	secStatus.show();
	topBox->show_all();
	buttonBox2.show_all();
	buttonBox.show_all();
//	rejectButton.show();
//        acceptButton.hide();

	acceptButton.signal_clicked().connect( SLOT( *this, &CallWidget::accept ) );
	rejectButton.signal_clicked().connect( SLOT( *this, &CallWidget::reject ) );
/******************************************************************************************/
	addCameraButton.signal_clicked().connect( SLOT( *this, &CallWidget::addCamera ) );
	addScreenButton.signal_clicked().connect( SLOT( *this, &CallWidget::addScreen ) );
	cancelCameraButton.signal_clicked().connect( SLOT( *this, &CallWidget::cancelCamera ) );
	cancelScreenButton.signal_clicked().connect( SLOT( *this, &CallWidget::cancelScreen ) );	

	cancelCameraButton.hide();
        cancelScreenButton.hide();


	if( incoming ){
		state = CALL_WIDGET_STATE_INCOMING;
//		acceptButton.show();
		//Using the markup, when receiving a call (full uri), the < and > make gtk complain ... s
		string sanitizedUri = remoteUri;
		if (sanitizedUri.find('<') != string::npos)
			sanitizedUri.replace( sanitizedUri.find('<'), 1, " " );
		if (sanitizedUri.find('>') != string::npos)
			sanitizedUri.replace( sanitizedUri.find('>'), 1, " " );
		status.set_markup( "<big><b>Incoming call from \n" + sanitizedUri
				+ "</b></big>");
		
// 		status.set_text( "Incoming call from \n" + remoteUri );
		
		secStatus.set_markup( "The call is <b>" + secure +"</b>." );
		if( secure == "secure" ){
				secureImage.set( Gtk::StockID( "minisip_secure") , Gtk::ICON_SIZE_DIALOG );
		}
		else{
				secureImage.set( Gtk::StockID( "minisip_insecure") , Gtk::ICON_SIZE_DIALOG );
		}

		startRinging();
	}
	else{
		acceptButton.set_sensitive( false );
		state = CALL_WIDGET_STATE_CONNECTING;
		status.set_markup( "<big><b>Connecting...</b></big>" );
		//secStatus.set_markup( "Security requested" );
		rejectButton.set_label( "Cancel" );
	}
}

CallWidget::~CallWidget(){
}

void CallWidget::accept(){
	CommandString * cmd = NULL;
	switch( state ){
                
		case CALL_WIDGET_STATE_INCOMING:
			cmd = new CommandString( mainCallId,
						SipCommandString::accept_invite );
			mainWindow->getCallback()->handleCommand("sip", *cmd );
			stopRinging();
			break;
		case CALL_WIDGET_STATE_INCOMING_TRANSFER:
			cmd = new CommandString( mainCallId, SipCommandString::user_transfer_accept );
			mainWindow->getCallback()->handleCommand("sip", *cmd );
			break;
	}
	if( cmd ){
			delete cmd;
	}
}

void CallWidget::reject(){
	CommandString cmdstr("","");
	stopRinging();


	switch( state ){
		case CALL_WIDGET_STATE_TERMINATED:
			mainWindow->removeCall( mainCallId );
			break;
		case CALL_WIDGET_STATE_INCALL:
		case CALL_WIDGET_STATE_CONNECTING:
		case CALL_WIDGET_STATE_RINGING:
			mainWindow->removeCall( mainCallId );
			cmdstr = CommandString( mainCallId, SipCommandString::hang_up );
			//mainWindow->getCallback()->guicb_handleCommand( cmdstr );
			mainWindow->getCallback()->handleCommand("sip", cmdstr );
			break;
		case CALL_WIDGET_STATE_INCOMING:
			mainWindow->removeCall( mainCallId );
			cmdstr = CommandString( mainCallId, SipCommandString::reject_invite);
			//mainWindow->getCallback()->guicb_handleCommand( cmdstr );
			mainWindow->getCallback()->handleCommand("sip", cmdstr );
			break;
		case CALL_WIDGET_STATE_INCOMING_TRANSFER:
			cmdstr = CommandString( mainCallId, SipCommandString::user_transfer_refuse );
			//mainWindow->getCallback()->guicb_handleCommand( cmdstr );
			mainWindow->getCallback()->handleCommand("sip", cmdstr );
			status.set_markup( "<big><b>In call</b></big>" );
			state = CALL_WIDGET_STATE_INCALL; 
			break;
	}
}

/**************************************************************************/
void CallWidget:: addCamera(){

//printf("=========================================> add Camera button pressed\n");
 	CommandString cmdstr = CommandString( "" , MediaCommandString::start_camera , mainCallId );
        mainWindow->getCallback()->handleCommand("media", cmdstr );
	addCameraButton.hide();
        cancelCameraButton.show();
}


void CallWidget:: cancelCamera(){

//printf("=========================================> cancel Camera button pressed\n");
 	

	CommandString cmdstr = CommandString( "", MediaCommandString::stop_camera , mainCallId );
        mainWindow->getCallback()->handleCommand("media", cmdstr );
	addCameraButton.show();
        cancelCameraButton.hide();

}

void CallWidget:: addScreen(){

//printf("=========================================> add Screen  button pressed\n");
	CommandString cmdstr = CommandString( "", MediaCommandString::start_screen , mainCallId);
        mainWindow->getCallback()->handleCommand("media", cmdstr );
        addScreenButton.hide();
        cancelScreenButton.show();



}

void CallWidget:: cancelScreen(){

//printf("=========================================> cancel Screen button pressed\n");
 	CommandString cmdstr = CommandString( "", MediaCommandString::stop_screen , mainCallId);
        mainWindow->getCallback()->handleCommand("media", cmdstr );
       addScreenButton.show();
        cancelScreenButton.hide();


}



void CallWidget::monitorButtonToggled () {
#ifndef OLDLIBGLADEMM
	string param2;
	if( monitoringButton.get_active() ) {
		param2 = "OFF";
	} else {
		param2 = "ON";
	}
	CommandString cmdstr( getMainCallId(), 
			MediaCommandString::set_session_sound_settings,
			"senders", param2 );
	//mainWindow->getCallback()->guicb_handleMediaCommand( cmdstr );
	mainWindow->getCallback()->handleCommand("media", cmdstr );
#endif
}

void CallWidget::audioOutSilenceButtonToggled () {
#ifndef OLDLIBGLADEMM
	string param2;
	if( audioOutSilenceButton.get_active() ) {
		param2 = "OFF";
	} else {
		param2 = "ON";
	}
	CommandString cmdstr( getMainCallId(), 
			MediaCommandString::set_session_sound_settings,
			"receivers", param2 );
	//mainWindow->getCallback()->guicb_handleMediaCommand( cmdstr );
	mainWindow->getCallback()->handleCommand("media", cmdstr );
#endif
}

void CallWidget::callRecordButtonToggled () {
#ifdef HAVE_LIBGLADEMM_2_6
	string param;
	if( callRecordButton.get_active() ) {
		param = "START";
	} else {
		param = "STOP";
	}
	CommandString cmdstr( getMainCallId(), 
			"call_recorder_start_stop",
			param );
	mainWindow->getCallback()->handleCommand( "media", cmdstr );
#endif
}

void CallWidget::hideAcceptButton(){
	acceptButton.set_sensitive( false );
}

bool CallWidget::handleCommand( CommandString command ){
#ifdef DEBUG_OUTPUT
	cerr << "handleCommand: " << command.getOp() << ", :" << command.getDestinationId() << endl;
#endif
    
	if( handlesCallId( command.getDestinationId() ) ){
		if( command.getOp() == SipCommandString::remote_user_not_found ){
			hideAcceptButton();
			status.set_markup( "<b>User not found</b>" );
			rejectButton.set_label( "Close" );
			state = CALL_WIDGET_STATE_TERMINATED;
		}

		if( command.getOp() == SipCommandString::invite_ok ){
			string who;
			if( command.getParam().length() > 0 ){
				who = " with " + command.getParam();
			}

			status.set_markup( "<big><b>In call" + who + "</b></big>" );

#ifndef OLDLIBGLADEMM
			monitoringButton.show();
			audioOutSilenceButton.show();
#ifdef HAVE_LIBGLADEMM_2_6
			callRecordButton.show();
#endif
			transferArrow.show_all();
			dtmfArrow.show_all();
#endif

			secStatus.set_markup( "The call is <b>" + 
					command.getParam2() + "</b>" );

			if( command.getParam2() == "secure" ){
					secureImage.set( Gtk::StockID( "minisip_secure") , Gtk::ICON_SIZE_DIALOG );
			}
			else{
					secureImage.set( Gtk::StockID( "minisip_insecure") , Gtk::ICON_SIZE_DIALOG );
			}

			rejectButton.set_label( "Hang up" );
			hideAcceptButton();
			stopRinging();
			state = CALL_WIDGET_STATE_INCALL;
			//activate this source
			CommandString cmdstr( getMainCallId(), 
					MediaCommandString::set_session_sound_settings,
					"senders", "ON" );
			mainWindow->getCallback()->handleCommand("media", cmdstr );
		}

		if( command.getOp() == SipCommandString::authentication_failed ){
			status.set_markup( "<b>Authentication failed</b>" );
			rejectButton.set_label( "Close" );
			state = CALL_WIDGET_STATE_TERMINATED;
		}

		if( command.getOp() == SipCommandString::invite_no_reply ){
			status.set_text( "No reply" );
			hideAcceptButton();
			rejectButton.set_label( "Close" );
			state = CALL_WIDGET_STATE_TERMINATED;
		}

		if( command.getOp() == SipCommandString::transport_error ){
			status.set_text( "The call failed due to a network error" );
			hideAcceptButton();
			rejectButton.set_label( "Close" );
			state = CALL_WIDGET_STATE_TERMINATED;
		}
		
		if( command.getOp() == SipCommandString::remote_unacceptable ){
			status.set_text( "The remote user could not\nhandle the call." );
			if( command.getParam() != "" ){
				secStatus.set_markup( "<small>" + command.getParam() + "</small>" );
			}
			hideAcceptButton();
			rejectButton.set_label( "Close" );
			state = CALL_WIDGET_STATE_TERMINATED;
		}

		if( command.getOp() == SipCommandString::remote_hang_up ){
			stopRinging();
			callIds.remove( command.getDestinationId() );
			if( command.getDestinationId() == mainCallId ){
					status.set_markup( "<b><big>Call ended</big></b>" );
					secStatus.set_text( "" );
					rejectButton.set_label( "Close" );
					state = CALL_WIDGET_STATE_TERMINATED;
#ifndef OLDLIBGLADEMM
					monitoringButton.hide();
					audioOutSilenceButton.hide();
#ifdef HAVE_LIBGLADEMM_2_6
					callRecordButton.hide();
#endif
					transferArrow.hide();
					dtmfArrow.hide();
#endif
			}
		}

		if( command.getOp() == SipCommandString::call_terminated_early ){
			stopRinging();
			callIds.remove( command.getDestinationId() );
			if( command.getDestinationId() == mainCallId ){
					status.set_markup( "<b><big>Call Terminated</big></b>" );
//					secStatus.set_text( "" );
					rejectButton.set_label( "Close" );
					state = CALL_WIDGET_STATE_TERMINATED;
#ifndef OLDLIBGLADEMM
					monitoringButton.hide();
					audioOutSilenceButton.hide();
#ifdef HAVE_LIBGLADEMM_2_6
					callRecordButton.hide();
#endif
					transferArrow.hide();
					dtmfArrow.hide();
#endif
			}
		}
		
		if( command.getOp() == SipCommandString::remote_reject ){
			status.set_text( "Remote side rejected the call" );
			secStatus.set_text( "" );
			rejectButton.set_label( "Close" );
			state = CALL_WIDGET_STATE_TERMINATED;
		}

		if( command.getOp() == SipCommandString::remote_cancelled_invite ){
			stopRinging();
			status.set_text( "Remote side cancelled the call" );
			rejectButton.set_label( "Close" );
			state = CALL_WIDGET_STATE_TERMINATED;
			secStatus.set_text( "" );
			hideAcceptButton();
		}

		if( command.getOp() == SipCommandString::remote_ringing ){
			status.set_markup( "<b><big>Ringing...</big></b>" );
			rejectButton.set_label( "Hang up" );
			state = CALL_WIDGET_STATE_RINGING;
		}

		if( command.getOp() == SipCommandString::temp_unavail ){
			status.set_text( "The user is reported temporary unavailable" );
			rejectButton.set_label("Close");
			state = CALL_WIDGET_STATE_TERMINATED;
		}

		if( command.getOp() == SipCommandString::cancel_ok ){
			status.set_text( "Call canceled" );
			rejectButton.set_label( "Close" );
			state = CALL_WIDGET_STATE_TERMINATED;
		}

		if( command.getOp() == SipCommandString::security_failed ){
			stopRinging();

			secStatus.set_text( "Security is not handled by the receiver" );
//			secStatus.set_text( "" );
			rejectButton.set_label( "Close" );
			state = CALL_WIDGET_STATE_TERMINATED;
		}
		
		if( command.getOp() == SipCommandString::transfer_requested ){
			status.set_text( "Accept transfer to " + command.getParam() + "?" );
			secStatus.set_text( "" );
			acceptButton.show();
			acceptButton.set_sensitive( true );
			acceptButton.set_label( "Accept" );
			rejectButton.set_label( "Reject" );
			state = CALL_WIDGET_STATE_INCOMING_TRANSFER;
		}

		if( command.getOp() == SipCommandString::call_transferred ){
				/* Change the callId so that we get next commands */
				mainCallId = command.getParam();
				callIds.push_back( mainCallId );
				status.set_text( "Call transferred... ");
		}
                
		if( command.getOp() == SipCommandString::transfer_pending ){
#ifndef OLDLIBGLADEMM
			transferProgress.set_text( "Transfer accepted..." );
#endif
			//transferProgress.pulse();
		}
                
		if( command.getOp() == SipCommandString::transfer_refused ){
#ifndef OLDLIBGLADEMM
			transferProgress.set_text( "Transfer rejected." );
			transferEntry.set_sensitive( true );
			transferButton.set_sensitive( true );
#endif
		}
                if( command.getOp() == "zrtp_security_change") {
                    secStatus.set_markup( "The call is <b>" + 
                            command.getParam() + "</b>" );
                    sasData.set_text("SAS: " + command.getParam2());
                    if( command.getParam() == "secure" ){
                        secureImage.set( Gtk::StockID( "minisip_secure") , Gtk::ICON_SIZE_DIALOG );
                    }
                    else {
                        secureImage.set( Gtk::StockID( "minisip_insecure") , Gtk::ICON_SIZE_DIALOG );
                    }
                }
		return true;
	}
	return false;
}

void CallWidget::startRinging(){
	if(!bell){
		bell = new Bell();
	}
	bell->start();
	CommandString cmdstr = CommandString( "", MediaCommandString::start_ringing );
	//mainWindow->getCallback()->guicb_handleMediaCommand( cmdstr );
	mainWindow->getCallback()->handleCommand("media", cmdstr );

}

void CallWidget::stopRinging(){
	if (bell){
		bell->stop();
		bell=NULL;
	}
	CommandString cmdstr = CommandString( "", MediaCommandString::stop_ringing );
	mainWindow->getCallback()->handleCommand("media", cmdstr );
}

#ifndef OLDLIBGLADEMM

void CallWidget::transfer(){
	string uri = Glib::locale_from_utf8( transferEntry.get_text() );
	if( uri.size() > 0 ){
	
			CommandString transfer( mainCallId, 
							SipCommandString::user_transfer, uri );
			//mainWindow->getCallback()->guicb_handleCommand( transfer );
			mainWindow->getCallback()->handleCommand("sip", transfer );

			transferEntry.set_sensitive( false );
			transferButton.set_sensitive( false );

			transferProgress.set_text( "Transfer requested..." );
			//transferProgress.pulse();
	}
}

void CallWidget::dtmfPressed( uint8_t symbol ){
	char s[2];
	s[0]=symbol;
	s[1]=0;
	// Handle 0 <= symbol <= 11
	string str( s, 1 );
	CommandString cmd( mainCallId,
			   MediaCommandString::send_dtmf,
			   str );
	mainWindow->getCallback()->handleCommand( "media", cmd );

#if 0
	MRef<Session *> session = Session::registry->getSession( mainCallId );

	if( session ){
			session->sendDtmf( symbol );
	}
#endif
}

#endif

string CallWidget::getMainCallId(){
	return mainCallId;
}

bool CallWidget::handlesCallId( string callId ){
	list<string>::iterator iCallId;

	for( iCallId = callIds.begin(); iCallId != callIds.end(); iCallId++ ){
			if( *iCallId == callId ){
					return true;
			}
	}
	return false;
}

StockButton::StockButton( Gtk::StockID stockId, Glib::ustring text ):
	box( 2 ), image( stockId, Gtk::ICON_SIZE_SMALL_TOOLBAR ), label( text ){
	box.pack_start( image, false, false );
	box.pack_end( label, true, true );
	add( box );
	show_all();
}

void StockButton::set_label (const Glib::ustring& label){
	this->label.set_text( label );
}

void CallWidget::activeWidgetChanged( bool isActive, int currentActive ) {

	if( isActive == activeCallWidget ) {
#ifdef DEBUG_OUTPUT
// 		cerr << "CallWidget::activeCall - nothing to do here (no active state change)" << endl;
#endif
		return;
	} else {
		activeCallWidget = isActive;
	}

	//our status has changed ... do something?
	if( !isActive ) {
#ifdef DEBUG_OUTPUT
// 		cerr << "CallWidget::activeCall - We were active ... not anymore" << endl;
#endif
		CommandString cmdstr( getMainCallId(), 
				MediaCommandString::set_session_sound_settings,
				"senders", "OFF" );
		//mainWindow->getCallback()->guicb_handleMediaCommand( cmdstr );
		mainWindow->getCallback()->handleCommand("media", cmdstr );
		return;
	} else {
#ifdef DEBUG_OUTPUT
// 		cerr << "CallWidget::activeCall - We active!" << endl;
#endif
		if( getState() == CALL_WIDGET_STATE_INCALL 
#ifndef OLDLIBGLADEMM
			&& ! monitoringButton.get_active() 
#endif
			){
			CommandString cmdstr( getMainCallId(), 
					MediaCommandString::set_session_sound_settings,
					"senders", "ON" );
			//mainWindow->getCallback()->guicb_handleMediaCommand( cmdstr );
			mainWindow->getCallback()->handleCommand("media", cmdstr );
		} else {
#ifdef DEBUG_OUTPUT
// 			fprintf( stderr, "CallWidget::onTabChange ... doing nothing (call widget state)!\n" );
#endif
		}
	}
}


IconToggleButton::IconToggleButton( Gtk::StockID id1, Gtk::StockID id2 ):
	image1( id1, Gtk::ICON_SIZE_BUTTON ),image2( id2, Gtk::ICON_SIZE_BUTTON ){
	set_relief( Gtk::RELIEF_NONE );
	add( image1 );
	image1.show_all();
	image2.show_all();
}

void IconToggleButton::on_toggled(){
	if( get_active() ){
		remove();
		add( image2 );
//		image1.show_all();
	}
	else{
		remove();
		add( image1 );
//		image2.show_all();
	}
	Gtk::ToggleButton::on_toggled();
}
