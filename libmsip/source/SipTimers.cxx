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


int SipTimers::T1 = _T1;
int SipTimers::T2 = 4000;
int SipTimers::T4 = _T4;
int SipTimers::A = _T1; //=T1
int SipTimers::B = 64 * _T1;
int SipTimers::C = 4*60*1000; //4min (should be >3min)
int SipTimers::D = 60000; // 60s (should be >32s)
int SipTimers::E = _T1;
int SipTimers::F = 64 * _T1;
int SipTimers::G = _T1;
int SipTimers::H = 64*_T1;
int SipTimers::I = _T4;
int SipTimers::J = 64*_T1;
int SipTimers::K = _T4;

