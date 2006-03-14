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

/* 
 * Purpose
 * 	Used to experiment with presence information. The class generates location update commands.
 * 
*/
#ifndef	LOCATIONDETECTOR_H
#define LOCATIONDETECTOR_H

#include<config.h>

#include<libmsip/SipDialogContainer.h>
#include"../sip/DefaultDialogHandler.h"

#include<libmutil/Thread.h>

/**
 * @author Erik Eliasson
 * 
*/
class LocationDetector : public Runnable {
	public:
		LocationDetector(MRef<SipDialogContainer*> callback);
		virtual ~LocationDetector(){}
                virtual void run();
	private:
		MRef<SipDialogContainer*> callback;
};


#endif
