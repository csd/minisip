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

#include"CallWidget.h"
#include"MainWindow.h"
//#include"../../../sip/state_machines/SipSMCommand.h"
//#include"../../../sip/state_machines/SipSoftPhone.h"
#include<libmsip/SipSMCommand.h>
#include<libmsip/SipCommandString.h>
#include"../../Bell.h"
#include"../../../mediahandler/MediaCommandString.h"




CallWidget::CallWidget( string callId, string remoteUri, MainWindow * mw, bool incoming, string secure):
		mainWindow( mw ),
                callId( callId ),
		status( "" ),
		secStatus( "" ),
                acceptButton( "Accept" ),
                rejectButton( "Reject" ),
		bell(/*NULL*/)//,
//		timeoutProvider(tp)
{
	bell = NULL;

	add( status );
	add( secStatus );
	pack_end( buttonBox, false, true );

	//buttonBox.set_expand( false );

	status.set_use_markup( true );
	secStatus.set_use_markup( true );

	buttonBox.add( acceptButton );
	buttonBox.add( rejectButton );

	status.show();
	secStatus.show();
	buttonBox.show();
	rejectButton.show();

	acceptButton.signal_clicked().connect( SigC::slot( *this, &CallWidget::accept ) );
	rejectButton.signal_clicked().connect( SigC::slot( *this, &CallWidget::reject ) );

	if( incoming ){
		state = CALL_WIDGET_STATE_INCOMING;
		acceptButton.show();
		status.set_markup( "Incoming call from \n<b>" + remoteUri
				+ "</b>");
		secStatus.set_markup( "The call is <b>" + secure +"</b>." );
		startRinging();
	}
	else{
		state = CALL_WIDGET_STATE_CONNECTING;
		status.set_text( "Connecting..." );
		rejectButton.set_label( "Cancel" );
	}
}

CallWidget::~CallWidget(){
}

#ifdef IPAQ
void CallWidget::update(){
	mainWindow->hide();
	mainWindow->show();
}
#endif


void CallWidget::accept(){
	if( state == CALL_WIDGET_STATE_INCOMING ){
	
		CommandString accept( callId, SipCommandString::accept_invite );
		mainWindow->getCallback()->guicb_handleCommand( accept );
	}
	stopRinging();
}

void CallWidget::reject(){
	CommandString cmdstr("","");
	stopRinging();

	switch( state ){
		case CALL_WIDGET_STATE_TERMINATED:
			mainWindow->removeCall( callId );
			break;
		case CALL_WIDGET_STATE_INCALL:
		case CALL_WIDGET_STATE_CONNECTING:
		case CALL_WIDGET_STATE_RINGING:
			mainWindow->removeCall( callId );
			cmdstr = CommandString( callId, SipCommandString::hang_up );
			mainWindow->getCallback()->guicb_handleCommand( cmdstr );
			break;
		case CALL_WIDGET_STATE_INCOMING:
			mainWindow->removeCall( callId );
			cmdstr = CommandString( callId, SipCommandString::reject_invite);
			mainWindow->getCallback()->guicb_handleCommand( cmdstr );
			
			break;


	}
}

void CallWidget::hideAcceptButton(){
	acceptButton.hide();
}

string CallWidget::getCallId(){
	return callId;
}

bool CallWidget::handleCommand( CommandString command ){
	if( callId == command.getDestinationId() ){
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

			status.set_text( "In call" + who );

			secStatus.set_markup( "The call is <b>" + 
					command.getParam2() + "</b>" );
			rejectButton.set_label( "Hang up" );
			hideAcceptButton();
			state = CALL_WIDGET_STATE_INCALL;
#ifdef IPAQ
			update();
#endif

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

			status.set_text( "Call ended" );
			secStatus.set_text( "" );
			rejectButton.set_label( "Close" );
			state = CALL_WIDGET_STATE_TERMINATED;
#ifdef IPAQ
			update();
#endif
		}

		if( command.getOp() == SipCommandString::remote_reject ){
			status.set_text( "Remote side rejected the call" );
			secStatus.set_text( "" );
			rejectButton.set_label( "Close" );
			state = CALL_WIDGET_STATE_TERMINATED;
#ifdef IPAQ
			update();
#endif
		}

		if( command.getOp() == SipCommandString::remote_cancelled_invite ){
			stopRinging();


			status.set_text( "Remote side cancelled the call" );
			rejectButton.set_label( "Close" );
			state = CALL_WIDGET_STATE_TERMINATED;
			secStatus.set_text( "" );
			hideAcceptButton();
#ifdef IPAQ
			update();
#endif
		}

		if( command.getOp() == SipCommandString::remote_ringing ){
			status.set_text( "Ringing..." );
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

			status.set_text( "Security is not handled by the receiver" );
			secStatus.set_text( "" );
			rejectButton.set_label( "Close" );
			state = CALL_WIDGET_STATE_TERMINATED;
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
	mainWindow->getCallback()->guicb_handleMediaCommand( cmdstr );

}

void CallWidget::stopRinging(){
	if (bell){
		bell->stop();
		bell=NULL;
	}
	CommandString cmdstr = CommandString( "", MediaCommandString::stop_ringing );
	mainWindow->getCallback()->guicb_handleMediaCommand( cmdstr );
}
