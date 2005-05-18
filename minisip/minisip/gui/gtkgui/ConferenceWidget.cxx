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

#include"ConferenceWidget.h"
#include "../../../conf/ConferenceControl.h"
#include"MainWindow.h"
//#include"../../../sip/state_machines/SipSMCommand.h"
//#include"../../../sip/state_machines/SipSoftPhone.h"
#include<libmsip/SipSMCommand.h>
#include<libmsip/SipCommandString.h>
#include"../../Bell.h"
#include"../../../mediahandler/MediaCommandString.h"
#include"../../../mediahandler/Session.h"
#include"../../../mediahandler/MediaStream.h"

#ifdef OLDLIBGLADEMM
#define SLOT(a,b) SigC::slot(a,b)
#define BIND SigC::bind
#else
#define SLOT(a,b) sigc::mem_fun(a,b)
#define BIND sigc::bind
#endif




ConferenceWidget::ConferenceWidget(string configUri, string confId, string users, string remoteUri,string callId, MainWindow * mw, bool incoming):
		mainWindow( mw ),
		status( "", Gtk::ALIGN_LEFT ),
		secStatus( "", Gtk::ALIGN_LEFT ),
                buttonBox(/*homogenius*/ true ),
		conferenceButton("Add Member"),

                secureImage(),// Gtk::StockID( "minisip_insecure" ), 
                          //   Gtk::ICON_SIZE_DIALOG ),
                insecureImage( Gtk::StockID( "minisip_insecure" ), 
                             Gtk::ICON_SIZE_DIALOG ),
                acceptButton( Gtk::Stock::OK, "Accept" ),
                rejectButton( Gtk::Stock::CANCEL, "Reject" ),
		bell(),
		mainCallId(callId),
		initiatorUri(remoteUri)
{
	conf=new ConferenceControl(configUri,confId, !incoming);
	mainWindow->getCallback()->setConferenceController(conf);
	mainConfId=confId;
	bell = NULL;
        //callIds.push_back( callId );
	cerr<<"list of users in the current conference: "+remoteUri+" "+users<<endl;
	
        Gtk::HBox * topBox = manage( new Gtk::HBox );

        topBox->pack_start( secureImage, false, false, 5 );

        Gtk::VBox * rightTopBox = manage( new Gtk::VBox );
        topBox->pack_start( *rightTopBox, false, false, 5 );

        
        rightTopBox->pack_start( status, false, false, 5 );
        rightTopBox->pack_start( secStatus, false, false, 5 );
        
        pack_start( *topBox, false, false, 5 );

        Pango::AttrList attrList( "<big><b></b></big>" );
        status.set_attributes( attrList );

        Gtk::HSeparator * separator = manage( new Gtk::HSeparator );
        pack_start( *separator, false, false, 5 );
        separator->show();

//	add( status );
//	add( secStatus );
	pack_end( buttonBox, false, true );

	//buttonBox.set_expand( false );

	status.set_use_markup( true );
	secStatus.set_use_markup( true );

/*#ifndef OLDLIBGLADEMM
        
        

	
        

#endif*/
	conferenceHBox.pack_end( conferenceButton, false, false ); 
        conferenceHBox.pack_end( conferenceEntry, true, true ); 
	pack_start( conferenceHBox, false, false, 4 );
	conferenceHBox.show_all();

	buttonBox.add( acceptButton );
	buttonBox.add( rejectButton );


//	status.show();
//	secStatus.show();
        topBox->show_all();
	buttonBox.show_all();
//	rejectButton.show();
//        acceptButton.hide();

	acceptButton.signal_clicked().connect( SLOT( *this, &ConferenceWidget::accept ) );
	conferenceButton.signal_clicked().connect( SLOT( *this, &ConferenceWidget::add ) );
	rejectButton.signal_clicked().connect( SLOT( *this, &ConferenceWidget::reject ) );

	if( incoming ){
		state = CONFERENCE_WIDGET_STATE_INCOMING;
//		acceptButton.show();
		conferenceButton.set_sensitive(false);
		rejectButton.set_label( "Reject" );
		status.set_markup( "<b>Incoming conference call from \n" + remoteUri
				+ "</b>");
		secStatus.set_markup( "<b>participants: </b>" + remoteUri+" "+users );
                /*if( secure == "secure" ){
                        secureImage.set( Gtk::StockID( "minisip_secure") , Gtk::ICON_SIZE_DIALOG );
                }
                else{
                        secureImage.set( Gtk::StockID( "minisip_insecure") , Gtk::ICON_SIZE_DIALOG );
                }*/

		startRinging();
	}
	else{
                acceptButton.set_sensitive( false );
		state = CONFERENCE_WIDGET_STATE_CREATED;
		//status.set_markup( "<big><b>Connecting...</b></big>" );
//                secStatus.set_markup( "Security requested" );
		acceptButton.set_label( "Accept" );
		rejectButton.set_label( "Quit" );
	}
}

ConferenceWidget::~ConferenceWidget(){
}

void ConferenceWidget::accept(){
        CommandString command(mainCallId, SipCommandString::accept_invite, initiatorUri);
	command.setParam3(mainConfId);
	mainWindow->getCallback()->guicb_handleConfCommand(command);
	acceptButton.set_sensitive( false );
	conferenceButton.set_sensitive(true);
	rejectButton.set_label( "Quit" );
	stopRinging();
}
void ConferenceWidget::add(){
        string uri = Glib::locale_from_utf8( conferenceEntry.get_text() );
        if( uri.size() > 0 ){
        
                CommandString cmd("", "join",uri);
		cmd.setParam3(mainConfId);
		mainWindow->getCallback()->guicb_handleConfCommand(cmd);

		//conf->handleGuiDoInviteCommand("ali");
		//mainWindow->getCallback()->guicb_confDoInvite(uri);
		//transferProgress.pulse();
        }
	//mainWindow->getCallback()->guicb_confDoInvite(uri);
	
}
void ConferenceWidget::reject(){
	CommandString hup(mainCallId, SipCommandString::hang_up);
	hup.setParam3(mainConfId);
	mainWindow->getCallback()->guicb_handleConfCommand(hup);
	mainWindow->getCallback()->guicb_handleCommand( hup );
	mainWindow->removeConference( mainConfId );
	stopRinging();
}

void ConferenceWidget::hideAcceptButton(){
	acceptButton.set_sensitive( false );
}

bool ConferenceWidget::handleCommand( CommandString command ){
	if( handlesConfId( command.getDestinationId() ) ){
		if( command.getOp() == SipCommandString::remote_user_not_found ){
			hideAcceptButton();
			status.set_markup( "<b>User not found</b>" );
			rejectButton.set_label( "Close" );
			state = CONFERENCE_WIDGET_STATE_TERMINATED;
		}
		if( command.getOp() == "list updated" ){
			cerr<<"command.getParam()::::::::::::"+command.getParam()<<endl;
			status.set_markup( "<b>Connected: </b>"+command.getParam());
			secStatus.set_markup( "<b>Pending: </b>"+command.getParam2() );
		}

		if( command.getOp() == SipCommandString::invite_ok ){
			string who;
			if( command.getParam().length() > 0 ){
				who = " with " + command.getParam();
			}

			status.set_markup( "<big><b>In call" + who + "</b></big>" );

#ifndef OLDLIBGLADEMM
                        
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
			state = CONFERENCE_WIDGET_STATE_INCALL;

		}

		if( command.getOp() == SipCommandString::authentication_failed ){
			status.set_markup( "<b>Authentication failed</b>" );
			rejectButton.set_label( "Close" );
			state = CONFERENCE_WIDGET_STATE_TERMINATED;
		}

		if( command.getOp() == SipCommandString::invite_no_reply ){
			status.set_text( "No reply" );
			hideAcceptButton();
			rejectButton.set_label( "Close" );
			state = CONFERENCE_WIDGET_STATE_TERMINATED;
		}

		if( command.getOp() == SipCommandString::transport_error ){
			status.set_text( "The call failed due to a network error" );
			hideAcceptButton();
			rejectButton.set_label( "Close" );
			state = CONFERENCE_WIDGET_STATE_TERMINATED;
		}
		
		if( command.getOp() == SipCommandString::remote_unacceptable ){
			status.set_text( "The remote user could not\nhandle the call." );
			if( command.getParam() != "" ){
				secStatus.set_markup( "<small>" + command.getParam() + "</small>" );
			}
			hideAcceptButton();
			rejectButton.set_label( "Close" );
			state = CONFERENCE_WIDGET_STATE_TERMINATED;
		}

		

		if( command.getOp() == SipCommandString::remote_reject ){
			status.set_text( "Remote side rejected the call" );
			secStatus.set_text( "" );
			rejectButton.set_label( "Close" );
			state = CONFERENCE_WIDGET_STATE_TERMINATED;
		}

		if( command.getOp() == SipCommandString::remote_cancelled_invite ){
			stopRinging();


			status.set_text( "Remote side cancelled the call" );
			rejectButton.set_label( "Close" );
			state = CONFERENCE_WIDGET_STATE_TERMINATED;
			secStatus.set_text( "" );
			hideAcceptButton();
		}

		if( command.getOp() == SipCommandString::remote_ringing ){
			status.set_markup( "<b><big>Ringing...</big></b>" );
			rejectButton.set_label( "Hang up" );
			state = CONFERENCE_WIDGET_STATE_RINGING;
		}

		if( command.getOp() == SipCommandString::temp_unavail ){
			status.set_text( "The user is reported temporary unavailable" );
			rejectButton.set_label("Close");
			state = CONFERENCE_WIDGET_STATE_TERMINATED;
		}

		if( command.getOp() == SipCommandString::cancel_ok ){
			status.set_text( "Call canceled" );
			rejectButton.set_label( "Close" );
			state = CONFERENCE_WIDGET_STATE_TERMINATED;
		}

		if( command.getOp() == SipCommandString::security_failed ){
			stopRinging();

			status.set_text( "Security is not handled by the receiver" );
			secStatus.set_text( "" );
			rejectButton.set_label( "Close" );
			state = CONFERENCE_WIDGET_STATE_TERMINATED;
		}
		
                

		return true;
	}

	return false;
}

void ConferenceWidget::startRinging(){
	if(!bell){
		bell = new Bell();
	}
	bell->start();
	CommandString cmdstr = CommandString( "", MediaCommandString::start_ringing );
	mainWindow->getCallback()->guicb_handleMediaCommand( cmdstr );

}

void ConferenceWidget::stopRinging(){
	if (bell){
		bell->stop();
		bell=NULL;
	}
	CommandString cmdstr = CommandString( "", MediaCommandString::stop_ringing );
	mainWindow->getCallback()->guicb_handleMediaCommand( cmdstr );
}

#ifndef OLDLIBGLADEMM

void ConferenceWidget::transfer(){
        string uri = Glib::locale_from_utf8( transferEntry.get_text() );
        if( uri.size() > 0 ){
        
                CommandString transfer( mainCallId, 
                                SipCommandString::user_transfer, uri );
                mainWindow->getCallback()->guicb_handleCommand( transfer );

                transferEntry.set_sensitive( false );
                transferButton.set_sensitive( false );

                transferProgress.set_text( "Transfer requested..." );
//                transferProgress.pulse();
        }
}

void ConferenceWidget::dtmfPressed( uint8_t symbol ){
        MRef<Session *> session = Session::registry->getSession( mainCallId );

        if( session ){
                
                session->sendDtmf( symbol );
        }
}

#endif

string ConferenceWidget::getMainCallId(){
        return mainCallId;
}
string ConferenceWidget::getMainConfId(){
        return mainConfId;
}
bool ConferenceWidget::handlesConfId( string confId ){
        

        if(confId==mainConfId){
               return true;
        }
        return false;
}

/*StockButton::StockButton( Gtk::StockID stockId, Glib::ustring text ):
	box( 2 ), image( stockId, Gtk::ICON_SIZE_SMALL_TOOLBAR ), label( text ){
	box.pack_start( image, false, false );
	box.pack_end( label, true, true );
	add( box );
	show_all();
}

void StockButton::set_label (const Glib::ustring& label){
	this->label.set_text( label );
}*/
