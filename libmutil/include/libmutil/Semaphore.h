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

/**
 * Semaphores are implemented differently depending on platform. This class
 * is a generalization of a semaphore implementation and only needs the
 * "Mutex" class. Only two operations are implemented, "inc" and "dec".
 *
 * @author Erik Eliasson, eliasson@it.kth.se
*/

#ifndef _MINISIPSEMAPHORE_H
#define _MINISIPSEMAPHORE_H

class Semaphore{
    public:
        Semaphore();
        ~Semaphore();
        
        /**
         * Put one resource into the set of resources.
         */
        void inc();
        
        /**
         * Wait until at least one resource is available and take it.
         */
        void dec();

    private:
	void *handlePtr;
};

#endif
