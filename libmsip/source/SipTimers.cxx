/*
  Copyright (C) 2005, 2004 Erik Eliasson, Johan Bilien
  
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
  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

/*
 * Authors: Erik Eliasson <eliasson@it.kth.se>
 *          Johan Bilien <jobi@via.ecp.fr>
*/


#include<config.h>
#include<libmsip/SipTimers.h>

#define _T1 500
#define _T4 5000


const int SipTimers::T1 = _T1;
const int SipTimers::T2 = 4000;
const int SipTimers::T4 = _T4;
const int SipTimers::timerA = _T1; //=T1
const int SipTimers::timerB = 64 * _T1;
const int SipTimers::timerC = 4*60*1000; //4min (should be >3min)
const int SipTimers::timerD = 60000; // 60s (should be >32s)
const int SipTimers::timerE = _T1;
const int SipTimers::timerF = 64 * _T1;
const int SipTimers::timerG = _T1;
const int SipTimers::timerH = 64*_T1;
const int SipTimers::timerI = _T4;
const int SipTimers::timerJ = 64*_T1;
const int SipTimers::timerK = _T4;

