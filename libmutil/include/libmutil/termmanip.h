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


#ifdef HAVE_CONFIG_H
#include<config.h>
#endif

//These definitions are only used when debugging is enabled.
//
#ifdef COLOR_TERMINAL

#define FG_BLACK "\033[30m"
#define FG_BLUE "\033[34m"
#define FG_BROWN "\033[33m"
#define FG_GREEN "\033[32m"
#define FG_MAGENTA "\033[35m"
#define FG_RED "\033[31m"
#define FG_WHITE "\033[37m"
#define FG_CYAN "\033[36m"

#define BG_BLACK "\033[40m"
#define BG_BLUE "\033[44m"
#define BG_BROWN "\033[43m"
#define BG_CYAN "\033[46m"
#define BG_GREEN "\033[42m"
#define BG_MAGENTA "\033[45m"
#define BG_RED "\033[41m"
#define BG_WHITE "\033[47m"

#define BOLD "\033[2m\033[1m"
#define BREW "\033[2m\033[1m\033[7m"
#define PLAIN "\033[m"
#define FG_ERROR FG_RED

#else


#define BOLD ""
#define BREW ""
#define PLAIN ""
#define FG_ERROR ""

#define FG_BLACK ""
#define FG_BLUE ""
#define FG_BROWN ""
#define FG_GREEN ""
#define FG_MAGENTA ""
#define FG_RED ""
#define FG_WHITE ""
#define FG_CYAN ""

#define BG_BLACK ""
#define BG_BLUE ""
#define BG_BROWN ""
#define BG_CYAN ""
#define BG_GREEN ""
#define BG_MAGENTA ""
#define BG_RED ""
#define BG_WHITE ""


#endif

