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


/**
 * StateMachine.h
 * Template implementation of state machine logic.
 * The state machine has any number of states, "State",
 * any number of transitions between the states,
 * "StateTransition" and each transition has an action
 * that will be executed if the transition is taken.
 *
 * The type of the input the state machine will accept
 * is given as the only argument to the classes.
 *
 * State Machines are expected to ha
 *
 * ExampleSM.cxx is an example of how it can be used.
 * @author Erik Eliasson, eliasson@imit.kth.se
 * (C) 2003 Erik Eliasson
*/

#ifndef STATEMACHINE_H
#define STATEMACHINE_H

#include <libmutil/libmutil_config.h>

#include<string>
#include<list>
#include<libmutil/TimeoutProvider.h>
#include<libmutil/MemObject.h>
#include<libmutil/massert.h>

#ifdef MSM_DEBUG
#include<libmutil/dbg.h> 
#endif

template<class CommandType, class TimeoutType> class StateTransition;
template<class CommandType, class TimeoutType> class State;

/**
 * Implementation of a generic state machine.
 *
 * A special state, StateMachine::anyState with the name "libmutil_any",
 * can be used as from-state in a transition. Such a transition will
 * be tried regardless of the current state.
 *
 * If the anyState/("libmutil_any") is used as destination state, 
 * the state of the machine will not change. A transition from 
 * anyState to anyState can be used to trap commands without 
 * affecting the state.
 * 
 * Note 1:  State machines, states and transitions are using the MRef/MObject
 * classes to handle "garbage collection". The state machines and the
 * transitions reference each other. Therefore we must break the circle 
 * so that the it becomes a chain that is not referenced by anyone (and therefore will
 * be freed). For this purpose you have the freeStateMachine method which
 * you (unfortunately) must run on any object you want to be removed
 * from the heap.
 *
 * Note 2: The state machine calls an action method to 
 *  a) determine if the transaction should be done and 
 *  b) perform the action associated with the transition.
 * This is done in a single call to a action method, and 
 * therefore the state machine has not transitioned into 
 * it's new state when the action is executed. Special 
 * attention has to be given to situations where an action 
 * sends a new command to the same machine (recursion) since 
 * the current state of the machine has not yet been updated 
 * with the target state of the transition. 
 *
 * An action method for an integer state machine typically 
 * looks like:
 *   bool MyStateMachine::myTransitionAction(const int& input){
 *           if (input=='1'){
 *	             doActionMagic();
 *	             return true;
 *           }else{
 *                   return false;
 *           }
 *   }
 *   
*/
template<class CommandType, class TimeoutType> class StateMachine : public virtual MObject{
	public:

		MRef<State<CommandType, TimeoutType> *> anyState;
		
		/**
		 * Initializes the state machine to have no states and no
		 * transitions.
		 * 
		 * @param tp	Timeoutprovider that the state machine will
		 * 		use for timeouts.
		 */
		StateMachine( MRef<TimeoutProvider<TimeoutType, MRef<StateMachine<CommandType, TimeoutType> *>  > *> tp): 
				current_state(NULL), 
				timeoutProvider(tp)
		{
			anyState = new State<CommandType,TimeoutType>(this,"libmutil_any");
		}
					
		virtual ~StateMachine(){
		}

		/**
		 * De-allocates the state machine. After calling this
		 * method, the state machine can not be used.
		 *
		 * Purpose: The state machine contains "reference loops"
		 * that makes the MRef/MObject garbage collection not
		 * free the resources unless the "freeStateMachine" method
		 * has been called. 
		 * After calling this method, the state machine can be
		 * garbage collected as an ordinary MObject.
		 *
		 */
		void freeStateMachine(){
			current_state=NULL;
			timeoutProvider=NULL;
			anyState->freeState();
			anyState=NULL;

			for (typename std::list<MRef<State<CommandType,TimeoutType> *> >::iterator i=states.begin(); 
					i!=states.end(); i++){
				(*i)->freeState();			//Break the state<---->transition circle
			}
				
			states.clear();
		}
		
		std::string getMemObjectType() const {return "StateMachine";}
		
		/**
		 * Adds a state that will have no transitions connected to
		 * it to the state machine. If it is the first state added
		 * to the machine it will be set as the current state.
		 */
		void addState(MRef<State<CommandType, TimeoutType> *> state){
			if (!current_state)
				current_state = state;
			states.push_back(state);
		}

		/**
		 * When a state is created it is given a name. This method
		 * returns the first state with a matching name.
		 */
		MRef<State<CommandType, TimeoutType> *> getState(const std::string &name){
			for (typename std::list<MRef<State<CommandType,TimeoutType> *> >::iterator i=states.begin(); i!=states.end(); i++)
				if ((*i)->getName()==name)
					return *i;
			return NULL;
		}

		/**
		 * A state machine has a current state that can only be
		 * NULL if the state machine has no state. This method
		 * sets which state is the current one (the state that
		 * the machine is in). Although allowed, a user of this 
		 * class should not set the current state to be
		 * anyState ("libmutil_any").
		 */
		void setCurrentState(MRef<State<CommandType,TimeoutType> *> state){
			current_state = state;
		}
		
		/**
		 * Each state is assigned a name (that is not required to be
		 * unique) when it is created. This method returns the name
		 * assigned to the state that the machine is in.
		 */
		std::string getCurrentStateName() const{
			return current_state ? current_state->getName() : "";
		}

		/**
		 * Handles input to the state machine. The machine can
		 * react on the input depending on which state it is
		 * in and which transitions that state has.
		 *
		 * A state machine will try (in the following order) until
		 * a the action associated with the transition returns true
		 * or all has been tried:
		 *  1. All transitions in the current state in the
		 *     order they were added.
		 *  2. All transitions in the anyState/("libmutil_any") 
		 *     state in the order they were added.
		 * 
		 * @return TRUE is returned if a transition/action was
		 * 	   taken and FALSE if no transition was triggered.
		 */
		virtual bool handleCommand(const CommandType &command){
			if (current_state){
				return current_state->handleCommand(command) || anyState->handleCommand(command);
			}else{
				return false;
			}
		}

		/**
		 * Requests a timeout that will be sent to this state
		 * machine.
		 */
		void requestTimeout(int32_t ms, const TimeoutType &command){
			timeoutProvider->requestTimeout(ms, this, command);
		}

		/**
		 * Cancels a previously requested timeout. If the timeout
		 * does not exist (it might have expired/fired) this
		 * call has no effect.
		 */
		void cancelTimeout(const TimeoutType &command){
			timeoutProvider->cancelRequest(this, command);
		}

		MRef< TimeoutProvider<TimeoutType, MRef<StateMachine<CommandType,TimeoutType> *> > *> getTimeoutProvider(){return timeoutProvider;}

		virtual void handleTimeout(const TimeoutType &){std::cerr <<"WARNING: UNIMPLEMENTED handleTimeout"<<std::endl;};

		/**
		 * This method should not be called by an application. It
		 * is required by Timeoutprovider.
		 */
		void timeout(const TimeoutType &command){handleTimeout(command);};

	private:
		std::list<MRef<State<CommandType,TimeoutType>*> > states;
		MRef<State<CommandType,TimeoutType>*> current_state;
		MRef< TimeoutProvider<TimeoutType, MRef<StateMachine<CommandType,TimeoutType> *> > *> timeoutProvider;
		
};

template<class CommandType, class TimeoutType>
class State : public MObject{
	public:
		State(MRef<StateMachine<CommandType,TimeoutType> *> stateMachine_, 
				const std::string &name_):
					stateMachine(stateMachine_),
					name(name_)
		{}

		~State(){
			freeState();
		}

		void freeState(){
			stateMachine=NULL; 
			transitions.clear();	
		}

		std::string getMemObjectType() const {return "State";}
		
		void register_transition(MRef<StateTransition<CommandType, TimeoutType> *> transition){
			transitions.push_back(transition);
		}

		MRef<StateTransition<CommandType, TimeoutType> *> getTransition(const std::string &name_){
			for (typename std::list<MRef<StateTransition<CommandType,TimeoutType> *> >::iterator i=transitions.begin(); i!=transitions.end(); i++)
				if ((*i)->getName()==name_)
					return *i;
			return NULL;
		}

		bool removeTransition(const std::string &name_){
			for (typename std::list<MRef<StateTransition<CommandType,TimeoutType> *> >::iterator i=transitions.begin(); i!=transitions.end(); i++){
				if ((*i)->getName()==name_){
					transitions.erase(i);
					return true;
				}
			}
			return false;
		}

		
		bool handleCommand(const CommandType &command){
			for (typename std::list<MRef<StateTransition<CommandType,TimeoutType> *> >::iterator i=transitions.begin(); i!=transitions.end(); i++){
				if ((*i)->handleCommand(command)){
					return true;
				}
			}
			return false;
		}

		std::string getName(){
			return name;
		}
		
	private:
		MRef<StateMachine<CommandType, TimeoutType> *>stateMachine;
		std::string name;
		std::list<MRef<StateTransition<CommandType,TimeoutType>*> > transitions;
};


template<class CommandType, class TimeoutType>
class StateTransition : public MObject{
	public:
	
		StateTransition(MRef<StateMachine<CommandType, TimeoutType> *> stateMachine_,
				const std::string &name_,
				bool (StateMachine<CommandType, TimeoutType>::*a)(const CommandType& ),
				MRef<State<CommandType,TimeoutType> *> from_state_, 
				MRef<State<CommandType,TimeoutType> *> to_state_):
					stateMachine(stateMachine_), 
					name(name_), 
					action(a),
					from_state(from_state_),
					to_state(to_state_)
		{
			from_state->register_transition(this);
		}

		~StateTransition(){
			stateMachine=NULL;	//Note: Setting the references to null
			from_state=NULL;	//makes sure we have no circular referencing
			to_state=NULL; 
		}
				
		std::string getMemObjectType() const {return "StateTransition";}

		bool handleCommand(const CommandType &c){
			bool handled;
			massert(action!=(bool (StateMachine<CommandType,TimeoutType>::*)(const CommandType& ))NULL);
			if (handled= ((**stateMachine).*action)(c) ){
				if ( !(to_state == stateMachine->anyState) )
					stateMachine->setCurrentState(to_state);
#ifdef MSM_DEBUG
				if( outputStateMachineDebug ) {
					merr << "MSM_DEBUG: "<<from_state->getName()<<" --> "<< to_state->getName()<<":\t" << stateMachine->getMemObjectType() << ": transition: " << name;
		#ifdef MSM_DEBUG_COMMAND
					merr << " ("<< c << ")";
		#endif
					merr << std::endl;
				}
#endif
			}
#ifdef MSM_DEBUG
			else if( outputStateMachineDebug ) {
				
				//Activate if needed ... it produces quite some extra debug ... 
				//merr << "MSM_DEBUG: " << stateMachine->getMemObjectType() << ": Transition Failed: " << name << ": " << from_state->getName()
				//	<<" -> "<<to_state->getName() << end;
			}
#endif

			return handled;
		}

		std::string getName(){return name;}
	private:
		MRef<StateMachine<CommandType,TimeoutType> *> stateMachine;
		std::string name;
		bool (StateMachine<CommandType, TimeoutType>::*action)(const CommandType& );
		
		MRef<State<CommandType, TimeoutType> *>from_state;
		MRef<State<CommandType, TimeoutType> *>to_state;
};


#endif

