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

#include"LogWidget.h"

#include"../../LogEntry.h"
#include<libmutil/itoa.h>


LogWidget::LogWidget( MainWindow * mainWindow ){
	this->mainWindow = mainWindow;
	
	columns.add( startColumn );
	columns.add( typeColumn );
	columns.add( uriColumn );
	columns.add( statusColumn );
	columns.add( entry );
	
	listStore = Gtk::ListStore::create( columns );

	set_model( listStore );
	append_column( "Start", startColumn );
	append_column( "Type", typeColumn );
	
	Gtk::CellRendererText * renderer = new Gtk::CellRendererText();
	
	insert_column_with_data_func( 2, "From", *renderer,
		SigC::slot( *this, &LogWidget::setFont ) );

	set_headers_visible( false );
	set_rules_hint( true );
	get_selection()->set_select_function( SigC::slot( *this, &LogWidget::lineSelect ) );

	iconRenderer = (Gtk::CellRendererPixbuf*)get_column_cell_renderer( 1 );

}

void LogWidget::addLogEntry( MRef<LogEntry *> entry ){
	Gtk::TreeModel::iterator iter = listStore->append();
	Gtk::StockID stockGoBack( Gtk::Stock::GO_BACK );
	Gtk::StockID stockGoForw( Gtk::Stock::GO_FORWARD );
	Gtk::IconSize menuIconSize( Gtk::ICON_SIZE_SMALL_TOOLBAR  );

	bool red = false;

	if( dynamic_cast<LogEntryIncoming *>( *entry ) != NULL ){
		(*iter)[ typeColumn ] = render_icon( stockGoBack, menuIconSize );
	}

	else{
		(*iter)[ typeColumn ] = render_icon( stockGoForw, menuIconSize );
	}

	(*iter)[ this->entry ] = entry;

#if 0
	item
	string callDetail = entry->peerSipUri;
	
	if( dynamic_cast<LogEntryFailure *>( *entry ) != NULL ){
		callDetail += "\n" + 
			dynamic_cast<LogEntryFailure *>(*entry)->error;
		red = true;
	}
		
	(*iter)[ uriColumn ] = callDetail;

	/* Build a list of attribute for the status message */
	Pango::AttrList attrList;
	
	Pango::Attribute attrScale;
	attrScale = Pango::Attribute::create_attr_scale(
			Pango::SCALE_SMALL );
	attrScale.set_start_index( entry->peerSipUri.length() + 1 );
	attrScale.set_end_index( callDetail.length() );
	attrList.insert( attrScale );

	Pango::Attribute attrColor;
	attrColor = Pango::Attribute::create_attr_foreground( red?65535:0, 0, 0  );
	attrColor.set_start_index( entry->peerSipUri.length() + 1 );
	attrColor.set_end_index( callDetail.length() );
	attrList.insert( attrColor );
	
	/* Apply that attribute list to the CellRenderer */
	Gtk::CellRendererText * r = 
		(Gtk::CellRendererText*)get_column_cell_renderer( 2 );

	r->property_attributes().set_value( attrList );
	
		
#endif
	
	
	struct tm * startTm = new struct tm;
#ifndef WIN32
	localtime_r( &entry->start, startTm );
	(*iter)[ startColumn ] = itoa( startTm->tm_hour ) + ":" + 
				((startTm->tm_min < 10) ? "0" : "") + 
		                 itoa( startTm->tm_min );
#endif
	delete startTm;
}

bool LogWidget::lineSelect( const Glib::RefPtr<Gtk::TreeModel>& model,
		            const Gtk::TreeModel::Path& path, bool ){
	return false;
}

void LogWidget::setFont( Gtk::CellRenderer * renderer,
		         const Gtk::TreeModel::iterator & iter ){

	Gtk::CellRendererText * textR =
		(Gtk::CellRendererText *)renderer;

	MRef<LogEntry *> logEntry = (*iter)[ this->entry ];
	
	string callDetail = logEntry->peerSipUri;
	
	MRef<LogEntryFailure *> failureEntry;

	failureEntry = dynamic_cast<LogEntryFailure *>( *logEntry );
	
	if( *failureEntry != NULL ){
		callDetail += ((string)"\n" + "<small><span foreground=\"#FF0000\">" +
			       failureEntry->error + "</span></small>");
	}

	textR->property_markup().set_value( callDetail );

}

