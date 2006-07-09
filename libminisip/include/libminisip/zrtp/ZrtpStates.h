/*
  Copyright (C) 2006 Werner Dittmann 
 
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

/*
 * Authors: Werner Dittmann <Werner.Dittmann@t-online.de>
 */

#ifndef _ZRTPSTATES_H_
#define _ZRTPSTATES_H_


#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <stdint.h>

class ZrtpStateClass;
/**
 * This structure hold the state name as enum (int) number and the pointer to
 * the functions that handles the various triggers that can occur in a state.
 */
typedef struct  {
    int32_t stateName;
    int32_t (ZrtpStateClass::*handler)(void);
} state_t;

/**
 * Implement a simple state handling.
 *
 * This class provides functions that manages the states and the event handler
 * functions. Its a very simple implementation.
 */
class ZrtpStates {
 public:

    ZrtpStates(state_t* const zstates,
	       const int32_t numStates,
	       const int32_t initialState):  
	numStates(numStates), states(zstates), state(initialState) {};

    int32_t processEvent(ZrtpStateClass& zsc) { 
	DEBUGOUT((fprintf(stdout, "ZrtpStates::processEvent, state: %d\n", state)));
	return (zsc.*states[state].handler)(); };

    int32_t inState(const int32_t s) { return ((s == state)? true : false); };

    void nextState(int32_t s)        { state = s; };

 private:
    const int32_t numStates;
    const state_t* states;
    int32_t  state;

    ZrtpStates();
};

#endif	//ZRTPSTATES

