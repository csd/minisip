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

SipTimers::SipTimers(){
	T1 = _T1;
	T2 = 4000;
	T4 = _T4;
	A = _T1; //=T1
	B = 64 * _T1;
	C = 4*60*1000; //4min (should be >3min)
	D = 60000; // 60s (should be >32s)
	E = _T1;
	F = 64 * _T1;
	G = _T1;
	H = 64*_T1;
	I = _T4;
	J = 64*_T1;
	K = _T4;

}
