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

template<class CommandType, class TimeoutType> class StateMachine : public virtual MObject{
	public:
		StateMachine( MRef<TimeoutProvider<TimeoutType, MRef<StateMachine<CommandType, TimeoutType> *>    > *> tp): 
				current_state(NULL), 
				timeoutProvider(tp)
		{
//#ifdef SM_DEBUG
//			sm_id = ++sm_counter;
////			cerr << "SM_DEBUG"<<sm_id<<endl;		
//#endif		
		};
				
		virtual ~StateMachine(){//TODO: delete states and transitions
		}
//#ifdef SM_DEBUG
//		int getId()const {return sm_id;}
//#endif
		string getMemObjectType(){return "StateMachine";}
		
		
		
		void addState(State<CommandType, TimeoutType> *state){
			states.push_back(state);
		}

		State<CommandType, TimeoutType> *getState(const string &name){
			for (typename list<State<CommandType,TimeoutType> *>::iterator i=states.begin(); i!=states.end(); i++)
				if ((*i)->getName()==name)
					return *i;
			return NULL;
		}

		
		void setCurrentState(State<CommandType,TimeoutType> *state){
			current_state = state;
		}
		
		string getCurrentStateName() const{
			return current_state->getName();
		}

		virtual bool handleCommand(const CommandType &command){
			if (current_state != NULL){
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
		list<State<CommandType,TimeoutType> *> states;
		State<CommandType,TimeoutType> *current_state;
		MRef< TimeoutProvider<TimeoutType, MRef<StateMachine<CommandType,TimeoutType> *> > *> timeoutProvider;
//#ifdef SM_DEBUG
//		static int sm_counter;
//		int sm_id;
//#endif	
		
};

//template<class CommandType, class TimeoutType>
//int StateMachine<CommandType, TimeoutType>::sm_counter=0;

template<class CommandType, class TimeoutType>
class State{
	public:
		State(StateMachine<CommandType,TimeoutType> *stateMachine, 
				const string &name):
					stateMachine(stateMachine),
					name(name)
		{}
		
		void register_transition(StateTransition<CommandType, TimeoutType> *transition){
			transitions.push_back(transition);
		}

		StateTransition<CommandType, TimeoutType> * getTransition(const string &name){
			for (typename list<StateTransition<CommandType,TimeoutType> *>::iterator i=transitions.begin(); i!=transitions.end(); i++)
				if ((*i)->getName()==name)
					return *i;
			return NULL;
		}

		bool removeTransition(const string &name){
			for (typename list<StateTransition<CommandType,TimeoutType> *>::iterator i=transitions.begin(); i!=transitions.end(); i++){
				if ((*i)->getName()==name){
					transitions.erase(i);
					return true;
				}
			}
			return false;
		}

		
		bool handleCommand(const CommandType &command){
			for (typename list<StateTransition<CommandType,TimeoutType> *>::iterator i=transitions.begin(); i!=transitions.end(); i++){
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
		StateMachine<CommandType, TimeoutType> *stateMachine;
		string name;
		list<StateTransition<CommandType,TimeoutType> *> transitions;
};


template<class CommandType, class TimeoutType>
class StateTransition{
	public:

/*
		StateTransition(StateMachine<CommandType, TimeoutType> *stateMachine, 
				const string &name, 
				//bool (*action)( StateMachine<CommandType, TimeoutType> *, State<CommandType, TimeoutType> *, State<CommandType, TimeoutType> *, const CommandType&),
				bool (StateMachine<CommandType, TimeoutType>::*action)(State<CommandType, TimeoutType> *, State<CommandType, TimeoutType> *, const CommandType& ),
				State<CommandType, TimeoutType> *from_state, 
				State<CommandType, TimeoutType> *to_state):
					stateMachine(stateMachine), 
					name(name), 
					action_v1(action),
					action_v2(NULL),
					from_state(from_state),
					to_state(to_state)
		{
			from_state->register_transition(this);
		}
*/									
		StateTransition(StateMachine<CommandType, TimeoutType> *stateMachine, 
				const string &name, 
				//bool (*action)(StateMachine<CommandType, TimeoutType> *, const CommandType&),
				bool (StateMachine<CommandType, TimeoutType>::*action)(const CommandType& ),
				State<CommandType,TimeoutType> *from_state, 
				State<CommandType,TimeoutType> *to_state):
					stateMachine(stateMachine), 
					name(name), 
//					action_v1(NULL),
					action_v2(action),
					from_state(from_state),
					to_state(to_state)
		{
			from_state->register_transition(this);
		}
				

		bool handleCommand(const CommandType &c){
			bool handled;
/*			if (action_v1!=NULL){
				//if (handled=action_v1(stateMachine, from_state, to_state, c)){
				if (handled=( (*stateMachine).*action_v1)(from_state, to_state, c) ){
					stateMachine->setCurrentState(to_state);
#ifdef SM_DEBUG
					merr << "SM_DEBUG"<<stateMachine->getId()<<": "<< name << ": " << from_state->getName()
						<<" -> "<<to_state->getName();
#ifdef SM_DEBUG_COMMAND
					merr << " ("<< c << ")" << end;
#endif
					merr << end;
#endif
				}
				return handled;
			}else{
*/				//if (handled=action_v2(stateMachine, c)){
				assert(action_v2!=NULL);
				if (handled= ((*stateMachine).*action_v2)(c) ){
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
/*			}
*/		}

		string getName(){return name;}
	private:
		StateMachine<CommandType,TimeoutType> *stateMachine;
		string name;
		//bool (*action_v1)(StateMachine<CommandType,TimeoutType> *, State<CommandType,TimeoutType> *, State<CommandType,TimeoutType> *, const CommandType &);
//		bool (StateMachine<CommandType, TimeoutType>::*action_v1)(State<CommandType, TimeoutType> *, State<CommandType, TimeoutType> *, const CommandType& );
		//bool (*action_v2)(StateMachine<CommandType,TimeoutType> *, const CommandType &);
		bool (StateMachine<CommandType, TimeoutType>::*action_v2)(const CommandType& );
		
		State<CommandType, TimeoutType> *from_state;
		State<CommandType, TimeoutType> *to_state;
};


#endif
