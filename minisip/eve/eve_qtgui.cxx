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

#include <qapplication.h>
#include <qpushbutton.h>
#include <qfont.h>
#include"eve_qtgui.h"
#include<qlistview.h>
#include<string>
#include<iostream>


using namespace std;

EveGui::EveGui():vlayout(this){
	listView = new QListView(this);
	listView->setAllColumnsShowFocus(true);
	listView->addColumn("Stream");
	//listView->setResizeMode(QListView::AllColumns);
	
	vlayout.addWidget(listView);
}

void EveGui::customEvent(QCustomEvent *e){
	CommandEvent *cep = (CommandEvent *)e;
	
	if (cep->command=="add"){
		QCheckListItem *item = new QCheckListItem(listView, 
				(cep->from+"->"+cep->to).c_str(), 
				QCheckListItem::CheckBox);
		item = NULL; //dummy statement to remove compile warning
	}
	
	if (cep->command=="remove"){

		QListViewItem * myChild = listView->firstChild();
		while( myChild ) {
			if ( strcmp(myChild->text(0).ascii(), (cep->from+"->"+cep->to).c_str() )==0){
				listView->takeItem(myChild);
				return;
			}
			myChild = myChild->nextSibling();
		}
			       
		cerr << "ERROR: remove stream form GUI unimplemented"<< endl;
	
	}
}

void EveGui::addStream(string from, string to){
	CommandEvent *ce = new CommandEvent("add", from, to);
	qApp->postEvent(this, ce);
}

void EveGui::removeStream(string from, string to){
	CommandEvent *ce = new CommandEvent("remove", from, to);
	qApp->postEvent(this, ce);
}

bool EveGui::doPlay(string from, string to){
	QCheckListItem *cur = (QCheckListItem*) listView->firstChild();

	while (cur!=NULL){
		if ( strcmp( cur->text().ascii(), (from+"->"+to).c_str() ) == 0 )
			return cur->isOn();
			
		cur = (QCheckListItem*)cur->nextSibling();
	}
	cerr << "ERROR: Unknown stream: "<< from <<"->" << to << endl;
	return false;
}


