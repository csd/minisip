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

#ifndef EVE_QTGUI_H
#define EVE_QTGUI_H

#include<qlineedit.h>
#include<qapplication.h>
#include<qlayout.h>
#include<qlistview.h>
#include<string>


using namespace std;
class EveGui : public QWidget{
	public:
		EveGui();
		~EveGui(){};
		void customEvent(QCustomEvent *);

		void addStream(string from, string to);
		void removeStream(string from, string to);
		bool doPlay(string from, string to);
		
	private:
		QVBoxLayout vlayout;
		QListView *listView;
};

class CommandEvent : public QCustomEvent{
	public:
		CommandEvent(string command, string from, string to):QCustomEvent(QEvent::User),command(command),from(from),to(to){};
		string command, from, to;
};


#endif
