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

#include<list>
#include<libmutil/TimeoutProvider.h>
#include<libmutil/dbg.h>
#include<libmutil/MemObject.h>

//#define SM_DEBUG
#undef SM_DEBUG
//#define SM_DEBUG_COMMAND
#undef SM_DEBUG_COMMAND


using namespace std;

template<class CommandType, class TimeoutType> class StateTransition;
template<class CommandType, class TimeoutType> class State;

/**
 * Implementation of a generic state machine.
 * 
 * Note:  State machines, states and transitions are using the MRef/MObject
 * classes to handle "garbage collection". The state machines and the
 * transitions reference each other. Therefore we must break the circel 
 * so that the it becomes a chain that none references (and therefore will
 * be freed). For this purpose you have the freeStateMachine method which
 * you (unfortunately) must run on any object you want to be removed
 * from the heap.
*/
template<class CommandType, class TimeoutType> class StateMachine : public virtual MObject{
	public:
		StateMachine( MRef<TimeoutProvider<TimeoutType, MRef<StateMachine<CommandType, TimeoutType> *>  > *> tp): 
				current_state(NULL), 
				timeoutProvider(tp)
		{
		}
					
		virtual ~StateMachine(){
			freeStateMachine();	//Note: this is not needed and we should not 
						//run the destructor if it
						//has not already been run
		}

		void freeStateMachine(){
			current_state=NULL;
			timeoutProvider=NULL;

			for (typename list<MRef<State<CommandType,TimeoutType> *> >::iterator i=states.begin(); 
					i!=states.end(); i++){
				(*i)->freeState();			//Break the state<---->transition circle
			
			}
				
			states.clear();
		}
		
		string getMemObjectType(){return "StateMachine";}
		
		void addState(MRef<State<CommandType, TimeoutType> *> state){
			states.push_back(state);
		}

		MRef<State<CommandType, TimeoutType> *> getState(const string &name){
			for (typename list<MRef<State<CommandType,TimeoutType> *> >::iterator i=states.begin(); i!=states.end(); i++)
				if ((*i)->getName()==name)
					return *i;
			return NULL;
		}

		
		void setCurrentState(MRef<State<CommandType,TimeoutType> *> state){
			current_state = state;
		}
		
		string getCurrentStateName() const{
			return current_state->getName();
		}

		virtual bool handleCommand(const CommandType &command){
			if (current_state){
				return current_state->handleCommand(command);
			}else{
				return false;
			}
		}

		void requestTimeout(int32_t ms, const TimeoutType &command){
			timeoutProvider->request_timeout(ms, this, command);
		}
		void cancelTimeout(const TimeoutType &command){
			timeoutProvider->cancel_request(this, command);
		}

		MRef< TimeoutProvider<TimeoutType, MRef<StateMachine<CommandType,TimeoutType> *> > *> getTimeoutProvider(){return timeoutProvider;}

		virtual void handleTimeout(const TimeoutType &){cerr <<"WARNING: UNIMPLEMENTED handleTimeout"<<endl;};

		void timeout(const TimeoutType &command){handleTimeout(command);};

	private:
		list<MRef<State<CommandType,TimeoutType>*> > states;
		MRef<State<CommandType,TimeoutType>*> current_state;
		MRef< TimeoutProvider<TimeoutType, MRef<StateMachine<CommandType,TimeoutType> *> > *> timeoutProvider;
		
};

template<class CommandType, class TimeoutType>
class State : public MObject{
	public:
		State(MRef<StateMachine<CommandType,TimeoutType> *> stateMachine, 
				const string &name):
					stateMachine(stateMachine),
					name(name)
		{}

		~State(){
			freeState();
		}

		void freeState(){
			stateMachine=NULL; 
			transitions.clear();	
		}

		string getMemObjectType(){return "State";}
		
		void register_transition(MRef<StateTransition<CommandType, TimeoutType> *> transition){
			transitions.push_back(transition);
		}

		MRef<StateTransition<CommandType, TimeoutType> *> getTransition(const string &name){
			for (typename list<MRef<StateTransition<CommandType,TimeoutType> *> >::iterator i=transitions.begin(); i!=transitions.end(); i++)
				if ((*i)->getName()==name)
					return *i;
			return NULL;
		}

		bool removeTransition(const string &name){
			for (typename list<MRef<StateTransition<CommandType,TimeoutType> *> >::iterator i=transitions.begin(); i!=transitions.end(); i++){
				if ((*i)->getName()==name){
					transitions.erase(i);
					return true;
				}
			}
			return false;
		}

		
		bool handleCommand(const CommandType &command){
			for (typename list<MRef<StateTransition<CommandType,TimeoutType> *> >::iterator i=transitions.begin(); i!=transitions.end(); i++){
				if ((*i)->handleCommand(command)){
					return true;
				}
			}
			return false;
		}

		string getName(){
			return name;
		}
		
	private:
		MRef<StateMachine<CommandType, TimeoutType> *>stateMachine;
		string name;
		list<MRef<StateTransition<CommandType,TimeoutType>*> > transitions;
};


template<class CommandType, class TimeoutType>
class StateTransition : public MObject{
	public:

		StateTransition(MRef<StateMachine<CommandType, TimeoutType> *> stateMachine, 
				const string &name, 
				bool (StateMachine<CommandType, TimeoutType>::*a)(const CommandType& ),
				MRef<State<CommandType,TimeoutType> *> from_state, 
				MRef<State<CommandType,TimeoutType> *> to_state):
					stateMachine(stateMachine), 
					name(name), 
					action(a),
					from_state(from_state),
					to_state(to_state)
		{
			from_state->register_transition(this);
		}

		~StateTransition(){
			stateMachine=NULL;	//Note: Setting the references to null
			from_state=NULL;	//makes sure we have no circular referencing
			to_state=NULL; 
		}
				
		string getMemObjectType(){return "StateTransition";}

		bool handleCommand(const CommandType &c){
			bool handled;
				assert(action!=(bool (StateMachine<CommandType, TimeoutType>::*)(const CommandType& ))NULL);
				if (handled= ((**stateMachine).*action)(c) ){
			                stateMachine->setCurrentState(to_state);
#ifdef SM_DEBUG
					merr << "SM_DEBUG" << stateMachine->getId() << ": " << name << ": " << from_state->getName()
						<<" -> "<<to_state->getName();
#ifdef SM_DEBUG_COMMAND
					merr << " ("<< c << ")";
#endif
					merr << end;
#endif
				}
				return handled;
		}

		string getName(){return name;}
	private:
		MRef<StateMachine<CommandType,TimeoutType> *> stateMachine;
		string name;
		bool (StateMachine<CommandType, TimeoutType>::*action)(const CommandType& );
		
		MRef<State<CommandType, TimeoutType> *>from_state;
		MRef<State<CommandType, TimeoutType> *>to_state;
};


#endif

