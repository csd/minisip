/*
 Copyright (C) 2004-2006 the Minisip Team
 
 This library is free software; you can redistribute it and/or
 modify it under the terms of the GNU Lesser General Public
 License as published by the Free Software Foundation; either
 version 2.1 of the License, or (at your option) any later version.
 
 This library is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 Lesser General Public License for more details.
 
 You should have received a copy of the GNU Lesser General Public
 License along with this library; if not, write to the Free Software
 Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
 */

/* Copyright (C) 2004 
 *
 * Authors: Erik Eliasson <eliasson@it.kth.se>
 *          Johan Bilien <jobi@via.ecp.fr>
*/

#include<config.h>

#include<libminisip/media/soundcard/SilenceSensor.h>

static int iabs(int i){
    if (i<0)
        return -i;
    return i;
}

static int energy(uint16_t *buf, int n){
    long ret=0;
    for (int i=0; i< n; i++)
        ret+=iabs(buf[i]);
    return ret/n;
}

SimpleSilenceSensor::SimpleSilenceSensor():inSilence(false),noiceLevel(0.0){
    limit_on=15;
    limit_off=7;
}

bool SimpleSilenceSensor::silence(uint16_t *buf, int n){
    int e = energy(buf, n);

    if (inSilence && e>=limit_on){
        inSilence=false;
    }
    if (!inSilence && e<=limit_off){
        inSilence=true;
    }
    return inSilence;
}

/*


.
...
.....
......       <--- 
...
....
..
.
.

.


.
.
..
.

.

..

.

*/
